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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/Tester/RawSETester.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
using namespace std;

AosRawSETester::AosRawSETester()
{
	mName = "RawSETester";
	mTries = 100;
}


bool 
AosRawSETester::start()
{
	return basicTest();
}


bool
AosRawSETester::basicTest()
{
	if (mTestDurationSec > 0)
	{
		// Run the torturer for 'mTestDuration' amount of time.
		i64 crt_sec = OmnTime::getCrtSec();
		while (OmnTime::getCrtSec() < crt_sec + mTestDurationSec)
		{
			runOneTest();
		}
	}
	else
	{
		if (mTries <= 0) mTries = eDefaultTries;
		for (int i=0; i<mTries; i+=mBatchSize)
		{
			runOneTest(mBatchSize);
		}
	}

	return true;
}


bool
AosRawSETester::oneBatchTests(const int batch_size)
{
	for (int i=0; i<batch_size; i++)
	{
		switch (rand() % 4)
		{
		case 0:
			 testCreateData(...);
			 break;

		case 1:
			 testModifyData(...);
			 break;

		}
	}

	return true;
}


bool
AosRawSETester::testCreateData(AosRundata *rdata)
{
	// This function creates a raw file. 
	u32 siteid = pickSiteid();
	u32 cubeid = pickCubeid();
	u32 aseid  = pickAseid();
	AosBuffPtr data = createData();
	rdata->setSiteid();
	i64 call_id = mDataComm->createData(rdata, this, siteid, 
			cubeid, aseid, data->data(), data->dataLen());
	aos_assert_rr(call_id > 0, rdata, false);
	return true;
}


bool 
AosRawSETester::callSuccess(
		const AosRundataPtr &rdata, 
		const i64 call_id)
{
}


bool 
AosRawSETester::callFailed(
		const AosRundataPtr &rdata, 
		const i64 call_id)
{
}


