////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 2014/06/12 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_ResendCheckThrd_h
#define AOS_SvrProxyMgr_ResendCheckThrd_h

#include "alarm_c/alarm.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "SvrProxyUtil/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

#include "SysMsg/Ptrs.h"

#include <deque>
using namespace std;

class AosResendCheckThrd: public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eWaitResendTimeOut = 2,		// 1 sec.
	};
	
	struct WaitEntry
	{
		int 	svr_id;
		u32		proc_id;
		bool	wait_is_ipc_conn;
		u32		wait_id;
		u64		start_wait_time;
		u64		wait_timeout_time;
	};

private:
	OmnMutexPtr 	    mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	
	deque<WaitEntry> mProcQueue;
	

public:
	AosResendCheckThrd();
	~AosResendCheckThrd();
	
	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread111(OmnString &err, const int thrdLogicId){ return true; };
	virtual bool    signal(const int threadLogicId){ return true; };

	//bool	start();
	//bool 	stop();

	bool 	addWaitResendEnd(
				const int svr_id,
				const u32 proc_id,
				const bool wait_is_ipc_conn,
				const u32 wait_id);

	bool 	removeWaitResendEnd(
				const int svr_id,
				const u32 proc_id,
				const bool wait_is_ipc_conn,
				const u32 wait_id);

private:
	bool 	isWaitTimeout(WaitEntry &entry);
	void 	increaseTimeoutTime(WaitEntry &entry);

	bool 	notifyResendTrans(
				const int to_sid, 
				const u32 to_pid,
				const bool wait_is_ipc_conn,
				const u32 wait_id);

	AosTriggerResendMsgPtr createTriggerResendMsg(
				const int to_sid,
				const u32 to_pid,
				const bool wait_is_ipc_conn,
				const u32 wait_id);

};
#endif
