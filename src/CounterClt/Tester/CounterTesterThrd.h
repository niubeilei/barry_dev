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
// 2011/06/15	Created by Lynch 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterClt_Tester_CounterTesterThrd_h
#define Aos_CounterClt_Tester_CounterTesterThrd_h

#include "CounterClt/Tester/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "Util/Opr.h"
#include <map>

using namespace std;

class AosCounterTesterThrd : virtual public OmnRCObject,
							virtual public OmnThreadedObj
{
	OmnDefineRCObject;
	friend class AosCounterTester;

private:
	int					mThreadId;
	OmnThreadPtr       	mThread;
	AosCounterTesterPtr	mTester;

public:
	AosCounterTesterThrd(const AosCounterTesterPtr &tester, const int thrdid);
	~AosCounterTesterThrd() {}

	// OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state,
                               const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId){return true; };
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true; };
	int				getThreadId() const {return mThreadId;}
};
#endif

