////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 06/03/2011: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "TransServer/TransServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ServerInfo.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include "TransServer/TransFileMgr.h"
#include "TransServer/TransSvrThrd.h"
#include "TransServer/ProcThrdMonitor.h"
#include "TransBasic/Trans.h"
#include "Util/File.h"

AosTransServer::AosTransServer()
:
mLock(OmnNew OmnMutex()),
mProcThrdNum(0),
mMaxCacheSize(0),
mCrtCacheSize(0),
mShowLog(false)
{
}


AosTransServer::~AosTransServer()
{
}


bool
AosTransServer::config(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	AosXmlTagPtr config = app_conf->getFirstChild(AOSCONFIG_TRANS_SERVER);
	aos_assert_r(config, false);

	mShowLog = config->getAttrBool(AOSCONFIG_SHOWLOG, false);
	mMaxCacheSize = config->getAttrU64(AOSCONFIG_TRANS_MAXCACHESIZE, 1000) * 1000000;
	
	//mProcThrdNum = config->getAttrU32(AOSTAG_PROC_THREAD_NUM, 10);
	mProcThrdNum = 2;	// Ketty temp. 1 is for app trans. 2 is for appMsg, trans ack, resp and sysTrans.
	if(mProcThrdNum > eMaxProcThrdNum)	mProcThrdNum = eMaxProcThrdNum;

	AosTransServerPtr trans_svr(this, false);
	for(u32 i=0; i<mProcThrdNum; i++)
	{
		mProcThrds[i] = OmnNew AosTransSvrThrd(trans_svr, eProcThrdId + i, mShowLog);
	}
	
	mProcThrdMonitor = OmnNew AosProcThrdMonitor(mProcThrdNum, mProcThrds);

	OmnString dir_name = OmnApp::getTransDir();
	mTransFileMgr = OmnNew AosTransFileMgr(trans_svr, dir_name, mShowLog);

	bool rslt = mTransFileMgr->getMaxRecvTid(mMaxRecvSeq);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosTransServer::switchFrom(const AosTransServerPtr &from)
{
	mShowLog = from->mShowLog;	
	mMaxCacheSize = from->mMaxCacheSize;
	
	AosTransServerPtr trans_svr(this, false);
	
	// from->mCrtCacheSize maybe not 0, contain Msg:eSwitchToMasterMsg(size:35)
	mCrtCacheSize = from->mCrtCacheSize;
	mProcThrdNum = from->mProcThrdNum;
	for(u32 i=0; i<mProcThrdNum; i++)
	{
		mProcThrds[i] = from->mProcThrds[i];
		mProcThrds[i]->resetTransSvr(trans_svr);
	}
	
	mProcThrdMonitor = from->mProcThrdMonitor;
	mProcThrdMonitor->resetProcThrds(mProcThrdNum, mProcThrds);

	mTransFileMgr = from->mTransFileMgr;
	mTransFileMgr->resetTransSvr(trans_svr);

	bool rslt = mTransFileMgr->getMaxRecvTid(mMaxRecvSeq);
	aos_assert_r(rslt, false);
	
	OmnScreen << "SWH; SwitchFrom; mMaxCacheSize:" << mMaxCacheSize << endl;
	return true;
}


bool
AosTransServer::start()
{
	bool rslt = startThrd();
	aos_assert_r(rslt, false);

	mTransFileMgr->start();

	//setActive();
	return true;
}


bool
AosTransServer::startThrd()
{
	// first need start this thrds.
	for(u32 i=0; i<mProcThrdNum; i++)
	{
		mProcThrds[i]->start();
	}
	mProcThrdMonitor->start();
	return true;
}


bool
AosTransServer::stop()
{
	for(u32 i=0; i<mProcThrdNum; i++)
	{
		mProcThrds[i]->stop();
	}
	mProcThrdMonitor->stop();

	mTransFileMgr->stop();
	return true;
}


bool
AosTransServer::reSet(const u64 &check_key)
{
	mLock->lock();
	map<u64, u64>::iterator itr = mMaxRecvSeq.find(check_key);
	itr->second = 0;
	mLock->unlock();
	return true;
}


bool
AosTransServer::recvMsg(const AosAppMsgPtr &msg)
{
	//if(mShowLog)
	//{
	//	OmnScreen << "TransServer; RecvMsg:"
	//		<< "; type:" << msg->getStrType()
	//		<< endl;
	//}
	if(!msg->isTrans())
	{
		//2013-09-16 Jozhi
		if (msg->getType() == MsgType::eAddServer)
		{
			OmnScreen << "*********************ReSet TransServer" << endl;
			//Linda, 2013/09/24
			//msg->proc();
			mCrtCacheSize += msg->getSize();
			msg->directProc();
			reSet(1);
			return true;
		}
		return pushMsgToQueue(msg);
	}

	AosTransPtr trans = (AosTrans *)msg.getPtr();
	return recvTrans(trans);
}


bool
AosTransServer::recvTrans(const AosTransPtr &trans)
{
	if(mShowLog)
	{
		OmnScreen << "TransServer; RecvTrans:"
			<< "; type:" << trans->getStrType()
			<< "; trans_id:" << trans->getTransId().toString() 
			<< "; cont:" << trans->toString()
			<< "; need_save:" << trans->isNeedSave() 
			<< "; is_resend:" << trans->isResend() 
			<< endl;
	}
	
	bool rslt;
	bool recved = isRecvBefore(trans);
	if(recved)	return trans->sendAck();
	
	// 2. setCrtRecvTrans.
	setMaxRecvPriv(trans);

	if(trans->isSystemTrans())
	{
		// if trans is sys trans. never ignore. so can't gerantee the order.
		rslt = pushMsgToQueue(trans.getPtr());
		aos_assert_r(rslt, false);
		return trans->sendAck();
	}
	
	rslt = tryPushTransToQueue(trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransServer::isRecvBefore(const AosTransPtr &trans)
{
	mLock->lock();
	AosTransId trans_id = trans->getTransId();
	u64 check_key = trans_id.getCltKey(); 

	map<u64, u64>::iterator itr = mMaxRecvSeq.find(check_key);
	if(itr == mMaxRecvSeq.end())
	{
		mLock->unlock();	
		return false;
	}

	u64 max_recv_seq = itr->second;
	mLock->unlock();
	
	if(trans_id.getSeqno() > max_recv_seq) return false;

	if(mShowLog)
	{
		OmnScreen << "TransServer; trans exist maybe reSend:"
			<< "; trans_id:" << trans_id.toString()
			<< "; max_recv_seq:" << max_recv_seq
			<< "; need_resp:" << trans->isNeedResp()
			<< endl;
	}
	return true;
}


bool
AosTransServer::setMaxRecvPriv(const AosTransPtr &trans)
{
	u64 clt_key = trans->getCltKey();
	AosTransId trans_id = trans->getTransId();
	u64 crt_seq = trans_id.getSeqno();
	
	map<u64, u64>::iterator itr = mMaxRecvSeq.find(clt_key);
	if(itr == mMaxRecvSeq.end())
	{
		mMaxRecvSeq.insert(make_pair(clt_key, crt_seq));
		return true;
	}
	
	aos_assert_r(itr->second < crt_seq, false);	
	itr->second = crt_seq;
	//if(mShowLog)
	//{
	//	OmnScreen << "TransServer; crt max recv seq:"
	//	  	<< "; clt_key:" << clt_key
	//		<< "; crt_seq:" << crt_seq
	//		<< endl;
	//}
	return true;
}


bool
AosTransServer::tryToSetMaxRecv(const u64 clt_key, const u64 crt_seq)
{
	if(crt_seq == 0)	return true;

	map<u64, u64>::iterator itr = mMaxRecvSeq.find(clt_key);
	if(itr == mMaxRecvSeq.end())
	{
		mMaxRecvSeq.insert(make_pair(clt_key, crt_seq));
		return true;
	}
	
	if(itr->second >= crt_seq)	return true;

	itr->second = crt_seq;
	if(mShowLog)
	{
		OmnScreen << "TransServer; crt max recv seq:"
		  	<< "; clt_key:" << clt_key
			<< "; crt_seq:" << crt_seq
			<< endl;
	}
	return true;
}


bool
AosTransServer::pushMsgToQueue(const AosAppMsgPtr &msg)
{
	mLock->lock();
	bool rslt = pushToQueueLocked(msg);
	mLock->unlock();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransServer::tryPushTransToQueue(const AosTransPtr &trans)
{
	bool rslt;
	mLock->lock();

	//if(mStatus != eActive || !mIgnoreTrans.empty() || 
	//		(mCrtCacheSize > mMaxCacheSize && !trans->isGetResp()) )
	if(hasIgnoreTrans() || isCacheFull())
	{
		// means has ignore trans. so this trans also need ignore.
		addIgnoreTransPriv(trans);
		mLock->unlock();
		return true;
	}

	if(trans->isNeedSave())
	{
		rslt = mTransFileMgr->saveToNormFile(trans);
		aos_assert_r(rslt, false);
	}

	rslt = pushToQueueLocked(trans.getPtr());
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	
	return trans->sendAck();
}


bool
AosTransServer::pushToQueueLocked(const AosAppMsgPtr &msg)
{
	aos_assert_r(msg, false);
	
	// The server don't hold the trans_buff.
	mCrtCacheSize += msg->getSize();
	
	AosTransSvrThrdPtr thrd = getProcThrd(msg);
	aos_assert_r(thrd, false);
	thrd->addMsg(msg);

	if(mShowLog)
	{
		OmnString trans_str;
		if(msg->isTrans())
		{
			AosTransPtr trans = (AosTrans *)msg.getPtr();
			trans_str << "; trans_id:" << trans->getTransId().toString();
		}
		OmnScreen << "TransServer. push to queue."
			<< "; type:" << msg->getStrType()
			<< trans_str 
			<< "; mCrtCacheSize:" << mCrtCacheSize
			<< endl;
	}
	return true;
}

void
AosTransServer::addIgnoreTransPriv(const AosTransPtr &trans)
{
	// means has ignore trans. so this trans also need ignore.
	set<AosTransId>::iterator itr = mIgnoreTransIds.find(trans->getTransId());
	if(itr != mIgnoreTransIds.end())	return;
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; ignore trans"
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; has_ignore:" << (!mIgnoreTrans.empty())
			<< "; cache is full:" << isCacheFull()
			<< endl;
	}

	mIgnoreTrans.push(trans);
	mIgnoreTransIds.insert(trans->getTransId());
}


void
AosTransServer::addIgnoreTrans(const AosTransPtr &trans)
{
	mLock->lock();
	addIgnoreTransPriv(trans);
	mLock->unlock();
}


AosTransSvrThrdPtr
AosTransServer::getProcThrd(const AosAppMsgPtr &msg)
{
	u32 proc_id;
	MsgType::E tp = msg->getType();
	if(tp == MsgType::eTransAckMsg || tp == MsgType::eTransResp ||
			tp == MsgType::eTransSvrDeathMsg || tp == MsgType::eTriggerResendMsg || 
			tp == MsgType::eSwitchToMasterMsg)
	{
		proc_id = 0;
		aos_assert_r(proc_id < mProcThrdNum && mProcThrds[proc_id], 0);
		return mProcThrds[proc_id];
	}

	proc_id = rand() % (mProcThrdNum - 1) + 1;
	aos_assert_r(proc_id < mProcThrdNum && mProcThrds[proc_id], 0);
	return mProcThrds[proc_id];
}


bool
AosTransServer::pushIgnoreTransToQueue()
{
	mLock->lock();
	bool rslt = pushIgnoreTransToQueuePriv();
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}

bool
AosTransServer::pushIgnoreTransToQueuePriv()
{
	if(!hasIgnoreTrans()) return true;

	// means has ignore trans.
	bool rslt;
	set<AosTransId>::iterator itr;
	while(!mIgnoreTrans.empty())
	{
		if(isCacheFull())	break;

		AosTransPtr trans = mIgnoreTrans.front();
		mIgnoreTrans.pop();
		itr = mIgnoreTransIds.find(trans->getTransId());
		aos_assert_r(itr != mIgnoreTransIds.end(), false);
		mIgnoreTransIds.erase(itr);
	
		if(trans->isNeedSave())
		{
			rslt = mTransFileMgr->saveToNormFile(trans);
			aos_assert_r(rslt, false);
		}

		rslt = pushToQueueLocked(trans.getPtr());
		aos_assert_r(rslt, false);
		trans->sendAck();
		if(mShowLog)
		{
			OmnScreen << "TransServer; the Ignore trans push to queue"
				<< "; trans_id:" << trans->getTransId().toString()
				<< endl;
		}
	}
	
	return true;	
}


bool
AosTransServer::finishTrans(vector<AosTransId> &trans_ids)
{
	return mTransFileMgr->finishTrans(trans_ids);
}


bool
AosTransServer::finishTrans(const AosTransPtr &trans)
{
	vector<AosTransId> v_tids;
	v_tids.push_back(trans->getTransId());
	return finishTrans(v_tids);
}

bool
AosTransServer::serializeMaxRecvSeq(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	mLock->lock();
	map<u64, u64>::iterator itr = mMaxRecvSeq.begin();
	for(; itr != mMaxRecvSeq.end(); itr++)
	{
		buff->setU64(itr->first);
		buff->setU64(itr->second);	
	}
	mLock->unlock();
	return true;
}


void
AosTransServer::resetCrtCacheSize(const u64 proced_msg_size)
{
	aos_assert(mCrtCacheSize >= proced_msg_size);
	mLock->lock();
	mCrtCacheSize -= proced_msg_size;
	mLock->unlock();

	if(mCrtCacheSize > eMinCacheSize)	return;
	
	bool rslt = contPushTransToQueue();
	aos_assert(rslt);
}


bool
AosTransServer::contPushTransToQueue()
{
	bool rslt = pushIgnoreTransToQueue();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransServer::isCacheFull()
{
	return mCrtCacheSize > mMaxCacheSize;
}


bool
AosTransServer::pushManyTransToQueue(vector<AosTransPtr> &v_trans)
{
	bool rslt;
	mLock->lock();
	for(u32 i=0; i<v_trans.size(); i++)
	{
		AosTransPtr tt = v_trans[i];
		
		//OmnScreen << "-------- recover trans:" 
		//	<< tt->getTransId().toString()
		//	<< "; type:" << tt->getStrType()
		//	<< endl;
		
		rslt = pushToQueueLocked(tt.getPtr()); 
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	
	return true;
}


bool
AosTransServer::hasIgnoreTrans()
{
	return !mIgnoreTrans.empty();
}


/*
bool
AosTransServer::procedMsgSize(const u64 proced_msg_size)
{
	//aos_assert_r(mCrtCacheSize >= proced_msg_size, false);
	
	mLock->lock();
	//Linda, 2013/09/25
	//mCrtCacheSize -= proced_msg_size;
	
	if(mStatus == eIdle)
	{
		mLock->unlock();
		return true;
	}

	if(mStatus == eRecovering)
	{
		// pose contine recover.
		mCondVarRecv->signal();
		mLock->unlock();
		return true;
	}
	
	bool rslt = pushIgnoreTransToQueue();
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}
*/
