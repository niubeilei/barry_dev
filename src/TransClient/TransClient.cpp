////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransClient/TransClient.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "TransClient/TransIdMgr.h"
#include "TransClient/WaitInfo.h"
#include "Thread/ThreadMgr.h"
#include "TransBasic/Trans.h"
#include "TransUtil/TransRespTrans.h"
#include "Util/OmnNew.h"
#include "SysMsg/TriggerResendMsg.h"
#include "SysMsg/ResendEndMsg.h"

#include "XmlInterface/WebRequest.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>


static u64 sgTransCallerId = 1000;


AosTransClient::AosTransClient()
:
mLock(OmnNew OmnMutex()),
mResendCondVar(OmnNew OmnCondVar()),
mShowLog(false)
{
	mIdMgr = OmnNew AosTransIdMgr();
}


AosTransClient::~AosTransClient()
{
}


bool
AosTransClient::config(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	AosXmlTagPtr conf = app_conf->getFirstChild(AOSCONFIG_TRANS_SERVER);
	mShowLog = false;
	if (conf)
	{
		mShowLog = conf->getAttrBool(AOSCONFIG_SHOWLOG, false);
	}
	
	OmnThreadedObjPtr thisPtr(this, false);
	mReSendThrd = OmnNew OmnThread(thisPtr, "TransCltReSendFunc",
			eReSendThrdId, true, true, __FILE__, __LINE__);
	return true;
}
	

bool
AosTransClient::start()
{
	mReSendThrd->start();
	mIdMgr->start();	
	return true;
}


bool
AosTransClient::sendTransAsyncResp(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans)
{
	aos_assert_r(trans, false);
	aos_assert_r(trans->isNeedResp(), false);
	trans->setNeedAsyncResp();
	
	bool rslt = sendTransPriv(rdata, trans);
	aos_assert_r(rslt, false);

	if(mShowLog)
	{
		OmnScreen << "TransClient; send Trans, async resp:"
			<< "; trans_id:" << trans->getTransId().toString()
			<< endl;
	}

	return true;
}


bool
AosTransClient::sendTransSyncResp(
		const AosRundataPtr &rdata,
		vector<AosTransPtr> vt_trans)
{
	u64 t1 = OmnGetTimestamp();
	if(mShowLog)
	{
		OmnScreen << "TransClient; send Trans vt, sync resp:"
			<< endl;
	}

	OmnSemPtr sem = OmnNew OmnSem(0);
	bool rslt;
	for(size_t i=0; i<vt_trans.size(); i++)
	{
		AosTransPtr trans = vt_trans[i];
		trans->setNeedSyncResp();
		trans->setSem(sem);
		
		rslt = sendTransPriv(rdata, trans);
		aos_assert_r(rslt, false);
	}
	
	for (size_t i=0; i<vt_trans.size(); i++)
	{
		vt_trans[i]->waitResp();
	}
	
	if(mShowLog)
	{
		OmnScreen << "TransClient; send Trans vt, recv sync resps:"
			<< "; time:" << OmnGetTimestamp() -t1
			<< endl;
	}
	return true;
}


bool
AosTransClient::sendTrans(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans,
		AosBuffPtr &resp,
		bool &svr_death)
{
	aos_assert_r(trans, false);
	aos_assert_r(trans->isNeedResp(), false);
	trans->setNeedSyncResp();

	OmnSemPtr sem = OmnNew OmnSem(0);
	trans->setSem(sem);

	bool rslt = sendTransPriv(rdata, trans);
	aos_assert_r(rslt, false);

	u64 t1 = OmnGetTimestamp();
	if(mShowLog)
	{
		OmnScreen << "TransClient; send Trans, sync resp:"
			<< "; trans_id:" << trans->getTransId().toString()
			<< endl;
	}
	
	resp = trans->waitResp();
	svr_death = trans->isSvrDeath();
	if(!svr_death) aos_assert_r(resp, false);

	if(mShowLog)
	{
		OmnScreen << "TransClient; sendTrans, recv sync resp:"
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; time:" << OmnGetTimestamp() -t1
			<< endl;
	}
	return true;
}


bool
AosTransClient::sendTrans(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans,
		bool &svr_death)
{
	// means this trans not need resp. but need wait ack.
	aos_assert_r(trans, false);
	aos_assert_r(!trans->isNeedResp(), false);

	OmnSemPtr sem = OmnNew OmnSem(0);
	trans->setSem(sem);
	
	u64 t1 = OmnGetTimestamp();
	//if(mShowLog)
	//{
	//	OmnScreen << "TransClient; send Trans, wait ack:" << endl; 
	//}

	bool rslt = sendTransPriv(rdata, trans);
	aos_assert_r(rslt, false);

	if(mShowLog)
	{
		OmnScreen << "TransClient; send Trans, wait ack:"
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; time:" << OmnGetTimestamp() -t1
			<< endl;
			//<< resp_str << endl;
	}

	trans->waitAck();
	svr_death = trans->isSvrDeath();

	if(mShowLog)
	{
		OmnScreen << "TransClient; send Trans, recv ack wakeup:"
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; send_key:" << trans->getSendKey()
			<< "; time:" << OmnGetTimestamp() -t1
			<< endl;
	}
	return true;
}


bool
AosTransClient::sendTransPriv(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans)
{
	aos_assert_r(trans, false);

	mLock->lock();
	bool rslt = readyToSend(trans);

	if(trans->getTransId() == AosTransId::Invalid)
	{
		AosTransId trans_id = mIdMgr->nextTransId();
		aos_assert_rl(trans_id != AosTransId::Invalid, mLock, false);
		trans->setTransId(trans_id);
	}
	else
	{
		// user maybe resend trans. this trans has have trans id.
	}
	trans->setRundata(rdata);

	if(mShowLog)
	{
		OmnString resp_str;
		if(trans->getType() == AosTransType::eTransResp)
		{
			AosTransRespTrans * resp_trans = (AosTransRespTrans *)trans.getPtr();
			resp_str << "; It's resp."
				<< "; req_trans_id:" << resp_trans->getReqId().toString();
		}
		
		OmnScreen << "TransClient. sendTrans" 
			<< "; type:" << trans->getStrType()
			<< "; trans_id:" << trans->getTransId().toString()
			<< resp_str
			<< "; need_resp:" << trans->isNeedResp()
			<< endl;
	}

	addToWaitQueue(trans);

	rslt = AosSendMsg(trans.getPtr());
	aos_assert_rl(rslt, mLock, false);
	
	mLock->unlock();
	return true;
}


bool
AosTransClient::recvResp(
		const AosTransId &trans_id,
		const AosBuffPtr &resp,
		const int from_sid)
{
	if(mShowLog)
	{
		OmnScreen << "TransClient; recv resp:"
			<< "; trans_id:" << trans_id.toString()
			<< "; from_sid:" << from_sid
			<< endl;
	}
	aos_assert_r(trans_id != AosTransId::Invalid, false);

	mLock->lock();
	map<AosTransId, AosWaitInfoPtr>::iterator itr = mWaitMap.find(trans_id);
	//aos_assert_rl(itr != mWaitMap.end(), mLock, false);
	if(itr == mWaitMap.end())
	{
		// maybe resp from svrProxy. svrProxy not check resend.
		mLock->unlock();
		return true;
	}

	AosWaitInfoPtr wait_info = itr->second;
	wait_info->addAck(from_sid);	 // recv resp means recv ack.
	if(wait_info->isAckFinish())
	{
		removeFromWaitQueue(trans_id);
	}

	AosTransPtr trans = wait_info->getTrans();
	mLock->unlock();
	
	if(!trans->isNeedResp())
	{
		OmnScreen << "trans not need resp." << endl;
	}
	
	trans->respRecved(resp);
	return true;
}


bool
AosTransClient::recvAck(const AosTransId &trans_id, const int from_sid)
{
	if(mShowLog)
	{
		OmnScreen << "TransClient; recv ack:"
			<< "; trans_id:" << trans_id.toString()
			<< "; from_sid:" << from_sid
			<< endl;
	}
	aos_assert_r(trans_id != AosTransId::Invalid, false);

	mLock->lock();
	map<AosTransId, AosWaitInfoPtr>::iterator itr = mWaitMap.find(trans_id);
	if(itr == mWaitMap.end())
	{
		// means this trans has acked finish.
		mLock->unlock();
		return true;
	}

	AosWaitInfoPtr wait_info = itr->second;
	wait_info->addAck(from_sid);

	AosTransPtr trans = wait_info->getTrans(); 
	bool ack_finish = wait_info->isAckFinish();
	if(trans->isNeedResp())
	{
		if(trans->isRespRecved() && ack_finish)
		{
			// resp has come and ack_finish.
			removeFromWaitQueue(trans_id);
		}
		mLock->unlock();
		return true;
	}
	
	// this trans not need resp.
	if(ack_finish) removeFromWaitQueue(trans_id);
	trans->ackRecved();
	mLock->unlock();
	return true;
}


bool
AosTransClient::recvSvrDeath(const AosTransId &trans_id, const int death_sid)
{
	if(mShowLog)
	{
		OmnScreen << "TransClient; Trans: recv svr death:"
			<< "; trans_id:" << trans_id.toString()
			<< "; death_sid:" << death_sid
			<< endl;
	}
	aos_assert_r(trans_id != AosTransId::Invalid, false);

	mLock->lock();
	map<AosTransId, AosWaitInfoPtr>::iterator itr = mWaitMap.find(trans_id);
	if(itr == mWaitMap.end())
	{
		mLock->unlock();
		return true;
	}

	AosWaitInfoPtr wait_info = itr->second;
	wait_info->addDeathSvr(death_sid);
	
	AosTransPtr trans = wait_info->getTrans(); 
	bool ack_finish = wait_info->isAckFinish();
	bool all_svr_death = wait_info->isAllSvrDeath();
	if(!trans->isNeedResp())
	{
		if(ack_finish) removeFromWaitQueue(trans_id);
		mLock->unlock();
		
		if(all_svr_death) trans->setSvrDeath();
		return true;
	}

	// this trans need resp.
	if(trans->isCubeTrans() && !trans->isNeedSave() && !trans->isNeedResend())
	{
		// Ketty 2014/03/13
		// this is for task trans. who send big data to cube.
		// not need save but need resp.
		// this trans not need resend to bkp svr.
		removeFromWaitQueue(trans_id);
		mLock->unlock();
		
		trans->setSvrDeath();
		return true;
	}

	if(!ack_finish)
	{
		mLock->unlock();
		return true;
	}

	if(trans->isRespRecved())
	{
		// means resp has recved and ack_finish.
		removeFromWaitQueue(trans_id);
		mLock->unlock();
		return true;
	}

	// means resp not recved. all ack has come(or death msg)
	if(!all_svr_death)
	{
		// wait for ack. maybe will wait after master switch.
		mLock->unlock();
		return true;
	}

	removeFromWaitQueue(trans_id);
	mLock->unlock();
	
	trans->setSvrDeath();
	return true;
}


bool
AosTransClient::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 thread_id = thread->getLogicId();
	if(thread_id == eReSendThrdId)
	{
		return resendThrdFunc(state, thread);
	}
	
	OmnAlarm << "Invalid thread logic id: " << thread->getLogicId() << enderr;
	return false;
}


bool
AosTransClient::triggerResend(const AosTriggerResendMsgPtr &msg)
{
	OmnScreen << "======== TransClient; triger resend"
		<< "; reason:" << msg->getReason() << endl;
	mLock->lock();
	mResendReq.push(msg);
	mResendCondVar->signal();
	mLock->unlock();
	return true;
}

bool
AosTransClient::resendThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosTriggerResendMsgPtr msg;
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mResendReq.empty())
		{
			mResendCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		
		msg = mResendReq.front();
		mResendReq.pop();
		mLock->unlock();
		
		resendAllTrans(msg);
	}

	return true;
}


bool
AosTransClient::resendAllTrans(const AosTriggerResendMsgPtr msg)
{
	//OmnScreen << "======= TransClient; resend all trans"
	//	<< "; reason:" << msg->getReason()
	//	<< "; resend_num:" << mWaitMap.size()
	//	<< endl;
	AosWaitInfoPtr wait_info;
	
	mLock->lock();
	map<AosTransId, AosWaitInfoPtr>::iterator itr = mWaitMap.begin();
	for(; itr != mWaitMap.end(); itr++) 
	{
		wait_info = itr->second;

		if(mShowLog)
		{
			OmnScreen << "======= TransClient; resend Trans:"
				<< wait_info->toString()
				<< endl;
		}
		resendTransPriv(wait_info->getTrans());
	}
	mLock->unlock();
	
	if(!msg->isNeedResendEnd())	return true;
	
	int target_sid = msg->getTargetSvrId();
	aos_assert_r(target_sid != -1, false);

	OmnScreen << "======= TransClient; send resend end."
		<< "target_sid:" << target_sid << "; "
		<< "target_pid:" << msg->getTargetProcId() << "; "
		<< "is_ipc_conn:" << msg->isIpcConn()
		<< endl;
	AosAppMsgPtr end_msg = OmnNew AosResendEndMsg(
			target_sid, msg->getTargetProcId(), msg->isIpcConn(),
			AosGetSelfServerId(), AosGetSelfProcId());
	bool rslt = AosSendMsg(end_msg);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransClient::resendTransPriv(const AosTransPtr &trans)
{
	aos_assert_r(trans, false);
	
	//trans->setIsResend();		// will set the connBuff. when this trans is just sending. error!
	
	// tmp. resend Trans and the orig send trans will use the same connBuff
	// may be some error!. and the resend Trans will change rlbSeq(SockConn).
	// will change the connBuff's crtIdx.
	//trans->resetConnBuff();
	// the trans mem mRlbSeq will also change.
	AosConnMsgPtr c_msg = trans->copy();
	
	AosTransPtr new_trans = (AosTrans *)c_msg.getPtr(); 
	new_trans->setIsResend();
	if(trans->isRespRecved())	new_trans->setIsRespRecved();

	new_trans->saveAttrFlagToConnBuff();	// tmp. to save whether resp recved to buff. 
	
	//if(mShowLog)
	{
		OmnScreen << "======= TransClient; resend Trans:"
			<< new_trans->getTransId().toString()
			<< "; is_resp_recved:" << new_trans->isRespRecved()
			<< endl;
	}
	
	bool rslt = AosSendMsg(new_trans.getPtr());
	aos_assert_r(rslt, false);
	return true;
}
	

bool
AosTransClient::readyToSend(const AosTransPtr &trans)
{
	u64 key = trans->getSendKey();

	map<u64, CacheInfo>::iterator itr = mCacheSize.find(key);
	if(itr == mCacheSize.end())	return true;

	bool timeout;
	u64 crt_send_size = (itr->second).mSize;
	OmnCondVarPtr cond_var = (itr->second).mCondVar;

	while(crt_send_size >= eMaxCacheSize)
	{
		// OmnScreen << "TransClient max cache"
		// 	<< "; key:" << key << endl;
		

		// maybe can't send.
		cond_var->timedWait(mLock, timeout, 5);
		
		itr = mCacheSize.find(key);
		aos_assert_r(itr != mCacheSize.end(), false);	
		crt_send_size = (itr->second).mSize;
	}

	return true;
}

bool
AosTransClient::addToWaitQueue(const AosTransPtr &trans)
{
	AosWaitInfoPtr wait_info = OmnNew AosWaitInfo(trans, mShowLog);
	mWaitMap.insert(make_pair(trans->getTransId(), wait_info));
	//wait_info->setSendTime();
	
	u64 key = trans->getSendKey();
	u64 trans_size = trans->getSize();

	if(mShowLog)
	{
		OmnScreen << "TransClient; add to wait queue :"
			<< "; sendkey:" << trans->getSendKey() << ", "
			<< "; trans_id:" << trans->getTransId().toString() << ", "
			<< "; trans_size:" << trans_size << ", "
			<< endl;
	}

	
	map<u64, CacheInfo>::iterator itr = mCacheSize.find(key);
	if(itr == mCacheSize.end())
	{
		CacheInfo cc(trans_size);
		mCacheSize.insert(make_pair(key, cc));
		return true;
	}


	itr->second.mSize += trans_size;
	return true;
}

bool
AosTransClient::removeFromWaitQueue(const AosTransId &tid)
{
	map<AosTransId, AosWaitInfoPtr>::iterator w_itr = mWaitMap.find(tid);
	aos_assert_r(w_itr != mWaitMap.end(), false);
	
	AosTransPtr trans = (w_itr->second)->getTrans(); 
	aos_assert_r(trans, false);
	mWaitMap.erase(w_itr);

	u64 key = trans->getSendKey();
	u64 trans_size = trans->getSize();
	
	map<u64, CacheInfo>::iterator c_itr = mCacheSize.find(key);
	aos_assert_r(c_itr != mCacheSize.end(), false);
	c_itr->second.mSize -= trans_size;

	if(mShowLog)
	{
		OmnScreen << "TransClient; remove from wait queue :"
			<< "; sendkey:" << trans->getSendKey() << ", "
			<< "; trans_id:" << trans->getTransId().toString() << ", "
			<< "; trans_size:" << trans_size << ", "
			<< endl;
	}

	if(c_itr->second.mSize <= eMaxCacheSize / 2)
	{
		c_itr->second.mCondVar->signal();
	}
	return true;
}

bool
AosTransClient::transFinished(
        const AosRundataPtr &rdata,
        const u64 trans_id,
        const u64 caller_id,
        const bool status,
        const OmnString &errmsg,
        const AosBuffPtr &data)
{
    mLock->lock();
    map<u64, CallerEntry>::iterator itr = mTransCallers.find(caller_id);
    if (itr == mTransCallers.end())
    {
        OmnAlarm << "Caller not found: " << caller_id << enderr;
        mLock->unlock();
        return false;
    }

    AosTransCallerObjPtr caller = itr->second.caller;
    mTransCallers.erase(itr);
    mLock->unlock();
    aos_assert_rr(caller, rdata, false);
    caller->transFinished(rdata, trans_id, caller_id, status, errmsg, data);
    return true;
}

/*
// Chen Ding, 2013/09/28
u64 
AosTransClient::addAsyncRespCaller(
		const AosRundataPtr &rdata, 
		const AosAsyncRespCallerPtr &caller)
{
	mLock->lock();
	u64 caller_id = sgTransCallerId++;
	mAsyncTransCallers[caller_id] = caller;
	mLock->unlock();
	return caller_id;
}


bool
AosTransClient::removeAsyncRespCaller(
		const AosRundataPtr &rdata, 
		const u64 caller_id)
{
	mLock->lock();
	mAsyncTransCallers.erase(caller_id
	mLock->unlock();
	return true;
}


AosAsyncRespCallerPtr
AosTransClient::getAsyncRespCaller(
		const AosRundataPtr &rdata, 
		const u64 caller_id)
{
	mLock->lock();
	map<u64, AosAsyncRespCallerPtr>::iterator itr = mAsyncTransCallers.find(caller_id);
	if (itr == mAsyncTransCallers.end())
	{
		OmnAlarm << "Caller not found: " << caller_id << enderr;
		mLock->unlock();
		return false;
	}

	AosAsyncRespCallerPtr caller = itr->second.caller;
	mLock->unlock();
	aos_assert_rr(caller, rdata, false);
	caller->transFinished(rdata, trans_id, caller_id, status, errmsg, data);
	return true;
}
*/
