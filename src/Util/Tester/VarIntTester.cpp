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
// 2014/12/28 Rain
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/VarIntTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/VarInt.h"
#include "Util1/Time.h"
#include "Util/ValueRslt.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"


AosVarIntTester::AosVarIntTester() 
:
mData(0),
mIdx(-1),
mBuffRaw(0)
{
	mName = "VarIntTester";
}


bool AosVarIntTester::start()
{
	cout << "...... Start VarInt Tester ......" << endl;
	mRundata = OmnNew AosRundata();

	mBuff = OmnNew AosBuff();
	mBuffRaw = mBuff.getPtr();
	mData = new char[eBuffLen];
	mIdx = 0;

	basicTest();
	return true;
}


bool
AosVarIntTester::basicTest()
{
	cout << "...... Start Basic Tester ......" << endl;

	aos_assert_r(mTestMgr, false);
	u32 count = 0;
	int tries = 100*1000*1000;
	//tries = 2;
	while (tries--)
	{
		if (count % 1000 == 0)
		{
			OmnScreen << "Count: " << count << endl;
		}
		addValuesToBuff();
		aos_assert_r(verifyFromBuff(), false);

		count++;
	}
	return true;
}


bool 
AosVarIntTester::addValuesToBuff()
{
	int nn = OmnRandom::intByRange(
			1, 1, 10,
			2, 10, 50,
			11, 100, 100,
			101, 10000, 10);
    //nn = 1;
	for (int i = 0; i < nn; i++)
	{
		switch (rand() % 4)
	//	switch(3)
		{
		case 0:
			 {
				i8 var_i8 = OmnRandom::nextInt1(-128, 127);
				bool rslt = AosVarInt::encode(var_i8, mBuffRaw);
				aos_assert_r(rslt, false);
				mValues.push_back(var_i8);
			 }
			 break;

		case 1:
			{
				i16 var_i16 = OmnRandom::nextInt1(-32768,32767);
				bool rslt = AosVarInt::encode(var_i16, mBuffRaw);
				aos_assert_r(rslt, false);
				mValues.push_back(var_i16);
			}
			break;

		case 2:
		{
			i32 var_i32 = OmnRandom::nextInt1(-2147483648,2147483647);
			bool rslt = AosVarInt::encode(var_i32, mBuffRaw);
			aos_assert_r(rslt, false);
			mValues.push_back(var_i32);
		}
		break;
			
		case 3:
		{
			i64 var_i64 = OmnRandom::nextInt64(-9223372036854775808,9223372036854775807);
			bool rslt = AosVarInt::encode(var_i64, mBuffRaw);
			aos_assert_r(rslt, false);
			mValues.push_back(var_i64);
		}
		break;
		/*
		case 4:
		{
			u64 docid = rand();
			bool rslt = AosVarInt::encodeDocid(docid, mBuffRaw);
			aos_assert_r(rslt, false);
			mValues.push_back(docid);
		}
		break;
        */
		default:
		break;
		}
	}
	return true;
}

bool 
AosVarIntTester::verifyFromBuff()
{
	AosValueRslt value_rslt;
	char *data = mBuffRaw->data();
	i64 idx = mBuffRaw->getCrtIdx();
	//sleep(1);
    //cout << idx << endl;
	//i64 idx = 0;
	for (int i=0; i < mValues.size(); i++)
	{
		AosVarInt::decode(data, mBuffRaw->dataLen(), idx, value_rslt);
		i64 vv = 0;
		//value_rslt.getU64Value(vv, mRundata.getPtr());
		vv = value_rslt.getInt64Value(mRundata.getPtr());
		aos_assert_r(vv == mValues[i], false);
	}

	//mValues.clear();
	return true;
}


bool
AosVarIntTester::verify()
{
	AosValueRslt value_rslt;

	i64 idx = 0;
	for (int i=0; i < mValues.size(); i++)
	{
		AosVarInt::decode(mData, mIdx, idx, value_rslt);
		i64 vv;
		vv = value_rslt.getInt64Value( mRundata.getPtr());
		aos_assert_r(vv == mValues[i], false);
	}

	mValues.clear();
	mIdx = 0;
	return true;
}
/*
bool
AosVarIntTester::performanceTest()
{
	cout << "...... Start Performance Tester ......" << endl;
	// 1. 
	int tries = 1000*1000;

	for (int i=0; i<tries; i++)
	{
		mIdx = 0;
		i64 idx = 0;
		u64 num = 100*1000*1000;
		AosValueRslt value_rslt;
		OmnString tt1, tt2, tt3, tt4, tt5, tt6, tt7, tt8, tt9, tt10, tt11, tt12, nn;
		nn << num;
		nn.convertToScientific();

		u64 Time = 0;

		//encode i8 ...
		u64 ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encode((i8)57, mData, eBuffLen, mIdx);
		}
		u64 ss2 = OmnGetTimestamp(); 
		Time = ss2-ss1;
		tt1 << Time;
		tt1.convertToScientific();
		u64 rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str;
		rate_str << rate;
		rate_str.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encode(i8)  " <<
			". Rate: " << rate_str << endl;

		//encode i8 ...(u8 vv, AosBuff *buff, int &len).....
		i64 len;
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encode((i8)57, mBuffRaw);
		}
		ss2 = OmnGetTimestamp(); 
		Time = ss2-ss1;
		OmnString ttt8;
		ttt8 << Time;
		ttt8.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str_i8;
		rate_str_i8 << rate;
		rate_str_i8.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encode(i8) (u8 vv, AosBuff *buff, int &len)  " << 
			". Rate: " << rate_str_i8 << endl;
		OmnScreen << endl;

		//decode i8....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::decode(mData, mIdx, idx, value_rslt);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt2 << Time;
		tt2.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str2;
		rate_str2 << rate;
		rate_str2.convertToScientific();

		OmnScreen << "tries " << i+1 << ":" << "decode(i8)  " << 
			 ". Rate: " << rate_str2 << endl;
		mIdx = 0;
		idx = 0;

		//encode ul6 ...
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encode((u16)0x1FFF, mData, eBuffLen, mIdx);
		}
		ss2 = OmnGetTimestamp(); 
		Time = ss2-ss1;
		tt3 << Time;
		tt3.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str3;
		rate_str3 << rate;
		rate_str3.convertToScientific();

		OmnScreen << "tries " << i+1 << ":" << "encode(u16) " <<  
			". Rate: " << rate_str3 << endl;

		//encode ul6 ...buff.......
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encode((u16)0x1FFF, mBuffRaw);
		}
		ss2 = OmnGetTimestamp(); 
		Time = ss2-ss1;
		OmnString ttt16;
		ttt16<< Time;
		ttt16.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str_u16;
		rate_str_u16 << rate;
		rate_str_u16.convertToScientific();

		OmnScreen << "tries " << i+1 << ":" << "encode(u16) encode(u16 vv, AosBuff *buff, int &len) " <<  
			". Rate: " << rate_str_u16 << endl;
		OmnScreen << endl;

		//decode U16....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::decode(mData, mIdx, idx, value_rslt);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt4 << Time;
		tt4.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str4;
		rate_str4 << rate;
		rate_str4.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "decode(u16) " << 
			". Rate: " << rate_str4 << endl;
		mIdx = 0;
		idx = 0;

		//encode u32 ...
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encode((u32)0x0FFFFFFF, mData, eBuffLen, mIdx);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt5 << Time;
		tt5.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str5;
		rate_str5 << rate;
		rate_str5.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encode(u32) " << 
			". Rate: " << rate_str5 << endl;
		
		//encode u32 ...buff...
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encode((u32)0x0FFFFFFF, mBuffRaw);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		OmnString ttt32;
		ttt32 << Time;
		ttt32.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str_u32;
		rate_str_u32 << rate;
		rate_str_u32.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encode(u32)(u32 vv, AosBuff *buff, int &len) " << 
			". Rate: " << rate_str_u32 << endl;
		OmnScreen << endl;
	
		//decode U32....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::decode(mData, mIdx, idx, value_rslt);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt6 << Time;
		tt6.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str6;
		rate_str6 << rate;
		rate_str6.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "decode(u32) " << 
			". Rate: " << rate_str6 << endl;
		mIdx = 0;
		idx = 0;

		//encode u64 ...
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encode((u64)0x03FFFFFFFFFF, mData, eBuffLen, mIdx);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt7 << Time;
		tt7.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str7;
		rate_str7 << rate;
		rate_str7.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encode(u64) " << 
			". Rate: " << rate_str7 << endl;

		//encode u64 ...buff......
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encode((u64)0x03FFFFFFFFFF, mBuffRaw);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		OmnString ttt64;
		ttt64 << Time;
		ttt64.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str_u64;
		rate_str_u64 << rate;
		rate_str_u64.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encode(u64) encode(u64 vv, AosBuff *buff, int &len)" << 
			". Rate: " << rate_str_u64 << endl;
		OmnScreen << endl;

		//decode U64....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::decode(mData, mIdx, idx, value_rslt);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt8 << Time;
		tt8.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str8;
		rate_str8 << rate;
		rate_str8.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "decode(u64) " << 
			". Rate: " << rate_str8 << endl;
		mIdx = 0;
		idx = 0;

		//encode docid.....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encodeDocid((u64)0x03FFFFFFFFFF, mData, eBuffLen, mIdx);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt9 << Time;
		tt9.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str9;
		rate_str9 << rate;
		rate_str9.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encodeDocid " << 
			". Rate: " << rate_str9 << endl;

		//encode docid....buff.....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encodeDocid((u64)0x03FFFFFFFFFF, mBuffRaw);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		//tt9 << Time;
		//tt9.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str_d;
		rate_str_d << rate;
		rate_str_d.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encodeDocid " << 
			". Rate: " << rate_str_d << endl;

		//decode docid....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::decodeDocid(mData, mIdx, idx, value_rslt);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt10 << Time;
		tt10.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str10;
		rate_str10 << rate;
		rate_str10.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "decodeDocid " << 
			". Rate: " << rate_str10 << endl;
		mIdx = 0;
		idx = 0;

		//encode Norm u64 ...
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::encodeNorm((u64)0x03FFFFFFFFFF, mData, eBuffLen, mIdx);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt11 << Time;
		tt11.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str11;
		rate_str11 << rate;
		rate_str11.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encodeNorm(u64) " <<
			". Rate: " << rate_str11 << endl;

		//decode Norm U64....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarInt::decodeNorm(mData, mIdx, idx, value_rslt);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt12 << Time;
		tt12.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str12;
		rate_str12 << rate;
		rate_str12.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "decodeNorm(u64) " << 
			". Rate: " << rate_str12 << endl;

		cout << endl;
	}
	return true;
}
*/
