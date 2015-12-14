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
// 2013/02/07	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILMgrBig_Testers_IILStrTester_h
#define Aos_IILMgrBig_Testers_IILStrTester_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"


class AosIILStrTester : public OmnTestPkg
{
public:

private:
	enum
	{
		eDftTries = 1000000,
		eDftSeedDataLen = 100,
		eDftKeyLen = 50,
		eDftBuffLen = 1000 * 1000 * 100		// 100M
	};

	AosRundataPtr		mRundata;
	int64_t				mTries;
	OmnString			mIILName;
	int					mSeedDataLen;
	u32					mKeyLen;
	u32					mRecordLen;
	u64					mBuffLen;

public:
	AosIILStrTester();
	~AosIILStrTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	config();
	bool 	tortureIndexing();
	bool 	indexOneTry();
	bool 	check();
};
#endif

