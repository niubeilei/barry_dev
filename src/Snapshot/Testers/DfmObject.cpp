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
// Modification History:
// 09/02/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/Testers/DfmObject.h"

#include "Rundata/Rundata.h"
#include "Snapshot/Testers/DfmIILHitTester.h"
#include "Snapshot/Testers/DfmIILStrTester.h"
#include "Snapshot/Testers/DfmIILU64Tester.h"
#include "Snapshot/Testers/DfmFixedDocTester.h"
#include "Snapshot/Testers/DfmNormalDocTester.h"

AosDfmObject::AosDfmObject()
{
	for (u32 i = 0; i < eNumObj; i++)
	{
		mDfmTest.push_back(0);
	}
}

AosDfmObject::~AosDfmObject()
{
}

bool
AosDfmObject::run(
		const u32 logicid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mDfmTest.size() == eNumObj, false);
	int r = rand() % eNumObj;
	AosDfmTesterPtr dfm_test;
	if (mDfmTest[r])
	{
		dfm_test = mDfmTest[r];
	}
	else
	{
		int b = rand() % 4;
		bool flag = false;
		if (flag)
		{
			if (b == 0)
			{
				dfm_test = OmnNew AosDfmIILHitTester(logicid, r, rdata);
			}

			if (b == 1)	
			{
				dfm_test = OmnNew AosDfmIILU64Tester(logicid, r, rdata);
			}

			if (b >= 2)	
			{
				dfm_test = OmnNew AosDfmIILStrTester(logicid, r, rdata);
			}
		}
		else
		{
			b = 2;
			if (b <= 1)
			{
				dfm_test = OmnNew AosDfmFixedDocTester(logicid, rdata);
			}
			else
			{
				dfm_test = OmnNew AosDfmNormalDocTester(logicid, rdata);
			}
		}
		mDfmTest[r] = dfm_test;
	}
	dfm_test->basicTest();
	return true;
}

bool
AosDfmObject::serializeFrom(const AosRundataPtr &rdata, const AosBuffPtr &buff)
{
	for (int i = 0; i<eNumObj; i++)
	{
		int e = buff->getInt(0);
		aos_assert_r(e == i, false);
		int b = buff->getInt(0);
		aos_assert_r(b, false);
		if (b == 1)
		{
			OmnString tt = buff->getOmnStr("");
			aos_assert_r(tt != "", false);

			AosDfmTesterPtr dfm_test;
			if (tt == "u64") dfm_test = OmnNew AosDfmIILU64Tester(rdata);
			if (tt == "hit") dfm_test = OmnNew AosDfmIILHitTester(rdata);
			if (tt == "str") dfm_test = OmnNew AosDfmIILStrTester(rdata);
			if (tt == "normal") dfm_test = OmnNew AosDfmNormalDocTester(rdata);
			if (tt == "fixed") dfm_test = OmnNew AosDfmFixedDocTester(rdata);
			dfm_test->serializeFrom(buff);
			mDfmTest[i] = dfm_test;
		}
		if (b == 2)
		{
			OmnScreen << "dddddddd:" << i << endl;
		}
	}
	return true;
}


bool
AosDfmObject::serializeTo(const AosBuffPtr &buff)
{
	AosDfmTesterPtr dfm_test;
	for (int i = 0; i<eNumObj; i++)
	{
		buff->setInt(i);
		if (mDfmTest[i])
		{
			buff->setInt(1);
			dfm_test = mDfmTest[i];
			OmnString tt = dfm_test->getType();
			aos_assert_r(tt != "", false);
			buff->setOmnStr(tt);
			dfm_test->serializeTo(buff);
		}
		else
		{
			buff->setInt(2);
		}
	}
	return true;
}
