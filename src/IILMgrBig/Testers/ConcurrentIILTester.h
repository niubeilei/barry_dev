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
// 2013/03/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILMgrBig_Testers_ConcurrentIILTester_h
#define Aos_IILMgrBig_Testers_ConcurrentIILTester_h

//#include "BitmapEngine/Ptrs.h"
//#include "BitmapEngine/Testers/Ptrs.h"
#include "DataTypes/DataColOpr.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Util/BuffArray.h"
#include <vector>
using namespace std;


class AosConcurrentIILTester : public OmnTestPkg
{
public:
	enum
	{
		eNumStrings = 1000,
		eDftTries = 200000,
		eBuffLength = 1000000,
		eDftBatchSize = 1000,
		eDftPageSize = 50,
		eDftRecordLen = 50
	};

private:
	AosBuffArrayPtr		mBuffArray;
	int					mRecordLen;
	AosRundataPtr		mRundata;
	int					mTries;
	OmnString			mIILName;
	AosCompareFunPtr	mCompareFunc;
	int					mBatchSize;
	int					mNumStrings;
	int					mKeyLen;
	int					mPageSize;
	int 				mAllNums;
	AosDataColOpr::E 	mOpr;
	AosIILUtil::AosIILIncType   mIncType;

public:
	AosConcurrentIILTester();
	~AosConcurrentIILTester() {}

	virtual bool		start();

private:
	bool	init();
	bool	basicTest();
	bool	config();
	bool	grandTorturer();
	bool 	addContents();
	bool 	checkResults();
	bool 	mergeIIL();
	int     pickOperation(const vector<int> &weights);
	bool	addToIIL(const AosBuffArrayPtr &buff_array, const AosRundataPtr &rdata);
};
#endif
