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
#include "Util/JimoEncodingTester/JEUIATorturer.h"
#include "Util/MemoryChecker.h"

AosJEUIATorturer::AosJEUIATorturer()
{
	cout << "==============Torturer=============" << endl;
}

AosJEUIATorturer::~AosJEUIATorturer()
{
}

bool
AosJEUIATorturer::start(int runtimes)
{
	cout << "...... Start Variant Tester ......" << endl;
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mBuffRaw = mBuff.getPtr();

	basicTest(runtimes);

	return true;
}


bool
AosJEUIATorturer::basicTest(int runtimes)
{
	cout << "...... Start Basic Tester ......" <<  endl;

	int count = 0;
	int tries = runtimes;

	while (tries--)
	{
		if (count % 1000000 == 0)
		{
			cout << "Count: " << count << endl;
		}
		bool rslt = addValue();
		if (rslt == false)
		{
			OmnAlarm << ".........add value failed........" << enderr;
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
AosJEUIATorturer::addValue()
{
	mflage = 0;
	mBuffRaw->clear();
	switch(rand() % 4)
	{
	case 0:
		{
			u8 var_u8 = 0;
			var_u8 = rand() % 236;
			bool rslt = AosJEUIA::encode(var_u8, mBuffRaw);
			mValueu8 = var_u8;
			mSizeMax = 1;
			mflage 	 = 8;
			aos_assert_r(rslt, false);
		}
		break;

	case 1:
		{
			u16 var_u16 = 0;
			var_u16 = rand();
			bool rslt = AosJEUIA::encode(var_u16, mBuffRaw);
			mValueu16 = var_u16;
			mSizeMax  = 3;
			mflage 	  = 16;
			aos_assert_r(rslt, false);
		}
		break;

	case 2:
		{
			u32 var_u32 = 0;
			var_u32 = rand();
			bool rslt = AosJEUIA::encode(var_u32, mBuffRaw);
			mValueu32 = var_u32;
			mSizeMax  = 5;
			mflage 	  = 32;
			aos_assert_r(rslt, false);
		}
		break;

	case 3:
		{
			u64 var_u64 = 0;
			var_u64 = rand();
			bool rslt = AosJEUIA::encode(var_u64, mBuffRaw);
			mValueu64 = var_u64;
			mSizeMax  = 9;
			mflage 	  = 64;
			aos_assert_r(rslt, false);
		}
		break;

	default:
		break;
	}

    return true;
}


bool
AosJEUIATorturer::verify()
{
	mBuff->reset();
	char *buff=mBuffRaw->data();

	u8  vv8 	= 0;
	u16 vv16	= 0;
	u32 vv32 	= 0;
	u64 vv64 	= 0;
	int size	= 0;

	int chooseflage = rand() % 2;

	switch(mflage)
	{
	case 8:
		if(0 == chooseflage)
		{
			vv8 = AosJEUIA::decodeU8(buff, mValueu8+1);
			size = AosJEUIA::getsize(mValueu8);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv8 == mValueu8, false);

		}
		else
		{
			AosJEUIA::decode(mBuffRaw, vv64);
			size = AosJEUIA::getsize(mValueu8);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValueu8, false);
		}
		break;

	case 16:
		if(0 == chooseflage)
		{
			vv16 = AosJEUIA::decodeU16(buff, mValueu16+1);
			size = AosJEUIA::getsize(mValueu16);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv16 == mValueu16, false);
		}
		else
		{
			AosJEUIA::decode(mBuffRaw, vv64);
			size = AosJEUIA::getsize(mValueu16);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValueu16, false);
		}
		break;

	case 32:
		if(0 == chooseflage)
		{
			vv32 = AosJEUIA::decodeU32(buff, mValueu32+1);
			size = AosJEUIA::getsize(mValueu32);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv32 == mValueu32, false);
		}
		else
		{
			AosJEUIA::decode(mBuffRaw, vv64);
			size = AosJEUIA::getsize(mValueu32);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValueu32, false);
		}
		break;

	case 64:
		if(0 == chooseflage)
		{
			vv64 = AosJEUIA::decodeU64(buff, mValueu64+1);
			size = AosJEUIA::getsize(mValueu64);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValueu64, false);
		}
		else
		{
			AosJEUIA::decode(mBuffRaw, vv64);
			size = AosJEUIA::getsize(mValueu64);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValueu64, false);
		}
		break;

	}

	return true;
}

