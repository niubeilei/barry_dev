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
// 08/09/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILMgr_Testers_IILTester_h
#define Aos_IILMgr_Testers_IILTester_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "IILMgr/IIL.h"
#include <vector>
using namespace std;


class AosIILTester : public OmnTestPkg
{
public:
	enum
	{
		eNumStrings = 1000,
		eDftTries = 1000000,
		eBuffLength = 1000000,
		eDftBatchSize = 100000,
		eDftRecordLen = 50
	};

private:

	AosRundataPtr			mRundata;
	int						mTries;
	OmnString				mIILName;

public:
	AosIILTester();
	~AosIILTester() {}

	virtual bool			start();

private:
	bool					init();
	bool					basicTest();
	bool					config();
	bool					grandTorturer();

	bool					addEntries(AosIILPtr &ill);
	AosIILPtr 				getIIL(
								const OmnString &iilname, 
								const u64 &illid);
	OmnString 				getRandomStr(u32 length);

};
#endif

