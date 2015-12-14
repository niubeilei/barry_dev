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
#ifndef Aos_CounterClt_Tester_CounterTester_h
#define Aos_CounterClt_Tester_CounterTester_h

#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "CounterClt/Tester/Ptrs.h"
#include "CounterTime/TimeInfo.h"
#include "CounterClt/Tester/CounterTesterThrd.h"
#include <map>

using namespace std;

class AosCounterTester : public OmnTestPkg
{
public:
	enum
    {
        eNumCounters = 5,
        eMaxSegments = 10,
		
		eNumTimeCounters = 3,
		eMaxTimeSegments = 3,

		eCheckCounter = 100,
		eBaseValue = 1000000,
		eDefaultTries = 1000000,
		eTimeOut = 1000
    };

	struct TimeStruct
	{
		u32 year;
		int64_t value;
		int64_t month[12];
		int64_t day[366];
	};

	struct CounterSturct
	{
		OmnString cType;
		OmnString cName;
		int64_t value;
		TimeStruct times[eMaxSegments];
	};

private:
	OmnMutexPtr                 mLock;
	OmnCondVarPtr				mCondVar;
	OmnString					mName;
    OmnString                   mCnames[eNumCounters];
    OmnString                   mSegments[eNumCounters][eMaxSegments];
    map<OmnString, int64_t>     mCounterMap;
	vector<AosCounterTesterThrdPtr>	mThreads;
	vector<OmnTestcase>			mTestcase;

	static int					mNumThreads;
	static int					mNumFinished;

	// base on the time
	CounterSturct				mRecordCounters[eNumCounters];

public:

	enum E
	{
		eProcThrdId
	};


public:
	AosCounterTester();
	~AosCounterTester() {}

	virtual bool		start();

	bool			basicTest(const int thrdid);

	bool threadFinished(const int thrdid);
	AosCounterTesterPtr clone(){ return OmnNew AosCounterTester();}

private:
	bool 	simpleCounterTest();
	bool 	counterTest();
	bool	timeCounterTest();
	bool    incorrectCounterTest();

	//simple
	bool	initSimplemData();
	bool    updateCounters(
				const int idx,
				const int64_t &value);
	bool	verifyCounters(const int number, const AosRundataPtr &rdata);

	// base on time
	bool	initTimemData();
	bool    updateTimeCounter(
                const int idxName,
                const int idxTime,
                const int idxMonth,
                const int idxDay,
				const int64_t value);

	bool 	verifyTimeCounter();
	bool    verifyTimeCounter(
                const int idxName,
	            const int idxTime,
	            const int idxMonth,
                const AosTimeInfo &date);

};
#endif

