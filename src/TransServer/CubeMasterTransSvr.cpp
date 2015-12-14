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
#include "TransServer/CubeMasterTransSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "FmtMgr/MasterFmtSvr.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgr/SystemId.h"
#include "TransServer/TransFileMgr.h"
#include "TransBasic/Trans.h"

#include "SysMsg/TestTrans.h"

AosCubeMasterTransSvr::AosCubeMasterTransSvr()
:
AosCubeTransSvr(true),
mStartFinish(false),
mMergeFinish(false),
mMergeLock(OmnNew OmnMutex()),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	mFmtSvr = OmnNew AosMasterFmtSvr();
	mFmtSvrRaw = (AosMasterFmtSvr *)mFmtSvr.getPtr();
}


AosCubeMasterTransSvr::~AosCubeMasterTransSvr()
{
}


bool
AosCubeMasterTransSvr::start()
{
	// Tmp
	//return recoverTest6();
	
	bool rslt = mFmtSvrRaw->start();	// maybe wiil proc Fmt.
	aos_assert_r(rslt, false);

	rslt = startVfsMgr();
	aos_assert_r(rslt, false);

	// Ketty  Temp.
	rslt = recoverTrans();
	aos_assert_r(rslt, false);

	mTransFileMgr->start();
	mStartFinish = true;
	return true;
}


bool
AosCubeMasterTransSvr::recoverTrans()
{
	u64 t1 = OmnGetTimestamp(); 
	if(mShowLog)
	{
		OmnScreen << "CubeMasterTransSvr; SWH; start recoverTrans" << endl;
	}
	
	int read_id = -1;
	bool rslt = recoverPriv(read_id);
	aos_assert_r(rslt, false);

	mMergeLock->lock();
	rslt = mTransFileMgr->mergeNewerFiles();
	aos_assert_rl(rslt, mMergeLock, false);
	//mStartFinish = true;
	mMergeFinish= true;
	mMergeLock->unlock();
	
	// continue recover the newer files
	rslt = recoverPriv(read_id);
	aos_assert_r(rslt, false);
	mTransFileMgr->normReqReadFinish(read_id);

	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; recoverTrans finish."
			<< "; time:" << OmnGetTimestamp() - t1
			<< endl;
	}
	return true;	
}


bool 
AosCubeMasterTransSvr::recoverPriv(int &read_id)
{
	bool rslt, finish = false;
	AosBuffPtr trans_buffs;
	vector<AosTransPtr> vt_trans;
	
	while(!finish)
	{
		rslt = mTransFileMgr->readUnfinishTrans(trans_buffs, read_id, finish);
		aos_assert_r(rslt, false);
		if(!trans_buffs)	continue;
		
		while(1)
		{
			AosTransPtr tt = AosTrans::getNextTrans(trans_buffs);
			if(!tt)	break;	
			
			tt->setIsRecover();
			vt_trans.push_back(tt);
			if(vt_trans.size() < 10) continue;
			
			rslt = procRecoveredTrans(vt_trans);	
			aos_assert_r(rslt, false);
			vt_trans.clear();
		}
	}

	if(vt_trans.size())
	{
		rslt = procRecoveredTrans(vt_trans);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosCubeMasterTransSvr::procRecoveredTrans(vector<AosTransPtr> &v_trans)
{
	bool rslt, timeout;
	mLock->lock();
	while(isCacheFull())
	{
		if(mShowLog)
		{
			OmnScreen << "TransServer; recoverTrans; queue is full" 
				<< "; "<< endl;
		}
		mCondVar->timedWait(mLock, timeout, 5);
		continue;
	}
	mLock->unlock();

	rslt = pushManyTransToQueue(v_trans); 
	aos_assert_r(rslt, false);
	return true;
}

	
bool
AosCubeMasterTransSvr::contPushTransToQueue()
{
	if(!mStartFinish)
	{
		// pose contine recover.
		mLock->lock();	
		mCondVar->signal();
		mLock->unlock();
		return true;
	}
	
	bool rslt = pushIgnoreTransToQueue();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCubeMasterTransSvr::switchFrom(const AosCubeTransSvrPtr &from)
{
	aos_assert_r(!from->isMaster(), false);
	
	bool rslt = AosCubeTransSvr::switchFrom(from);
	aos_assert_r(rslt, false);
	
	// delay FmtSvr's switchFrom because FmtSvr's switch will take times.
	//rslt = mFmtSvrRaw->switchFrom(from->mFmtSvr);
	//aos_assert_r(rslt, false);
	mFmtSvrRaw->setSwitchFrom(from->getFmtSvr());
	
	OmnScreen << "!!!! clean first recv resp." << endl;
	mFirstRecvRespSeq.clear();
	
	return true;
}


bool
AosCubeMasterTransSvr::stopSendFmt()
{
	OmnScreen << "TransServer; SWH; old master stop send fmt." << endl;

	bool rslt = mFmtSvrRaw->stop();
	aos_assert_r(rslt, false);
	
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, false);
	rslt = vfs_mgr->switchToBkp();
	aos_assert_r(rslt, false);
	return true;
}

int
AosCubeMasterTransSvr::getCrtMaster()
{
	return AosGetSelfServerId(); 
}

bool
AosCubeMasterTransSvr::recvTrans(const AosTransPtr &trans)
{
	bool rslt;
	setFirstRecvTrans(trans);
	
	if(isRecvBefore(trans))
	{
		//trans->sendAck();
		rslt = procResendTrans(trans);
		aos_assert_r(rslt, false);
		return true;
	}

	setMaxRecvPriv(trans);
	if(trans->isSystemTrans())
	{
		// 3. if trans is sys trans. never ignore. so can't gerantee the order.
		rslt = pushMsgToQueue(trans.getPtr());
		aos_assert_r(rslt, false);
		trans->sendAck();
		return true;
	}
	
	//if(!mStartFinish)
	if(!mMergeFinish)
	{
		mMergeLock->lock();
		if(!mMergeFinish)
		{
			rslt = recvNewerTrans(trans);
			mMergeLock->unlock();
			return true;
		}
		mMergeLock->unlock();
	}
	
	if(!mStartFinish)
	{
		// will has second recover.
		addIgnoreTrans(trans);
		OmnScreen << "TransServer; master not started yet. ignore trans;"
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; type:" << trans->getStrType()
			<< endl;
		return true;
	}
	
	rslt = tryPushTransToQueue(trans);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosCubeMasterTransSvr::setFirstRecvTrans(const AosTransPtr &trans)
{
	if(trans->isResend() || !trans->isNeedResp() || trans->isNeedSave())	return true;
	
	AosTransId trans_id = trans->getTransId();
	u64 check_key = trans->getCltKey(); 
	u64 crt_seq = trans_id.getSeqno();
	
	map<u64, u64>::iterator itr = mFirstRecvRespSeq.find(check_key);
	if(itr == mFirstRecvRespSeq.end())
	{
		mFirstRecvRespSeq.insert(make_pair(check_key, crt_seq));
	}
	return true;
}


bool
AosCubeMasterTransSvr::procResendTrans(const AosTransPtr &trans)
{
	if(!trans->isResend())	return trans->sendAck();
	
	// just for proc trans. who need resp and resp don't recved.
	if(!trans->isCubeTrans() || !trans->isNeedResp() || trans->isRespRecved()) return trans->sendAck();
	if(trans->isNeedSave())	return trans->sendAck();
	
	u64 check_key = trans->getCltKey();	
	AosTransId trans_id = trans->getTransId();
	map<u64, u64>::iterator itr = mFirstRecvRespSeq.find(check_key);
	if(itr != mFirstRecvRespSeq.end() && trans_id.getSeqno() >= itr->second)
	{
		return trans->sendAck();
	}

	// this resend trans need proc.
	//if(mShowLog)
	{
		OmnScreen << "TransServer; trans resend. this trans need proc."
			<< "; trans_id:" << trans_id.toString()
			<< "; start_finish:" << mStartFinish
			<< endl;
	}
		
	bool rslt = tryPushTransToQueue(trans);	// maybe add to ignore Queue.
	aos_assert_r(rslt, false);
	return true;	
	/*
	if(mStartFinish)
	{
		bool rslt = tryPushTransToQueue(trans);	// maybe add to ignore Queue.
		aos_assert_r(rslt, false);
		return true;	
	}
	
	mLock->lock();
	mResendedTrans.insert(make_pair(trans_id, trans));
	mLock->unlock();
	return true;
	*/
}


bool
AosCubeMasterTransSvr::recvNewerTrans(const AosTransPtr &trans)
{
	if(!hasIgnoreTrans() && trans->isNeedSave())
	{
		OmnScreen << "TransServer; recv newer Trans. save to file. "
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; type:" << trans->getStrType()
			<< endl;
		bool rslt = mTransFileMgr->saveToNewerFile(trans);
		aos_assert_r(rslt, false);
		trans->sendAck();
		return true;
	}
	
	addIgnoreTrans(trans);
	OmnScreen << "TransServer; recv newer Trans. ignore trans;"
		<< "; trans_id:" << trans->getTransId().toString()
		<< "; type:" << trans->getStrType()
		<< endl;
	return true;
}


//==============================

/*
bool
AosCubeMasterTransSvr::reSwitch()
{
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, false);
	bool rslt = vfs_mgr->reStart();
	aos_assert_r(rslt, false);
	
	// 2. notify fmt_mgr.
	//AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	//aos_assert_r(fmt_mgr, false);
	//fmt_mgr->reSwitchToMaster();
	mFmtSvrRaw->reSwitch();

	startFinish();
	return true;
}
*/



/*
bool
AosCubeMasterTransSvr::procRecoveredTransBuffs(const AosBuffPtr &cont)
{
	aos_assert_r(cont, false);

	bool rslt;
	vector<AosTransPtr> vt_trans;
	while(1)
	{
		AosTransPtr tt = AosTrans::getNextTrans(cont);
		if(!tt)	break;	
		
		tt->setIsRecover();
		vt_trans.push_back(tt);
		
		if(vt_trans.size() >=10)
		{
			rslt = procRecoveredTrans(vt_trans);	
			aos_assert_r(rslt, false);
			vt_trans.clear();
		}
	}
	
	if(vt_trans.size())
	{
		rslt = procRecoveredTrans(vt_trans);
		aos_assert_r(rslt, false);
	}
	return true;
}
*/

bool
AosCubeMasterTransSvr::createTrans()
{
	//u32 buff_size = 100000000;	// 100M
	u32 buff_size = 1000000;	// 1M
	AosBuffPtr buff = OmnNew AosBuff(buff_size AosMemoryCheckerArgs);
	buff->setDataLen(buff_size);

	//for(u32 i=0; i<60; i++)		// 100M
	for(u32 i=0; i<6000; i++)		// 1M
	//for(u32 i=0; i<100; i++)		// 1M
	{
		AosTransPtr trans = OmnNew AosTestTrans(0, buff);
	
		AosTransId trans_id = {0, 2, i+1};
		trans->setTransId(trans_id);	
		bool rslt = mTransFileMgr->saveToNormFile(trans);
		aos_assert_r(rslt, false);
	}
	
	return true;
}


bool 
AosCubeMasterTransSvr::recoverTest1()
{
	createTrans();

	bool rslt, finish = false;
	AosBuffPtr trans_buffs;

	int read_id = -1;
	while(!finish)
	{
		rslt = mTransFileMgr->readUnfinishTrans(trans_buffs, read_id, finish);
		aos_assert_r(rslt, false);
		if(!trans_buffs)	continue;
		
		while(1)
		{
			AosTransPtr tt = AosTrans::getNextTrans(trans_buffs);
			if(!tt)	break;	
		}
	}

	OmnScreen << "bbbbbbbbbbbb" << endl;
	return true;
}



bool 
AosCubeMasterTransSvr::recoverTest2()
{
	createTrans();
	
	deque<AosTransPtr> my_que;
	recoverTesttt(my_que);

	OmnScreen << "aaaaaaaaaaaaa" << endl;
	while(!my_que.empty())
	{
		my_que.pop_front();
	}
	OmnScreen << "bbbbbbbbbbbb" << endl;

	return true;
}

bool 
AosCubeMasterTransSvr::recoverTest3()
{
	createTrans();
	
	recoverTesttt(mQueueTest);

	OmnScreen << "aaaaaaaaaaaaa" << endl;
	while(!mQueueTest.empty())
	{
		mQueueTest.pop_front();
	}
	OmnScreen << "bbbbbbbbbbbb" << endl;

	return true;
}


bool 
AosCubeMasterTransSvr::recoverTesttt(deque<AosTransPtr> &my_que)
{
	bool rslt, finish = false;
	AosBuffPtr trans_buffs;

	int read_id = -1;
	u32 num = 0;
	while(!finish)
	{
		rslt = mTransFileMgr->readUnfinishTrans(trans_buffs, read_id, finish);
		aos_assert_r(rslt, false);
		if(!trans_buffs)	continue;
		
		while(1)
		{
			AosTransPtr tt = AosTrans::getNextTrans(trans_buffs);
			if(!tt)	break;	
		
			num++;
			//recoverTestPriv(tt.getPtr(), my_que);
			my_que.push_back(tt);
		}
	}
	return true;
}


void
AosCubeMasterTransSvr::recoverTestPriv(
		const AosAppMsgPtr &msg,
		deque<AosAppMsgPtr> &my_que)
{
	my_que.push_back(msg);
}

	
bool 
AosCubeMasterTransSvr::recoverTest4()
{
	createTrans();
	
	vector<AosTransPtr> my_que;

	bool rslt, finish = false;
	AosBuffPtr trans_buffs;
	int read_id = -1;
	u32 num = 0;
	while(!finish)
	{
		rslt = mTransFileMgr->readUnfinishTrans(trans_buffs, read_id, finish);
		aos_assert_r(rslt, false);
		if(!trans_buffs)	continue;
		
		while(1)
		{
			AosTransPtr tt = AosTrans::getNextTrans(trans_buffs);
			if(!tt)	break;	
		
			num++;
			my_que.push_back(tt);
		}
	}

	OmnScreen << "aaaaaaaaaaaaa" << endl;
	my_que.clear();
	OmnScreen << "bbbbbbbbbbbb" << endl;

	return true;
}


bool 
AosCubeMasterTransSvr::recoverTest5()
{
	createTrans();
	
	bool rslt, finish = false;
	AosBuffPtr trans_buffs;

	AosTransPtr *trans = new AosTransPtr[6000];
	int idx = 0;

	int read_id = -1;
	while(!finish)
	{
		rslt = mTransFileMgr->readUnfinishTrans(trans_buffs, read_id, finish);
		aos_assert_r(rslt, false);
		if(!trans_buffs)	continue;
		
		while(1)
		{
			AosTransPtr tt = AosTrans::getNextTrans(trans_buffs);
			if(!tt)	break;	

			trans[idx++] = tt;
		}
	}

	for (int i=0; i<6000; i++) trans[i] = 0;

	delete [] trans;

	OmnScreen << "bbbbbbbbbbbb" << endl;
	return true;
}


bool 
AosCubeMasterTransSvr::recoverTest6()
{
	vector<AosTransPtr> my_que;
	
	//u32 buff_size = 100000000;	// 100M
	u32 buff_size = 1000000;	// 1M
	//AosBuffPtr buff = OmnNew AosBuff(buff_size AosMemoryCheckerArgs);
	//buff->setDataLen(buff_size);

	//for(u32 i=0; i<60; i++)		// 100M
	for(u32 i=0; i<6000; i++)		// 1M
	//for(u32 i=0; i<100; i++)		// 1M
	{
		AosBuffPtr buff = OmnNew AosBuff(buff_size AosMemoryCheckerArgs);
		buff->setDataLen(buff_size);
		AosTransPtr trans = OmnNew AosTestTrans(0, buff);
	
		AosTransId trans_id = {0, 2, i+1};
		trans->setTransId(trans_id);	
			
		my_que.push_back(trans);
	}
	
	OmnScreen << "aaaaaaaaaaaaa" << endl;
	my_que.clear();
	OmnScreen << "bbbbbbbbbbbb" << endl;
	
	return true;
}


