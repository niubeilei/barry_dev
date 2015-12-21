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
// 2013/02/12	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/Tester/BuffArrayTester.h"

#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "Util/CompareFun.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
using namespace std;

AosBuffArrayTester::AosBuffArrayTester()
{
	mName = "AosBufflTester";
	mTries = 100;
}

struct u64comp
{
	bool operator()(const char *s1, const char *s2)
	{
		return (*(u64 *)s1) < (*(u64 *)s2);
	}
};


bool AosBuffArrayTester::start()
{
	testInsert();
	return true;
}


bool
AosBuffArrayTester::testInsert()
{
	/*
	set<u64> data;
	vector<u64> datavec;

	AosFunU64U641Ptr comp_func = OmnNew AosFunU64U641();
	AosBuffArrayPtr buffarray = OmnNew AosBuffArray(comp_func, false, true, 100000);

	int tries = 1000000;
	u64 tt1 = OmnGetTimestamp();
	for (int i=0; i<tries; i++)
	{
		int opr = rand() % 10;
		if (opr == 0)
		{
			if (data.size() > 0)
			{
				aos_assert_r(datavec.size() > 0, false);
				int idx = rand() % datavec.size();
				u64 docid = datavec[idx];
//				OmnScreen << "Remove: " << docid << endl;
				datavec.erase(datavec.begin() + idx);
				data.erase(docid);
				bool rslt = buffarray->removeRecord((const char *)&docid);
				if (!rslt)
				{
					rslt = buffarray->removeRecord((const char *)&docid);
					rslt = buffarray->removeRecord((const char *)&docid);
					rslt = buffarray->removeRecord((const char *)&docid);
					rslt = buffarray->removeRecord((const char *)&docid);
				}
				aos_assert_r(rslt, false);
			}
		}
		else
		{
			u64 docid = rand();
			set<u64>::iterator itr = data.find(docid);
			if (itr == data.end())
			{
//				OmnScreen << "Insert: " << docid << endl;
				data.insert(docid);
				datavec.push_back(docid);
				bool rslt = buffarray->insertValue((const char *)&docid, sizeof(u64), 0);
				aos_assert_r(rslt, false);
			}
		}

		aos_assert_r(data.size() == datavec.size(), false);
		if (i % 100000 == 0) 
		{
			u64 tt2 = OmnGetTimestamp();
			OmnScreen << "Num Entries: " << i << ":" << tt2 -tt1 << endl;
			tt1 = tt2;
		}
	}
	*/

	return true;
}

#endif
