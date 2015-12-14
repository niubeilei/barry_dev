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
// 11/11/2012:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_Tester_HashMapTester_h
#define Omn_Util_Tester_HashMapTester_h

#include "Tester/TestPkg.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObjImp.h"
#include "Util/RCObject.h"
#include "Util/HashMap.h"
#include "Util/Tester/Ptrs.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;


class AosTmpTest : public OmnRCObject
{
	OmnDefineRCObject;

public:
	u64		mData;
	AosTmpTest(const u64 &v)
	{
		mData = v;
	}
};

class AosHashMapTester : public OmnTestPkg, public OmnThreadedObj
{
	enum
	{
		eMaxThreads = 100,
		eNumEntries = 1000000
	};

private:
	typedef AosHashMap<u64, AosTmpTestPtr, std::u64_hash, std::u64_cmp, 10000> 			map_t;
	typedef AosHashMap<u64, AosTmpTestPtr, std::u64_hash, std::u64_cmp, 10000>::iterator 	mapitr_t;
	OmnThreadPtr   	mThreads[eMaxThreads];
	int				mNumThreads;
	int				mNumThreadsRunning;
	map_t			mHashMap;
	int				mNumEntries;
	u64			*	mValues;
	bool		*	mStatus;

public:
	AosHashMapTester();
	~AosHashMapTester() {}

	virtual bool		start();

	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

private:
	bool basic();
	bool testOneRound();
	bool testIterator();
	bool testPerformance();
	bool testPerformance1();
	bool testMapPerformance();
};
#endif

