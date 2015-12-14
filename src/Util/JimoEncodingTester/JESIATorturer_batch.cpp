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
#include "Util/JimoEncodingTester/JESIATorturer_batch.h"
#include "Util/MemoryChecker.h"

AosJESIATorturer_batch::AosJESIATorturer_batch()
{
	cout << "==============Torturer=============" << endl;
}

AosJESIATorturer_batch::~AosJESIATorturer_batch()
{
}

bool
AosJESIATorturer_batch::start(int runtimes)
{
	cout << "...... Start Variant Tester ......" << endl;
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mBuffRaw = mBuff.getPtr();

	basicTest(runtimes);

	return true;
}


bool
AosJESIATorturer_batch::basicTest(int runtimes)
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
			OmnAlarm << "......decoding failed..........."<< enderr;
			return false;
		}
		count++;
	}

	return true;

}


bool
AosJESIATorturer_batch::addValues()
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
AosJESIATorturer_batch::addValue()
{
	switch(rand() % 4)
	{
	case 0:
		{
			i8 var_i8 = 0;
			int flage = rand() % 2;
			if( flage == 0)
			{
				 var_i8 = rand();
			}
			else
			{
				 var_i8 = (-1) * rand();
			}
			bool rslt = AosJESIA::encode(var_i8, mBuffRaw);
			mValues.push_back(var_i8);
			aos_assert_r(rslt, false);
		}
		break;

	case 1:
		{
			i16 var_i16 = 0;
			int flage = rand() % 2;
			if( flage == 0)
			{
				 var_i16 = rand();
			}
			else
			{
				 var_i16 = (-1) * rand();
			}
			bool rslt = AosJESIA::encode(var_i16, mBuffRaw);
			mValues.push_back(var_i16);
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
			mValues.push_back(var_i32);
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
			mValues.push_back(var_i64);
			aos_assert_r(rslt, false);
		}
		break;

	default:
		break;
	}

    return true;
}


bool
AosJESIATorturer_batch::verify()
{
	i64 vv = 0;
	mBuff->reset();
	for (std::vector<i64>::iterator i = mValues.begin(); i != mValues.end(); i++)
	{
		i64 value = *i;
		AosJESIA::decode(mBuffRaw, vv);
		aos_assert_r(vv == value, false);
	}

	return true;
}

