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
// 09/03/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/XmlDocCacheTester.h"

#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

AosXmlDocCacheTester::AosXmlDocCacheTester()
:
mNumEntries(0),
mStopAdding(false)
{
	mName = "AosXmlDocCacheTester";

	for (int i=0; i<eArraySize; i++)
	{
		mArray[i] = new Entry();
	}
}


bool 
AosXmlDocCacheTester::start()
{
	return basicTest();
}


bool
AosXmlDocCacheTester::basicTest()
{
	int tries = mTestMgr->getTries();
	if (tries <= 0) tries = 1000000 * 200;

	mStopAdding = false;
	for (int i=0; i<tries; i++)
	{
if (!(mNumEntries >= 0 && (u32)mNumEntries == mMap.getNumNodes()))
	OmnMark;
		aos_assert_r(mNumEntries >= 0 && (u32)mNumEntries == mMap.getNumNodes(), false);
		aos_assert_r(mNumEntries >= 0 && mNumEntries <= eArraySize, false);
		if (mNumEntries == 0)
		{
			// cout << "Restore adding: " << i << endl;
			mStopAdding = false;
		}
		if (mNumEntries == eArraySize)
		{
			if (rand() % 100 <= eStopAddingWeight) 
			{
				// cout << "Stop adding: " << i << endl;
				mStopAdding = true;
			}
		}

		if (i % 1000000 == 0) cout << "Trying: " << i << ":" << mNumEntries << endl;
		int opr = rand() % 100;
		if (opr <= eAddWeight)
		{
			if (mStopAdding) continue;
			int nn = mNumEntries;
			bool added = false;
			aos_assert_r(addEntry(added), false);
			if (!added)
			{
				aos_assert_r(nn == mNumEntries, false);
			}
			else
			{
				if (nn == eArraySize)
				{
					aos_assert_r(nn == mNumEntries, false);
				}
				else
				{
					aos_assert_r(nn < eArraySize, false);
					aos_assert_r(nn + 1 == mNumEntries, false);
				}
			}
			continue;
		}

		if (opr <= eAddWeight + eDeleteWeight)
		{
			// int nn = mNumEntries;
			// bool deleted = false;
			aos_assert_r(deleteEntry(), false);

			continue;
		}

		opr += eDeleteWeight;
		if (opr <= eAddWeight + eDeleteWeight + eGetWeight)
		{
			aos_assert_r(getEntry(), false);
			continue;
		}
		
		aos_assert_r(modifyEntry(), false);
	}
	return true;
}


bool
AosXmlDocCacheTester::addEntry(bool &added)
{
	// It adds one entry to the map. 
	OmnString key;
	key = OmnRandom::word(eMinKeyLen, eMaxKeyLen-1);
	AosXmlTagPtr value = new AosXmlTag(AosMemoryCheckerArgsBegin);

	aos_assert_r(mArray[0], false);
	// Check whether the entry is already in the array
	for (int i=0; i<mNumEntries; i++)
	{
		if (mArray[i]->key == OmnString(key))
		{
			// It is already there. Do nothing.
			added = false;
			aos_assert_r(mArray[0], false);
			return true;
		}
	}

	bool rslt = mMap.addData(key, value);
	aos_assert_r(rslt, false);
	if (mNumEntries < eArraySize)
	{
		// Move the array
		mNumEntries++;
	}

	for (int i=mNumEntries-2; i>=0; i--) 
	{
		mArray[i+1]->key = mArray[i]->key;
		mArray[i+1]->value = mArray[i]->value;
	}

	mArray[0]->key = key;
	mArray[0]->value = value;
	added = true;
	aos_assert_r(mArray[0], false);
	return true;
}


bool
AosXmlDocCacheTester::deleteEntry()
{
	aos_assert_r(mArray[0], false);
	if (rand() % 100 < eDeleteInvalid || mNumEntries <= 0)
	{
		// Delete an invalid entry
		OmnString key;
		key = OmnRandom::word(eMinKeyLen, eMaxKeyLen-1);

		for (int i=0; i<mNumEntries; i++)
		{
			if (mArray[i]->key == OmnString(key)) 
			{
				aos_assert_r(mArray[0], false);
				return true;
			}
		}

		bool rslt = mMap.deleteData(key);
		aos_assert_r(!rslt, false);
		aos_assert_r(mArray[0], false);
		return true;
	}

	int idx = rand() % mNumEntries;
	bool rslt = mMap.deleteData(mArray[idx]->key);
if (!rslt)
	OmnMark;
	aos_assert_r(rslt, false);
	for (int i=idx; i<mNumEntries-1; i++) 
	{
		mArray[i]->key = mArray[i+1]->key;
		mArray[i]->value = mArray[i+1]->value;
	}
	mArray[mNumEntries-1]->key = "";
	mArray[mNumEntries-1]->value = 0;
	mNumEntries--;
	aos_assert_r(mArray[0], false);
	return true;
}


bool
AosXmlDocCacheTester::modifyEntry()
{
	aos_assert_r(mArray[0], false);
	if (mNumEntries <= 0) return true;
	int idx = rand() % mNumEntries;

	// It adds one entry to the map. 
	AosXmlTagPtr value = new AosXmlTag(AosMemoryCheckerArgsBegin);

	OmnString key = mArray[idx]->key;
	bool rslt = mMap.addData(mArray[idx]->key, value);
	aos_assert_r(rslt, false);

	// Move the array
	if (idx > 0)
	{
		for (int i=idx-1; i>=0; i--) 
		{
			mArray[i+1]->key = mArray[i]->key;
			mArray[i+1]->value = mArray[i]->value;
		}
	}

	mArray[0]->key = key;
	mArray[0]->value = value;
	aos_assert_r(mArray[0], false);
	return true;
}


bool
AosXmlDocCacheTester::getEntry()
{
	aos_assert_r(mArray[0], false);
	AosXmlTagPtr value = new AosXmlTag(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr value1 = value;

	if (rand() % 100 <= eGetValidDataWeight && mNumEntries > 0)
	{
		int idx = rand() % mNumEntries;
		bool rslt = mMap.getData(mArray[idx]->key, value);
		aos_assert_r(rslt, false);
		aos_assert_r(value == mArray[idx]->value, false);
		aos_assert_r(mArray[0], false);
		return true;
	}

	// Retrieve Invalid
	OmnString key;
	key = OmnRandom::word(eMinKeyLen, eMaxKeyLen-1);

	for (int i=0; i<mNumEntries; i++)
	{
		if (mArray[i]->key == OmnString(key)) 
		{
			aos_assert_r(mArray[0], false);
			return true;
		}
	}

	bool rslt = mMap.getData(key, value);
	aos_assert_r(!rslt, false);
	aos_assert_r(value == value1, false);
	aos_assert_r(mArray[0], false);
	return true;
}

