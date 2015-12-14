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
#ifndef AOS_TransServer_ProcThrdMonitor_h
#define AOS_TransServer_ProcThrdMonitor_h

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

class AosProcThrdMonitor: public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eMonitorThrdSleep = 10, 
	};

private:
	OmnThreadPtr		 mMonitorThrd;
	u32					 mThrdNum;
	AosTransSvrThrdPtr * mProcThrds;
	
public:
	AosProcThrdMonitor(const u32 thrd_num, AosTransSvrThrdPtr * thrds);
	~AosProcThrdMonitor();
	
	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread111(OmnString &err, const int thrdLogicId);
	virtual bool    signal(const int threadLogicId);

	bool	start();
	bool 	stop();

	void 	resetProcThrds(
				const u32 thrd_num,
				AosTransSvrThrdPtr * thrds);

};
#endif
