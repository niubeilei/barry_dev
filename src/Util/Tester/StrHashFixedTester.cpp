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
// 01/13/2010:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/StrHashFixedTester.h"

#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/StrHashFixed.h"

static u64 sgAddTime = 0;
static u64 sgNextTime1 = 0;
static u64 sgNextTime2 = 0;
static u64 sgTotalAdd = 0;
static u64 sgTotalNext1 = 0;
static u64 sgTotalNext2 = 0;

bool AosStrHashFixedTester::start()
{
	// 
	// Test default constructor
	//
	cout << "    Start AosStrHash Tester...";
	torturer();
	return true;
}


bool 
AosStrHashFixedTester::torturer()
{
	OmnBeginTest << "Test";

	while (sgTotalNext1 < 4000000)
	{
		if (sgTotalNext1 % 100000 == 0)
		{
			OmnScreen << "Trying ... " << sgTotalNext1 << endl;
		}
		mHash.reset();
		mMap.clear();
		addEntries(mHash);
		nextEntries(mHash);

		// aos_assert_r(readEntry(hash), false);
	}

	OmnString ss1, ss2, ss3, ss4, ss5, ss6;
	ss1 << sgAddTime;
	ss2 << sgNextTime1;
	ss5 << sgNextTime2;
	ss3 << sgTotalAdd;
	ss4 << sgTotalNext1;
	ss6 << sgTotalNext2;
	ss1.convertToScientific();
	ss2.convertToScientific();
	ss3.convertToScientific();
	ss4.convertToScientific();
	ss5.convertToScientific();
	ss6.convertToScientific();
	OmnScreen << "Results:" << endl
		<< "    Add Time1:   " << ss1 << endl
		<< "    Next Time1:  " << ss2 << endl
		<< "    Next Time2:  " << ss5 << endl
		<< "    Total Add:   " << ss3 << endl
		<< "    Total Next1: " << ss4 << endl
		<< "    Total Next2: " << ss6 << endl;

	/*
	hash.printKeyDistribution();

	// For each key, retrieve it, remove it, retrieve it again. 
	group = 0;
	start_time = OmnGetTimestamp();
	for (int i=tries-1; i>=0; i--)
	{
		if (group++ == 100)
		{
			group = 0;
			u64 end_time = OmnGetTimestamp();
			OmnString ss;
			ss << 100 * 1000 * 1000 / (end_time - start_time);
			ss.convertToScientific();
			cout << "Deleting ... " << i << ", Rate: " << ss << endl;
		}
		key = mKeys[i];
		value = mValues[i];
if (!hash.get(key, value, false))
{
	hash.get(key, value, false);
}
		aos_assert_r(hash.get(key, value, false), false);
		aos_assert_r(hash.get(key, value, true), false);
		aos_assert_r(!hash.get(key, value, false), false);

		mNumEntries--;
		if (mNumEntries == 0) return true;
		aos_assert_r(readEntry(hash), false);
	}
	*/

	return true;
}


bool
AosStrHashFixedTester::nextEntries(OmnStrHashFixed<u64, 260, eBucketLen> &hash)
{
	OmnString key; 
	u64 value;
	u64 start_time = OmnGetTimestamp();
	while (hash.nextEntry(key, value)) 
	{
	/*
		itr_t itr = mMap.find(key);
		aos_assert_r(itr != mMap.end(), false);
		aos_assert_r(itr->second == value, false);
		mMap.erase(itr);
	*/
		sgTotalNext1++;
	}
	sgNextTime1 += OmnGetTimestamp() - start_time;
/*
	start_time = OmnGetTimestamp();
	itr_t itr = mMap.begin();
	while (itr != mMap.end())
	{
		sgTotalNext2++;
		++itr;
	}
	sgNextTime2 += OmnGetTimestamp() - start_time;
*/
	return true;
}


bool
AosStrHashFixedTester::addEntries(OmnStrHashFixed<u64, 260, eBucketLen> &hash)
{
	const int tries = 1000;
	OmnString key;
	char buff[1000];
	for (int i=0; i<tries; i++)
	{
		// if (group++ == 10000)
		// {
		// 	group = 0;
		// 	u64 end_time = OmnGetTimestamp();
		// 	OmnString ss;
		// 	ss << 100 * 1000 * 1000 / (end_time - start_time);
		// 	ss.convertToScientific();
		// 	start_time = end_time;
		// 	cout << "Add Entries ... " << i << ", Rate: " << ss << endl;
		// }

		int len;
		aos_assert_r(AosRandomLetterStr(1, 250, len, buff), false);
		u64 value = rand();

		key = buff;
		itr_t itr = mMap.find(key);
		if (itr == mMap.end())
		{
			// Not in the map. 
			aos_assert_r(!hash.get(key, value, false), false);
	u64 start_time = OmnGetTimestamp();
			hash.add(key, value);
	sgAddTime += OmnGetTimestamp() - start_time;
			mMap[key] = value;
			sgTotalAdd++;
		}
		else
		{
			u64 vv;
			aos_assert_r(hash.get(key, vv, false), false);
			aos_assert_r(vv == itr->second, false);
		}
	}
	return true;
}


bool
AosStrHashFixedTester::readEntry(OmnStrHashFixed<u64, 260, eBucketLen> &hash)
{
	/*
	aos_assert_r(mNumEntries > 0, false);
	const int tries = 100;
	char buff[1000];
	for (int k=0; k<tries; k++)
	{
		int len;
		aos_assert_r(AosRandomLetterStr(1, 300, len, buff), false);
		bool found = false;
		int found_idx = -1;
		for (int m=0; m<=mNumEntries; m++)
		{
			if (mKeys[m] == (const char *)buff) 
			{
				found = true;
				found_idx = m;
				break;
			}
		}

		if (found)
		{
			int value;
			aos_assert_r(found_idx >= 0 && found_idx <= mNumEntries, false);
			aos_assert_r(hash.get(buff, len, value, false), false);
if (value != mValues[found_idx])
{
	aos_assert_r(hash.get(buff, len, value, false), false);
}
			aos_assert_r(value == mValues[found_idx], false);
		}
		else
		{
			int value;
			aos_assert_r(!hash.get(buff, len, value, false), false);
		}
	}
	*/
	return true;
}

