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
// 2014/08/10 Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/Tester/VarUnintTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/VarUnInt.h"
#include "Util1/Time.h"
#include "Util/ValueRslt.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"


AosVarUnintTester::AosVarUnintTester() 
:
mData(0),
mIdx(-1),
mBuffRaw(0)
{
	mName = "VarUnintTester";
}


bool AosVarUnintTester::start()
{
	cout << "...... Start VarUnint Tester ......" << endl;
	mRundata = OmnNew AosRundata();
	mRundata->setSiteid(100);

	mBuff = OmnNew AosBuff();
	mBuffRaw = mBuff.getPtr();
	mData = new char[eBuffLen];
	mIdx = 0;

	basicTest();
	//basicTest2();
	//performanceTest();
	return true;
}


bool
AosVarUnintTester::basicTest()
{
	cout << "...... Start Basic Tester ......" << endl;

	int count = 0;
	int tries = 100*1000*1000;

	while (tries--)
	{
		if (count % 100000 == 0)
		{
			OmnScreen << "Count: " << count << endl;
		}
		addValues();
		verify();
		count++;
	}
	return true;
}

bool
AosVarUnintTester::basicTest2()
{
	cout << "...... Start Basic Tester2 ......" << endl;

	int count = 0;
	int tries = 100*1000*1000;

	while (tries--)
	{
		mBuff = OmnNew AosBuff();
		mBuffRaw = mBuff.getPtr();

		if (count % 100000 == 0)
		{
			OmnScreen << "Count: " << count << endl;
		}
		addValuesToBuff();
		verifyFromBuff();
		count++;
	}
	return true;
}

bool 
AosVarUnintTester::addValuesToBuff()
{
	int nn = OmnRandom::intByRange(
			1, 1, 10,
			2, 10, 50,
			11, 100, 100,
			101, 10000, 10);

	for (int i = 0; i < nn; i++)
	{
		switch (rand() % 4)
		{
		case 0:
			{
				u8 var_u8 = rand();
				bool rslt = AosVarUnInt::encode(var_u8, mBuffRaw);
				aos_assert_r(rslt, false);
				mValues.push_back(var_u8);
				mDocidFlags.push_back(1);
			}
			break;

		case 1:
			{
				u16 var_u16 = rand();
				bool rslt = AosVarUnInt::encode(var_u16, mBuffRaw);
				aos_assert_r(rslt, false);
				mValues.push_back(var_u16);
				mDocidFlags.push_back(1);
			}
			break;

		case 2:
		{
			u32 var_u32 = rand();
			bool rslt = AosVarUnInt::encode(var_u32, mBuffRaw);
			aos_assert_r(rslt, false);
			mValues.push_back(var_u32);
			mDocidFlags.push_back(1);
		}
		break;
			
		case 3:
		{
			u64 var_u64 = rand();
			bool rslt = AosVarUnInt::encode(var_u64, mBuffRaw);
			aos_assert_r(rslt, false);
			mValues.push_back(var_u64);
			mDocidFlags.push_back(1);
		}
		break;
		
		case 4:
		{
			u64 docid = rand();
			bool rslt = AosVarUnInt::encodeDocid(docid, mBuffRaw);
			aos_assert_r(rslt, false);
			mValues.push_back(docid);
			mDocidFlags.push_back(2);
		}
		break;

		default:
		break;
		}
	}
	return true;
}

bool 
AosVarUnintTester::verifyFromBuff()
{
	AosValueRslt value_rslt;
	char *data = mBuffRaw->data();
	i64 idx = 0;

	for (int i=0; i < mValues.size(); i++)
	{
		if (mDocidFlags[i] == 1)
		{
			AosVarUnInt::decode(data, mBuffRaw->dataLen(), idx, value_rslt);
			u64 vv;
			value_rslt.getU64Value(vv, mRundata.getPtr());
			aos_assert_r(vv == mValues[i], false);
		}
		else if (mDocidFlags[i] == 2)
		{
			AosVarUnInt::decodeDocid(data, mBuffRaw->dataLen(), idx, value_rslt);
			u64 vv;
			value_rslt.getU64Value(vv, mRundata.getPtr());
			aos_assert_r(vv == mValues[i], false);
		}
		else
		{
			OmnAlarm << "Internal error" << enderr;
			return false;
		}
	}

	mValues.clear();
	mDocidFlags.clear();
	return true;
}

bool
AosVarUnintTester::addValues()
{
	int nn = OmnRandom::intByRange(
			1, 1, 10,
			2, 10, 50, 
			11, 100, 100, 
			101, 10000, 10);

	for (int i=0; i<nn; i++)
	{
		switch (rand() % 5)
		{
		case 0:
		{
			u8 var_u8 = rand();
			bool rslt = AosVarUnInt::encode(var_u8, mData, eBuffLen, mIdx);
			aos_assert_r(rslt, false);
			mValues.push_back(var_u8);
			mDocidFlags.push_back(1);
		}
		break;

		case 1:
		{
			u16 var_u16 = rand();
			bool rslt = AosVarUnInt::encode(var_u16, mData, eBuffLen, mIdx);
			aos_assert_r(rslt, false);
			mValues.push_back(var_u16);
			mDocidFlags.push_back(1);
		}
		break;

		case 2:
		{
			u32 var_u32 = rand();
			bool rslt = AosVarUnInt::encode(var_u32, mData, eBuffLen, mIdx);
			aos_assert_r(rslt, false);
			mValues.push_back(var_u32);
			mDocidFlags.push_back(1);
		}
		break;
			
		case 3:
		{
			u64 var_u64 = rand();
			bool rslt = AosVarUnInt::encode(var_u64, mData, eBuffLen, mIdx);
			aos_assert_r(rslt, false);
			mValues.push_back(var_u64);
			mDocidFlags.push_back(1);
		}
		break;
		
		case 4:
		{
			u64 docid = rand();
			bool rslt = AosVarUnInt::encodeDocid(docid, mData, eBuffLen, mIdx);
			aos_assert_r(rslt, false);
			mValues.push_back(docid);
			mDocidFlags.push_back(2);
		}
		break;
	
		default:
		break;
		}
	}
	return true;
}

bool
AosVarUnintTester::verify()
{
	AosValueRslt value_rslt;

	i64 idx = 0;
	for (int i=0; i < mValues.size(); i++)
	{
		if (mDocidFlags[i] == 1)
		{
			AosVarUnInt::decode(mData, mIdx, idx, value_rslt);
			u64 vv;
			value_rslt.getU64Value(vv, mRundata.getPtr());
			aos_assert_r(vv == mValues[i], false);
		}
		else if (mDocidFlags[i] == 2)
		{
			AosVarUnInt::decodeDocid(mData, mIdx, idx, value_rslt);
			u64 vv;
			value_rslt.getU64Value(vv, mRundata.getPtr());
			aos_assert_r(vv == mValues[i], false);
		}
		else
		{
			OmnAlarm << "Internal error" << enderr;
			return false;
		}
	}

	mValues.clear();
	mDocidFlags.clear();
	mIdx = 0;
	return true;
}

bool
AosVarUnintTester::performanceTest()
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

		//encode u8 ...
		u64 ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarUnInt::encode((u8)57, mData, eBuffLen, mIdx);
		}
		u64 ss2 = OmnGetTimestamp(); 
		Time = ss2-ss1;
		tt1 << Time;
		tt1.convertToScientific();
		u64 rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str;
		rate_str << rate;
		rate_str.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encode(u8)  " <<
			". Rate: " << rate_str << endl;

		//encode u8 ...(u8 vv, AosBuff *buff, int &len).....
		i64 len;
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarUnInt::encode((u8)57, mBuffRaw);
		}
		ss2 = OmnGetTimestamp(); 
		Time = ss2-ss1;
		OmnString ttt8;
		ttt8 << Time;
		ttt8.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str_u8;
		rate_str_u8 << rate;
		rate_str_u8.convertToScientific();
		OmnScreen << "tries " << i+1 << ":" << "encode(u8) (u8 vv, AosBuff *buff, int &len)  " << 
			". Rate: " << rate_str_u8 << endl;
		OmnScreen << endl;

		//decode U8....
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarUnInt::decode(mData, mIdx, idx, value_rslt);
		}
		ss2 = OmnGetTimestamp();
		Time = ss2-ss1;
		tt2 << Time;
		tt2.convertToScientific();
		rate = num * 1000 * 1000 / (ss2-ss1);
		OmnString rate_str2;
		rate_str2 << rate;
		rate_str2.convertToScientific();

		OmnScreen << "tries " << i+1 << ":" << "decode(u8)  " << 
			 ". Rate: " << rate_str2 << endl;
		mIdx = 0;
		idx = 0;

		//encode ul6 ...
		ss1 = OmnGetTimestamp();
		for (int k=0; k<num; k++)
		{
			AosVarUnInt::encode((u16)0x1FFF, mData, eBuffLen, mIdx);
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
			AosVarUnInt::encode((u16)0x1FFF, mBuffRaw);
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
			AosVarUnInt::decode(mData, mIdx, idx, value_rslt);
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
			AosVarUnInt::encode((u32)0x0FFFFFFF, mData, eBuffLen, mIdx);
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
			AosVarUnInt::encode((u32)0x0FFFFFFF, mBuffRaw);
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
			AosVarUnInt::decode(mData, mIdx, idx, value_rslt);
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
			AosVarUnInt::encode((u64)0x03FFFFFFFFFF, mData, eBuffLen, mIdx);
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
			AosVarUnInt::encode((u64)0x03FFFFFFFFFF, mBuffRaw);
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
			AosVarUnInt::decode(mData, mIdx, idx, value_rslt);
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
			AosVarUnInt::encodeDocid((u64)0x03FFFFFFFFFF, mData, eBuffLen, mIdx);
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
			AosVarUnInt::encodeDocid((u64)0x03FFFFFFFFFF, mBuffRaw);
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
			AosVarUnInt::decodeDocid(mData, mIdx, idx, value_rslt);
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
			AosVarUnInt::encodeNorm((u64)0x03FFFFFFFFFF, mData, eBuffLen, mIdx);
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
			AosVarUnInt::decodeNorm(mData, mIdx, idx, value_rslt);
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
#endif
