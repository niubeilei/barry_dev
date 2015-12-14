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
// 2014/06/12 	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/ResendCheckThrd.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"

#include "SEInterfaces/SvrProxyObj.h"
#include "SysMsg/TriggerResendMsg.h"

AosResendCheckThrd::AosResendCheckThrd()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "ResendCheckThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosResendCheckThrd::~AosResendCheckThrd()
{
}

/*
bool
AosResendCheckThrd::start()
{
	return mThread->start();
}


bool
AosResendCheckThrd::stop()
{
	return mThread->stop();
}
*/


bool
AosResendCheckThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	bool rslt, timeout;
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mProcQueue.empty())
		{
			//mCondVar->timedWait(mLock, timeout, 0, 60);
			mCondVar->timedWait(mLock, timeout, 10);
			mLock->unlock();
		
			continue;
		}
		
		deque<WaitEntry>::iterator itr = mProcQueue.begin();
		for(; itr != mProcQueue.end(); itr++)
		{
			WaitEntry &entry = *itr;	
			//if(!isWaitTimeout(entry.start_wait_time))	continue;
			if(!isWaitTimeout(entry))	continue;
		
			increaseTimeoutTime(entry);
			rslt = notifyResendTrans(entry.svr_id, entry.proc_id, 
				entry.wait_is_ipc_conn, entry.wait_id);
			aos_assert_r(rslt, false);
		}
		mLock->unlock();
		
		OmnSleep(eWaitResendTimeOut);
	}
	
	return true;
}


bool
AosResendCheckThrd::isWaitTimeout(WaitEntry &entry)
{
	return OmnGetTimestamp() - entry.start_wait_time > entry.wait_timeout_time;
}

void
AosResendCheckThrd::increaseTimeoutTime(WaitEntry &entry)
{
	u32 ff = entry.start_wait_time / eWaitResendTimeOut;
	ff *= 2;
	entry.wait_timeout_time += ff * eWaitResendTimeOut;
}


bool
AosResendCheckThrd::notifyResendTrans(
		const int to_sid, 
		const u32 to_pid,
		const bool wait_is_ipc_conn,
		const u32 wait_id)
{
	AosTriggerResendMsgPtr msg = createTriggerResendMsg(to_sid, to_pid,
			wait_is_ipc_conn, wait_id);
	aos_assert_r(msg, false);

	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);

	bool succ;
	bool rslt = svr_proxy->sendAppMsg(msg, succ);
	aos_assert_r(rslt, false);
	
	return true;
}


AosTriggerResendMsgPtr
AosResendCheckThrd::createTriggerResendMsg(
		const int to_sid,
		const u32 to_pid,
		const bool wait_is_ipc_conn,
		const u32 wait_id)
{
	OmnString reason;	
	if(wait_is_ipc_conn)
	{
		reason = "IpcSvrInfo wait resend end timeout.";
		reason << "svr_id:" << AosGetSelfServerId() << "; "
			<< "proc_id:" << wait_id;
	}
	else
	{
		reason = "NetSvrInfo wait resend end timeout.";
		reason << "svr_id:" << AosGetSelfServerId() << "; "
			<< "wait_svr_id:" << wait_id;
	}

	AosTriggerResendMsgPtr msg = OmnNew AosTriggerResendMsg(
			reason, AosGetSelfServerId(), wait_id, wait_is_ipc_conn);
	
	msg->setToSvrId(to_sid);
	msg->setToProcId(to_pid);		// direct let this proc to resend trans.
	return msg;
}


bool
AosResendCheckThrd::addWaitResendEnd(
		const int svr_id,
		const u32 proc_id,
		const bool wait_is_ipc_conn,
		const u32 wait_id)
{
	mLock->lock();

	deque<WaitEntry>::iterator itr = mProcQueue.begin();
	for(; itr != mProcQueue.end(); itr++)
	{
		WaitEntry &crt_entry = *itr;	
		if(crt_entry.svr_id == svr_id && crt_entry.proc_id == proc_id &&
				crt_entry.wait_is_ipc_conn == wait_is_ipc_conn && 
				crt_entry.wait_id == wait_id)
		{
			// exist
			mLock->unlock();
			return true;
		}
	}
	
	WaitEntry entry;
	entry.svr_id = svr_id;
	entry.proc_id = proc_id;
	entry.wait_is_ipc_conn = wait_is_ipc_conn;
	entry.wait_id = wait_id;
	entry.start_wait_time = OmnGetTimestamp(); 
	entry.wait_timeout_time = eWaitResendTimeOut;

	mProcQueue.push_back(entry);
	mCondVar->signal();
	mLock->unlock();
	
	return true;	
}


bool
AosResendCheckThrd::removeWaitResendEnd(
		const int svr_id,
		const u32 proc_id,
		const bool wait_is_ipc_conn,
		const u32 wait_id)
{
	mLock->lock();

	deque<WaitEntry>::iterator itr = mProcQueue.begin();
	for(; itr != mProcQueue.end(); itr++)
	{
		WaitEntry &crt_entry = *itr;	
		if(crt_entry.svr_id == svr_id && crt_entry.proc_id == proc_id &&
				crt_entry.wait_is_ipc_conn == wait_is_ipc_conn && 
				crt_entry.wait_id == wait_id)
		{
			mProcQueue.erase(itr);
			break;
		}
	}
			
	mLock->unlock();
	return true;
}

