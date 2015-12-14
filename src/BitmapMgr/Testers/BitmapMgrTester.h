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
// 01/06/2013	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapMgr_Tester_BitmapMgrTester_h
#define Aos_BitmapMgr_Tester_BitmapMgrTester_h

#include "QueryRslt/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"

#include <list>
using namespace std;

class OmnTestMgr;
struct Entry 
{
	u64 target;
	vector<u64> list;
};

class AosBitmapMgrTester : public OmnTestPkg
{
public:
	enum
	{
		eMaxDocid = 100000000,
		eDftBitBlockSize = 1000000,
		eDftMaxDocid = 1000000
	};
private:
	list<u64>			mSrcList;
	list<u64>			mSEList;
	list<u64>			mRList;
	map<u64, Entry*>	mEntryMap;
	u32 				mTries;
	u32					mBitBlockSize;
	u32					mMaxDocid;	
	AosRundataPtr 		mRundata;

public:
	AosBitmapMgrTester();
	~AosBitmapMgrTester(); 

public:
	bool 	start();
private:
	bool 				config();
	bool 				basicTest();
	bool				countStep1();
	bool				countStep2();
	bool				clearEntryMap();
	bool				checkCountRslt();
	bool				addEntry(const u64 &docid);
	bool				addEntry(const u64 &target, const vector<u64> &list);
	bool				isInList(const list<u64> &list, const u64 &docid);
	bool				testSections();
};
#endif
