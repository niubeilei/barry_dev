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
#include "Util/JimoEncodingTester/JESIATorturer.h"
#include "Util/MemoryChecker.h"

AosJESIATorturer::AosJESIATorturer()
{
	cout << "==============Torturer=============" << endl;
}

AosJESIATorturer::~AosJESIATorturer()
{
}

bool
AosJESIATorturer::start(int runtimes)
{
	cout << "...... Start Variant Tester ......" << endl;
	mBuff	= OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mBuffRaw = mBuff.getPtr();

	basicTest(runtimes);

	return true;
}


bool
AosJESIATorturer::basicTest(int runtimes)
{
	cout << "...... Start Basic Testing ......" <<  endl;

	int count = 0;
	int tries = runtimes;

	while (tries--)
	{
		if (count % 1000000 == 0)
		{
			cout << "Count: " << count << endl;
		}
		bool rslt =addValue();
		if (rslt == false)
		{
			OmnAlarm << ".........add value failed........" << enderr;
			return false;
		}
		rslt = verify();
		if(rslt == false)
		{
			OmnAlarm << "......decoding failed..........."<< enderr;
			return false;
		}
		count++;
	}

	return true;

}


bool
AosJESIATorturer::addValue()
{
	mflage = 0;
	mBuffRaw->clear();
	switch(rand() % 4)
	{
	case 0:
		{
			i8 var_i8 = 0;
			int flage = rand() % 2;
			if( flage == 0)
			{
				 var_i8 = rand() % 128;
			}
			else
			{
				 var_i8 = (-1) * (rand() % 111);
			}

			bool rslt = AosJESIA::encode(var_i8, mBuffRaw);
			mValuei8 = var_i8;
			mSizeMax = 1;
			mflage = 8;
			aos_assert_r(rslt, false);
		}
		break;

	case 1:
		{
			i16 var_i16 = 0;
			int flage = rand() % 2;
			if( flage == 0)
			{
				 var_i16 = rand() % 32768;
			}
			else
			{
				 var_i16 = (-1) * (rand() % 32768);
			}

			bool rslt = AosJESIA::encode(var_i16, mBuffRaw);
			mValuei16 = var_i16;
			mflage 	 = 16;
			mSizeMax = 3;
			aos_assert_r(rslt, false);
		}
		break;

	case 2:
		{
			i32 var_i32 = 0;
			int flage = rand() % 2;
			if( flage == 0)
			{
				 var_i32 = rand();
			}
			else
			{
				 var_i32 = (-1) * rand();
			}

			bool rslt = AosJESIA::encode(var_i32, mBuffRaw);
			mValuei32 = var_i32;
			mflage 	 = 32;
			mSizeMax = 5;
			aos_assert_r(rslt, false);
		}
		break;

	case 3:
		{
			i64 var_i64 = 0;
			int flage = rand() % 2;
			if( flage == 0)
			{
				 var_i64 = rand();
			}
			else
			{
				 var_i64 = (-1) * rand();
			}

			bool rslt = AosJESIA::encode(var_i64, mBuffRaw);
			mValuei64 = var_i64;
			mflage 	 = 64;
			mSizeMax = 9;
			aos_assert_r(rslt, false);
		}
		break;

	default:
		break;
	}

    return true;
}


bool
AosJESIATorturer::verify()
{
	mBuff->reset();
	char *buff=mBuffRaw->data();

	i8  vv8	 = 0;
	i16 vv16 = 0;
	i32 vv32 = 0;
	i64 vv64 = 0;
	int size = 0;

	int chooseflage = rand() % 2;

	switch(mflage)
	{
	case 8:
		if(0 == chooseflage)
		{
			vv8	= AosJESIA::decodeI8(buff, mValuei8 + 1);
			size = AosJESIA::getsize(mValuei8);
			aos_assert_r(size<=mSizeMax, false);
			aos_assert_r(vv8 == mValuei8, false);
		}
		else
		{
			AosJESIA::decode(mBuffRaw, vv64);
			size = AosJESIA::getsize(mValuei8);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValuei8, false);
		}
		break;

	case 16:
		if(0 == chooseflage)
		{
			vv16 = AosJESIA::decodeI16(buff, mValuei16 + 1);
			size = AosJESIA::getsize(mValuei16);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv16 == mValuei16, false);
		}
		else
		{
			AosJESIA::decode(mBuffRaw, vv64);
			size = AosJESIA::getsize(mValuei16);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValuei16, false);
		}
		break;

	case 32:
		if(0 == chooseflage)
		{
			vv32 = AosJESIA::decodeI32(buff, mValuei32 + 1);
			size = AosJESIA::getsize(mValuei32);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv32 == mValuei32, false);
		}
		else
		{
			AosJESIA::decode(mBuffRaw, vv64);
			size = AosJESIA::getsize(mValuei32);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValuei32, false);
		}
		break;

	case 64:
		if(0 == chooseflage)
		{
			vv64 = AosJESIA::decodeI64(buff, mValuei64 + 1);
			size = AosJESIA::getsize(mValuei64);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValuei64, false);
		}
		else
		{
			AosJESIA::decode(mBuffRaw, vv64);
			size = AosJESIA::getsize(mValuei64);
			aos_assert_r(size <= mSizeMax, false);
			aos_assert_r(vv64 == mValuei64, false);
		}
		break;

	}

	return true;
}

