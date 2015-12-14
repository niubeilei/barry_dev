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
#ifndef Aos_CounterTorturer_CounterTester_h
#define Aos_CounterTorturer_CounterTester_h

#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "CounterClt/Tester/Ptrs.h"
#include "CounterTime/CounterTimeInfo.h"
#include "CounterTorturer/CounterTesterThrd.h"
#include "UtilTime/TimeGran.h"
#include <map>

typedef map<OmnString, vector<OmnString> > 						CStr2VectorMap;
typedef map<OmnString, vector<OmnString> >::iterator 			CStr2VectorItr_t;


using namespace std;
class AosTime;

class AosCounterTester : public OmnTestPkg, public OmnThreadedObj
{
public:
	enum
    {
        eNumCounters = 5,
        eMaxSegments = 10,
		
		eNumTimeCounters = 4,
		eMaxTimeSegments = 3,

		eCheckCounter = 100,
		eBaseValue = 1000000,
		eDefaultTries = 1000000,
		eMaxThreadNum = 10,
		eTimeOut = 5 
    };

private:
	OmnMutexPtr                 mLock;
	OmnCondVarPtr				mCondVar;
	AosRundataPtr				mRundata;
	CStr2VectorMap				mCnames;
	vector<OmnString>			mCounterId;
	static int					mCounterId_Idx[eMaxThreadNum];
	map<OmnString, int64_t>		mValues;
	int							mCacheNum;
	static bool					mFlag[eMaxThreadNum];

public:
	AosCounterTester();
	~AosCounterTester(); 

	virtual bool		start();
	virtual bool		threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool		signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool			basicTest(const int thrdid);

	bool threadFinished(const int thrdid);
	AosCounterTesterPtr clone(){ return OmnNew AosCounterTester();}

private:
	bool 	basicTest1(const int thrdid);

	bool 	addCounter(const int thrdid);
	bool 	procCounter(const OmnString &cname,
					const AosRundataPtr &rdata);
	bool 	runQuery(const AosRundataPtr &rdata);
	OmnString composeSingleReq();
	OmnString composeMultiReq();
	OmnString composeSubReq();
	OmnString composeSub2Req();
	OmnString composeSub2Req2();
	bool	createDoc(const AosRundataPtr &rdata);

	bool 	updateCounter(const AosRundataPtr &rdata, const u32 &thrdid);	
	bool 	createCname(const u32 &thrdid, OmnString &cname, int64_t &value);
	int64_t retrieveSingleCounter(
				const AosRundataPtr &rdata,
				const u32 &thrdid,
				OmnString &counter_id);
	bool 	verifyValue(const OmnString &counter_id, const int64_t &value);
	bool 	addCounter(const AosRundataPtr &rdata, const OmnString &cname, const int64_t &value);

	bool 	basicTest2(const int thrdid);
	bool 	addCounter2(const AosRundataPtr &rdata);
	u16	createEntryType2(const int level);
	OmnString createCname2();
	bool runQuery2(const AosRundataPtr &rdata);

	bool basicTest3(const int thrdid);
	bool addCounter3(const AosRundataPtr &rdata);
	OmnString createKey3();
	bool runQuery3(const AosRundataPtr &rdata);
	OmnString composeMultiSubReq();

	bool basicTest4(const int thrdid);
	bool addCounter4(const AosRundataPtr &rdata);
	OmnString createCname4();
	bool runQuery4(const AosRundataPtr &rdata);
	OmnString composeMultiSub2Req();
	u16	createEntryType4(const int level);

	bool basicTest5(const int thrdid);
	bool addCounter5(const int thrdid);
	bool procCounter5(const OmnString &cname, const AosRundataPtr &rdata);
	bool runQuery5(const AosRundataPtr &rdata);
	OmnString composeSub2Req5();
	OmnString composeMultiSub2Req2();
	OmnString composeAll2Req2();


};
#endif

