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
#include "TransServer/TransSvrThrd.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "TransServer/TransServer.h"
#include "TransBasic/Trans.h"
#include "Util/OmnNew.h"

AosTransSvrThrd::AosTransSvrThrd(
		const AosTransServerPtr &trans_svr,
		const u32 thrd_id,
		const bool show_log)
:
mTransSvr(trans_svr),
mThrdId(thrd_id),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mStartProcTime(0),
mShowLog(show_log)
{
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "TransSvrThrd", thrd_id, true, true, __FILE__, __LINE__);
}


AosTransSvrThrd::~AosTransSvrThrd()
{
}


bool
AosTransSvrThrd::start()
{
	//mThrdStatus = false;	
	return mThread->start();
}


bool
AosTransSvrThrd::stop()
{
	//mThrdStatus = false;
	return mThread->stop();
}


void
AosTransSvrThrd::resetTransSvr(const AosTransServerPtr &trans_svr)
{
	mTransSvr = trans_svr;
}

bool
AosTransSvrThrd::isProcTooLong()
{
	mLock->lock();
	AosAppMsgPtr msg = mActiveMsg;
	mLock->unlock();
	
	u64 start_time = mStartProcTime;
	if(start_time == 0 || !msg) return false;

	u64 proc_time = OmnGetSecond() - start_time;
	if(proc_time > eTransMaxProcTime)
	{
		OmnString trans_str;
		if(msg->isTrans())
		{
			AosTransPtr trans = (AosTrans *)msg.getPtr();
			trans_str << "; trans_id:" << trans->getTransId().toString();
		}
		OmnScreen << "This msg:" 
				<< "; opr:" << msg->getType() 
				<< "; has proced too long! " << proc_time
				<< "; trans_str"
				<< "; thrd_id:" << mThrdId
				<< endl;
		return true;
	}

	return false;
}


bool
AosTransSvrThrd::addMsg(const AosAppMsgPtr &msg)
{
	aos_assert_r(msg, false);
	
	mLock->lock();
	mProcQueue.push_back(msg);
	mCondVar->signal();
	mLock->unlock();
	return true;	
}


bool
AosTransSvrThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 proc_time;
	bool timeout;
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mProcQueue.empty())
		{
			// temp
			//deque<AosAppMsgPtr>	tmp_que;
			//mProcQueue.swap(tmp_que);	

			mCondVar->timedWait(mLock, timeout, 1);
			mLock->unlock();
			
			// trigger proc Ignore Trans.
			mTransSvr->resetCrtCacheSize(0);
			
			continue;
		}
	
		AosAppMsgPtr msg = mProcQueue.front();
		mProcQueue.pop_front();
		int queue_size = mProcQueue.size();
		
		mActiveMsg = msg;
		mStartProcTime = OmnGetSecond();
		mLock->unlock();
	
		procMsgPriv(msg, proc_time, queue_size);

		mLock->lock();
		mActiveMsg = 0;
		mStartProcTime = 0;
		mLock->unlock();
	}

	return true;
}



/*
bool
AosTransSvrThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u64 proced_cache_size = 0;
	u32 proc_time;
	bool timeout;
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mProcQueue.empty())
		{
			//mCondVar->timedWait(mLock, timeout, 60);
			mCondVar->timedWait(mLock, timeout, 0, 10000);
			mLock->unlock();
		
			mTransSvr->procedMsgSize(proced_cache_size);
			proced_cache_size = 0;
			continue;
		}
	
		AosAppMsgPtr msg = mProcQueue.front();
		mProcQueue.pop_front();
		
		mActiveMsg = msg;
		mStartProcTime = OmnGetSecond();
		//u32 crt_qsize = mProcQueue.size();
		mLock->unlock();
	
		procMsgPriv(msg, proc_time);

		mLock->lock();
		mActiveMsg = 0;
		mStartProcTime = 0;
		mLock->unlock();
		
		proced_cache_size += msg->getSize(); 	
		if(proced_cache_size >= eMaxProcedMsgSize)
		{
			mTransSvr->procedMsgSize(proced_cache_size);
			proced_cache_size = 0;
		}
	
	}

	return true;
}


bool
AosTransSvrThrd::procMsgPriv(const AosAppMsgPtr &msg, u32 &proc_time)
{
	u64 t1 = OmnGetTimestamp(); 
	aos_assert_r(msg, false);
	
	OmnString trans_str;
	if(msg->isTrans())
	{
		AosTransPtr trans = (AosTrans *)msg.getPtr();
		if(!trans->isSystemTrans())
		{
			proc_time = OmnGetTimestamp() - t1;
			return true;
		}
	}
	
	if(mShowLog)
	{
		OmnScreen << "TransServer. proc msg."
			<< "; type:" << msg->getStrType()
			<< trans_str
			<< endl;
	}
	
	msg->proc();

	proc_time = OmnGetTimestamp() - t1;
	if(mShowLog)
	{
		OmnScreen << "TransServer. proc msg finish."
			<< "; type:" << msg->getStrType()
			<< trans_str
			<< "; time:" << proc_time 
			<< endl;
	}
	
	//if(msg->isTrans())
	//{
	//	AosTransPtr trans = (AosTrans *)msg.getPtr();
	//	if(trans->isFinishLater())	return true;
	//	
	//	bool rslt = AosFinishTrans(trans);
	//	aos_assert_r(rslt, false);
	//}
	return true;
}
*/


bool
AosTransSvrThrd::procMsgPriv(const AosAppMsgPtr &msg, u32 &proc_time, const int queue_size)
{
	u64 t1 = OmnGetTimestamp(); 
	aos_assert_r(msg, false);
	
	OmnString trans_str;
	if(msg->isTrans())
	{
		AosTransPtr trans = (AosTrans *)msg.getPtr();
		trans_str << "; trans_id:" << trans->getTransId().toString();
		if(!trans->isSystemTrans()) trans_str << "; norm_trans";
		/*
		if(trans->isGetResp())
		{
			if(mShowLog)
			{
				OmnScreen << "TransServer; trans is reSend to get resp."
					<< trans_str 
					<< endl;
			}

			trans->procGetResp();
			return true;
		}
		*/
	}
	
	if(mShowLog)
	{
		OmnScreen << "TransServer. proc msg start."
			<< "; type:" << msg->getStrType()
			<< "; " << trans_str
			<< "; QueueSize: " << queue_size
			<< endl;
	}
	
	//Linda, 2013/09/24
	//msg->proc();
	msg->directProc();

	if(mShowLog)
	{
		OmnScreen << "TransServer. proc msg finish."
			<< "; type:" << msg->getStrType()
			<< "; " << trans_str
			<< "; QueueSize: " << queue_size
			<< endl;
	}
	proc_time = OmnGetTimestamp() - t1;
	return true;
}

