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
#ifndef Aos_QueryRslt_Tester_BitmapNew_h
#define Aos_QueryRslt_Tester_BitmapNew_h

#include "QueryRslt/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"
#include <vector>
using namespace std;

class OmnTestMgr;
struct AosAndBlock
{
	u64 start;
	u64 end;
	AosAndBlock(const u64 s, const u64 e)
	:
	start(s),
	end(e)
	{}
	~AosAndBlock(){}
};

class AosBitmapTesterNew : public OmnTestPkg 
{
public:
	enum
	{
		eNumBitmaps = 20,
		eDftTries = 10000000,
		eMaxDocid = 0xffffffffffffffff
	};
	
	enum Operation
	{
		eCheckDoc,
		eRemoveDocid,
		eAppendDocid,
		eSaveToBuff,
		eLoadFromBuff,
		eNextDocid,
		eCountAndAnotB,
		eCountAnd,
		eCountOr,
		eCountAndBlock,
		eCountOrBlock,
		eClean,
		eNextNDocid,
		eGetDocids,
		eGetDocidCount,
		eGetSections,

		eNumOperations
	};

private:
	AosAndBlock*						mAndData;
	vector<AosBitmapObjPtr>		mBitmaps;
	vector<vector<u64> >				mDocids;
	vector<u64>							mOrDocids;
	int									mSanityCheckValue;
	int									mTries;
	int									mCrtTries;

public:
	AosBitmapTesterNew();
	~AosBitmapTesterNew(); 

public:
	bool 				start();
private:
	bool 				basicTest();
	bool 				initAndData();
	bool 				initData();
	bool 				checkDoc();
	bool 				checkAppendDocid();
	bool 				checkRemoveDocid();
	bool 				checkSaveToBuff();
	bool 				checkLoadFromBuff();
	bool 				checkNextDocid();
	bool 				checkCountAndAnotB();
	bool 				checkCountOr();
	bool 				checkCountAndBlock();
	bool 				checkCountOrBlock();
	bool 				checkNextNDocid();
	bool 				checkGetDocids();
	bool 				checkGetDocidCount();
	bool 				checkCountAnd();
	bool 				clean();
	bool 				checkAndAdd(const int idx, const u64 &docid);
	bool                checkAndRemove(const int idx, const u64 &docid);
	u64 				pickDocid();
	bool 				checkDocid(const u64 &docid, const vector<u64> &docids);
	bool				sanityCheck();
	bool 				sanityCheck(
				        	const vector<u64> &docids,
						    const AosBitmapObjPtr &bitmap);
	bool				unique(const vector<u64> &docids);
	bool				checkGetDocidCountNew();
};
#endif
