////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// Nov 17, 2015 created by tracy
////////////////////////////////////////////////////////////////////////////
#include "Util/JimoEncodingTester/JEUIATorturer_batch.h"
#include "Util/MemoryChecker.h"

AosJEUIATorturer_batch::AosJEUIATorturer_batch()
{
	cout << "==============Torturer=============" << endl;
}

AosJEUIATorturer_batch::~AosJEUIATorturer_batch()
{
}

bool
AosJEUIATorturer_batch::start(int runtimes)
{
	cout << "...... Start Variant Tester ......" << endl;
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mBuffRaw = mBuff.getPtr();

	basicTest(runtimes);

	return true;
}


bool
AosJEUIATorturer_batch::basicTest(int runtimes)
{
	cout << "...... Start Basic Tester ......" <<  endl;

	int count = 0;
	int tries = runtimes;

	while (tries--)
	{
		if (count % 100000 == 0)
		{
			cout << "Count: " << count << endl;
		}
		bool rslt = addValues();
		if (rslt == false)
		{
			OmnAlarm << ".........add values failed........" << enderr;
			return false;
		}
		rslt = verify();
		if(rslt == false)
		{
			OmnAlarm << "......decoding  failed..........."<< enderr;
			return false;
		}
		count++;
	}

	return true;

}


bool
AosJEUIATorturer_batch::addValues()
{
	int nn = OmnRandom::intByRange(
			1, 1, 10,
			2, 10, 50,
			11, 50, 100,
			101, 10000, 10);

	mValues.clear();
	mBuffRaw->clear();
	for (int i = 0; i < nn; i++)
	{
		bool rslt = addValue();
		aos_assert_r(rslt, false);

	}
	return true;
}


bool
AosJEUIATorturer_batch::addValue()
{
	switch(rand() % 4)
	{
	case 0:
		{
			u8 var_u8 = 0;
			var_u8 = rand();
			bool rslt = AosJEUIA::encode(var_u8, mBuffRaw);
			mValues.push_back(var_u8);
			aos_assert_r(rslt, false);

		}
		break;

	case 1:
		{
			u16 var_u16 = 0;
			var_u16 = rand();
			bool rslt = AosJEUIA::encode(var_u16, mBuffRaw);
			mValues.push_back(var_u16);
			aos_assert_r(rslt, false);
		}
		break;

	case 2:
		{
			u32 var_u32 = 0;
			var_u32 = rand();
			bool rslt = AosJEUIA::encode(var_u32, mBuffRaw);
			mValues.push_back(var_u32);
			aos_assert_r(rslt, false);

		}
		break;

	case 3:
		{
			u64 var_u64 = 0;
			var_u64 = rand();
			bool rslt = AosJEUIA::encode(var_u64, mBuffRaw);
			mValues.push_back(var_u64);
			aos_assert_r(rslt, false);
		}
		break;

	default:
		break;
	}

    return true;
}


bool
AosJEUIATorturer_batch::verify()
{
	u64 vv = 0;

	mBuff->reset();
	for (std::vector<u64>::iterator i = mValues.begin(); i != mValues.end(); i++)
	{
		u64 value = *i;
		AosJEUIA::decode(mBuffRaw, vv);
		aos_assert_r(vv == value, false);
	}
	return true;
}

