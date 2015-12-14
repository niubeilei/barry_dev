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
// 	Created: 03/29/2011 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransServer_TransSvrThrd_h
#define AOS_TransServer_TransSvrThrd_h

#include "alarm_c/alarm.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "TransUtil/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "SvrProxyUtil/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

#include <deque>
using namespace std;

class AosTransSvrThrd: public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eTransMaxProcTime = 5,
		//eMaxProcedMsgSize = 10000000,	// 10 M.
	};

private:
	AosTransServerPtr 	mTransSvr;
	u32					mThrdId;
	bool				mThrdStatus;
	OmnMutexPtr 	    mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	
	u64					mStartProcTime;
	AosAppMsgPtr		mActiveMsg;
	deque<AosAppMsgPtr>	mProcQueue;
	
	bool				mShowLog;
	
public:
	AosTransSvrThrd(const AosTransServerPtr &trans_svr, const u32 thrd_id, const bool show_log);
	~AosTransSvrThrd();
	
	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread111(OmnString &err, const int thrdLogicId){ return true; };
	virtual bool    signal(const int threadLogicId){ return true; };

	bool	start();
	bool 	stop();

	void 	resetTransSvr(const AosTransServerPtr &trans_svr);

	bool 	addMsg(const AosAppMsgPtr &msg);
	bool	isProcTooLong();

private:
	bool 	procMsgPriv(const AosAppMsgPtr &msg, u32 &proc_time, const int queue_size);

};
#endif
