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
// 2011/01/31	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SearchEngine_TesterNew_IILStrPerfTester_h
#define Aos_SearchEngine_TesterNew_IILStrPerfTester_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"


class AosIILStrPerfTester : public OmnTestPkg
{
public:

private:
	u64 				mIILID;
	u64 				mIILID2;
	AosIILStrPtr 		mIIL;
	AosIILStrPtr 		mIIL2;
	AosRundataPtr 		mRunData;

public:
	AosIILStrPerfTester();
	~AosIILStrPerfTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	randomValue();
	bool	sameValue();
};
#endif

