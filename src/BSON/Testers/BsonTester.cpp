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
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BSON/Testers/BsonTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "Util1/Time.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"
#include "BSON/BsonValue.h"
#include "BSON/BsonValueStrMap.h"
#include "BSON/BsonValueU64Map.h"
#include "BSON/BsonValueI64Map.h"
#include "BSON/BsonValueMapStrItr.h"
#include "BSON/BsonValueMapU64Itr.h"
#include "BSON/BsonValueMapI64Itr.h"


#include <vector>
using namespace std;
//static bool sgPrintOn = false;

AosBsonTester::AosBsonTester() 
{
	mName = "BsonTester";
}


bool AosBsonTester::start()
{
	cout << "...... Start Bson Tester ......" << endl;

       basicTest();
	return true;
}
/*
bool AosBsonTester::basicTest()
{
   AosRundataPtr rdata = OmnNew AosRundata();
   bool rslt;
   AosBSON mRecord = AosBSON();
   mRecord.setValue("first", OmnString("firstValue"));
   OmnString name("map");
   rslt = mRecord.createMap(name,AosBsonField::eFieldTypeString, AosBsonField::eFieldTypeString);
   aos_assert_r(rslt, false);
   OmnString key1 = OmnString("key1");
   OmnString key2 = OmnString("key2");
   OmnString key3 = OmnString("key3");
   OmnString key4 = OmnString("key4");
   OmnString vv = OmnString("d");
   AosValueRslt value(vv);
   
   mRecord[name][key1] = OmnString("a");
   mRecord[name][key2] = OmnString("b") ;
   mRecord[name][key3] = OmnString("c");
   rslt = mRecord[name].insert(key4,value);
   aos_assert_r(rslt, false);
   mRecord.setValue("second",OmnString("secondValue"));
   aos_assert_r(mRecord[name].getValue(key2) == "b", false); 
   cout << "succesful" << endl; 
   OmnString tmp = mRecord[name][key1];
   cout << tmp << endl;
   OmnString name1("mapU64");
   u64 a1 = 25;  AosValueRslt b1 = AosValueRslt((i64)-100); 
   u64 a2 = 26;  AosValueRslt b2 = AosValueRslt((i64)-200); 
   u64 a3 = 27;  AosValueRslt b3 = AosValueRslt((i64)-300); 
   rslt = mRecord.createMap(name1,AosBsonField::eFieldTypeU64, AosBsonField::eFieldTypeInt64);
   rslt = mRecord[name1].insert(a1,b1);
   aos_assert_r(rslt, false);
   rslt = mRecord[name1].insert(a2,b2);
   aos_assert_r(rslt, false);
   rslt = mRecord[name1].insert(a3,b3);
   aos_assert_r(rslt, false);
   
   mRecord.setValue("five", OmnString("fifthValue"));
   AosBuffPtr mPtr = mRecord.getBuff();
   aos_assert_r(mPtr, NULL);
   
	AosBuffPtr buff2 = OmnNew AosBuff();
	buff2->setBuff(mPtr);

   AosBSON mRecord1(buff2);
   mRecord1[OmnString("first")] = OmnString("123");
   mRecord1[OmnString("second")] = OmnString("123");
   //aos_assert_r(mRecord1["first"] == mRecord["second"], false);
 
   mRecord.setValue("four", OmnString("forthValue"));
   mRecord.setValue("six", OmnString("sixthValue"));
   mRecord.setValue("seven", OmnString("seventhValue"));
   OmnString aa("aa");
   OmnString ss("ss");
   mRecord1["first"] = aa;
   mRecord1["third"] = ss;

    OmnString key5 =  OmnString("key5");
	mRecord1[name][key5] = OmnString("e");
	mRecord1[name][key4] = OmnString("f");
	
    AosBuffPtr mPtr1 = mRecord1.getBuff();
	aos_assert_r(mPtr1, NULL);

	AosBuffPtr buff3 = OmnNew AosBuff();
	buff3->setBuff(mPtr1);

	AosBSON mRecord2(buff3); 
//	aos_assert_r(mRecord2["first"] == mRecord1["first"], false);
//	aos_assert_r(mRecord2["first"] != mRecord["first"], false);
	OmnString v1 = mRecord1["first"];
	OmnString v2 = mRecord2["first"];
	aos_assert_r(v1 == v2, false);

	OmnString v3 = mRecord2[name][OmnString("key4")];

	//aos_assert_r(mRecord2[name][OmnString("key4")] != mRecord[name][OmnString("key4")], false);
    OmnString v4 = mRecord[name][OmnString("key4")] ;
	//mRecord2[name][OmnString("key4")] ;
	return true;
}
*/
/*
bool
AosBsonTester::basicTest()
{
	AosRundataPtr rdata = OmnNew AosRundata();

	cout << "...... Start Basic Tester ......" << endl;

	int tries = 10000;
	//tries = 1;
	u32 count = 0;
	AosValueRslt value;
	bool rslt;
	while (tries--)
	{
		//if (count %10 == 0)
		OmnScreen << "count: " << count << endl;

		mRecord = OmnNew AosBSON();
		rslt = createRecord(rdata.getPtr());
		aos_assert_rr(rslt, rdata, false);

		rslt = verifyRecord(rdata.getPtr());
		aos_assert_rr(rslt, rdata, false);

		count++;
	}
	return true;
}


bool 
AosBsonTester::createRecord(AosRundata *rdata)
{
	// It randomly determine how many fields to create. 
	// For each field, it randomly determine the field type (string, u64, int64, null......)
	int nn = OmnRandom::intByRange(
			1, 1, 10,
			2, 10, 50,
			11, 100, 100,
			101, 1000, 10);
    //nn = 7;
	int nameIdx = 0;
	for (int i=0; i<nn; i++)
	{
		AosValueRslt value;
		//it randomly determine the "name"
		//it randomly determine the 'name' type ('u32' or 'string')
		if (rand()%2)
		//if(1)
		{
			//name type is 'string'
			int ll = OmnRandom::intByRange(
						1, 1, 10,
						1, 10, 50,
						10, 50, 30,
						50, 100, 20);
			OmnString name = myPrintableStr(ll, false);

			bool duplicated = false;
			for (u32 i=0; i<mStrNames.size(); i++)
			{
				map<OmnString, int>::iterator itr = mStrNames.find(name);
				if (itr != mStrNames.end())
				{
					duplicated = true;
					break;
				}
			}
			if (duplicated) continue;
			mStrNames[name] = nameIdx;
			nameIdx++;

			// For each field, it randomly determine the field type (string, u64, int64, null......)
			switch (rand()%13)
			//switch(12)
			{
			//the field type is "string"
			case 0:
				 {
					 mTypes.push_back(AosBsonField::eFieldTypeString);
					 int ll = OmnRandom::intByRange(
											1, 1, 10,
											1, 10, 50,
											11, 100, 20);

					 OmnString vv = OmnRandom::printableStr(ll, ll, false);
				   	 mRecord->setValue(name, vv);
					 value.setValue(vv);
					 //mRecord->setValue(name, value);
					 mValues.push_back(value);
				 }
				 break;
			
			//the field type is "U64"
			case 1:
				 {
					 mTypes.push_back(AosBsonField::eFieldTypeU64);
					 //mTypes.push_back(AosBsonField::eFieldTypeVarUint);
					 u64 vv = OmnRandom::nextU64();
					 mRecord->setValue(name, vv);
					 value.setValue(vv);
					 //mRecord->setValue(rdata, name, value);
					 mValues.push_back(value);
				 }
				 break;

			case 2:
				 //the field type is "Int64"
				 {
				    mTypes.push_back(AosBsonField::eFieldTypeInt64);
					//mTypes.push_back(AosBsonField::eFieldTypeVarInt);
					i64 vv = ((i64)(rand()) << 32) | ((i64)rand());
					//mRecord->appendI64(name, vv);
					mRecord->setValue(name, vv);
					value.setValue(vv);
					 //mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;

			case 3:
				 //the field type is "Double"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeDouble);
					double vv = (double)(rand());
					//mRecord->appendDouble(name, vv);
					mRecord->setValue(name, vv);
					value.setValue(vv);
					 //mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;

			case 4:
				 //the field type is "U32"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeU32);
					u32 vv = ((u32)(rand()) << 16) | ((u32)rand());
					//mRecord->appendU32(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((u64)vv);
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;

			case 5:
				 //the field type is "int32"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeInt32);
					i32 vv = ((i32)(rand()) << 16) | ((i32)rand());
					//mRecord->appendI32(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((i64)vv);
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;
				 

			case 6:
				 //the field type is "U16"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeU16);
					u16 vv = ((u16)(rand()) << 8) | ((u16)rand());
					//mRecord->appendU16(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((u64)vv);
					 //mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;

			case 7:
				 //the field type is "int16"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeInt16);
					i16 vv = ((i16)(rand()) << 8) | ((i16)rand());
				//	mRecord->appendI16(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((i64)vv);
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;
				 
			case 8:
				 //the field type is "U8"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeU8);
					u8 vv = (u8)rand();
				//	mRecord->appendU8(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((u64)vv);
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;

			case 9:
				 //the field type is "int8"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeInt8);
					i8 vv = (i8)rand();
				//	mRecord->appendI8(name, vv);
					mRecord->setValue(name, vv);
					value.setValue(vv);
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;
		
			case 10:
				 //the field type is "float"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeFloat);
					float vv = (float)(rand());
				//	mRecord->appendFloat(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((double)vv);
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;

			case 11:
				 //the field type is "charstr"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeString);
					int ll = OmnRandom::intByRange(
											1, 1, 10,
											1, 10, 50,
											11, 100, 20);

					OmnString vv = OmnRandom::printableStr(ll, ll, false);
					const char *data = vv.data();
				//	mRecord->appendCharStr(name, data, ll);
					mRecord->setValue(name, vv);
					value.setValue(vv);
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;
			
			case 12:
				 //the field type is "bool"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeBool);
					bool vv = (bool)(rand() % 2);
				//	mRecord->appendBool(name, vv);
					mRecord->setValue(name, vv);
					value.setBool(vv);
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;
			
			case 13:
				 //the field type is "Null"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeNull);
					//OmnString vv = "null";
				    //mRecord->appendNull(name);
					mRecord->setValue(name);
					value.setNullValue();
					// mRecord->setValue(rdata, name, value);
					mValues.push_back(value);
				 }
				 break;
			}
//OmnScreen << "name:" << name << ", value:" << value.toString() << endl;
		
		}
		else
		{
			//name type is 'u32'
			u32 name = ((u32)(rand()) << 16) | ((u32)rand());
			bool duplicated = false;
			for (u32 i=0; i<mU32Names.size(); i++)
			{
				map<u32, int>::iterator itr = mU32Names.find(name);
				if (itr != mU32Names.end())
				{
					duplicated = true;
					break;
				}
			}
			if (duplicated) continue;
			mU32Names[name] = nameIdx;
			nameIdx++;

			// For each field, it randomly determine the field type (string, u64, int64, null......)
   		  	switch (rand()%13)
	        //switch(2)
			{
			//the field type is "string"
			case 0:
				 {
					 mTypes.push_back(AosBsonField::eFieldTypeString);
					 int ll = OmnRandom::intByRange(
											1, 1, 10,
											1, 10, 50,
											11, 100, 20);

					 OmnString vv = OmnRandom::printableStr(ll, ll, false);
					 mRecord->setValue(name, vv);
					 value.setValue(vv);
					 mValues.push_back(value);
				 }
				 break;
			
			//the field type is "U64"
			case 1:
				 {
					 mTypes.push_back(AosBsonField::eFieldTypeU64);
					 //mTypes.push_back(AosBsonField::eFieldTypeVarUint);
					 u64 vv = OmnRandom::nextU64();
					 mRecord->setValue(name, vv);
					 value.setValue(vv);
					 mValues.push_back(value);
				 }
				 break;

			case 2:
				 //the field type is "Int64"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeInt64);
					//mTypes.push_back(AosBsonField::eFieldTypeVarInt);
					i64 vv = ((i64)(rand()) << 32) | ((i64)rand());
				//	mRecord->appendI64(name, vv);
					mRecord->setValue(name, vv);
					value.setValue(vv);
					mValues.push_back(value);
				 }
				 break;

			case 3:
				 //the field type is "Double"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeDouble);
					double vv = (double)(rand());
				//	mRecord->appendDouble(name, vv);
					mRecord->setValue(name, vv);
					value.setValue(vv);
					mValues.push_back(value);
				 }
				 break;

			case 4:
				 //the field type is "U32"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeU32);
					u32 vv = ((u32)(rand()) << 16) | ((u32)rand());
				//	mRecord->appendU32(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((u64)vv);
					mValues.push_back(value);
				 }
				 break;

			case 5:
				 //the field type is "int32"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeInt32);
					i32 vv = ((i32)(rand()) << 16) | ((i32)rand());
				//	mRecord->appendI32(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((i64)vv);
					mValues.push_back(value);
				 }
				 break;
				 

			case 6:
				 //the field type is "U16"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeU16);
					u16 vv = ((u16)(rand()) << 16) | ((u16)rand());
					//mRecord->appendU16(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((u64)vv);
					mValues.push_back(value);
				 }
				 break;

			case 7:
				 //the field type is "int16"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeInt16);
					i16 vv = ((i16)(rand()) << 8) | ((i16)rand());
				//	mRecord->appendI16(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((i64)vv);
					mValues.push_back(value);
				 }
				 break;
				 
			case 8:
				 //the field type is "U8"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeU8);
					u8 vv = (u8)rand();
				//	mRecord->appendU8(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((u64)vv);
					mValues.push_back(value);
				 }
				 break;

			case 9:
				 //the field type is "int8"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeInt8);
					i8 vv = (i8)rand();
				//	mRecord->appendI8(name, vv);
					mRecord->setValue(name, vv);
					value.setValue(vv);
					mValues.push_back(value);
				 }
				 break;
				 
			case 10:
				 //the field type is "float"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeFloat);
					float vv = (float)(rand());
				//	mRecord->appendFloat(name, vv);
					mRecord->setValue(name, vv);
					value.setValue((double)vv);
					mValues.push_back(value);
				 }
				 break;

			case 11:
				 //the field type is "charstr"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeString);
					int ll = OmnRandom::intByRange(
											1, 1, 10,
											1, 10, 50,
											11, 100, 20);

					OmnString vv = OmnRandom::printableStr(ll, ll, false);
					const char *data = vv.data();
				//	mRecord->appendCharStr(name, data, ll);
					mRecord->setValue(name, vv);
					value.setValue(vv);
					mValues.push_back(value);
				 }
				 break;
			
			case 12:
				 //the field type is "bool"
				 {	
				    mTypes.push_back(AosBsonField::eFieldTypeBool);
					bool vv = (bool)(rand() % 2);
				//	mRecord->appendBool(name, vv);
					mRecord->setValue(name, vv);
					value.setBool(vv);
					mValues.push_back(value);
                 }
				 break;
			
			case 13:
				 //the field type is "Null"
				 {
					mTypes.push_back(AosBsonField::eFieldTypeNull);
				  //	OmnString vv = "null";
				  //  mRecord->appendNull(name);
					mRecord->setValue(name);
					value.setNullValue();
					mValues.push_back(value);
				 }
				 break;
			}
//OmnScreen << "name:" << name << ", value:" << value.toString() << endl;
		}
	}
	return true;
}


bool
AosBsonTester::verifyRecord(AosRundata *rdata)
{
	// It randomly determine how many times to verify. 
	int nn = OmnRandom::intByRange(
			1, 1, 10,
			2, 10, 20,
			11 , 100, 50,
			101, 5000, 20);

	AosValueRslt value;
	bool rslt;
	u32 idx, offset;

	int count = 0;
	for (int i=0; i<nn; i++)
	{
		//if (rand()%2)
		if(1)
		{
			//string formate name
			if (mStrNames.size() == 0) continue;
			offset = rand() % mStrNames.size();
//OmnScreen << "offset:" << offset << endl;
			map<OmnString, int>::iterator itr = mStrNames.begin();
			while(offset) {offset --;itr++;}
			aos_assert_r(itr != mStrNames.end(), false);
			const OmnString name = itr->first;
			idx = itr->second;
			aos_assert_r(idx<mValues.size(), false);
			rslt = mRecord->getValue(name, value);
			aos_assert_r(rslt, false);
			aos_assert_r(mValues[idx] == value, false);
		}
		else
		{
			if (mU32Names.size() == 0) continue;
			u32 size = (u32)(mU32Names.size());
			offset = rand()%size;
			map<u32, int>::iterator itr = mU32Names.begin();
			while(offset) {offset --; itr++;}
			aos_assert_r(itr != mU32Names.end(), false);
			u32 name = itr->first;
			idx = itr->second;
			aos_assert_r(idx<mValues.size(), false);
			rslt = mRecord->getValue(name, value);
			aos_assert_r(rslt, false);
			aos_assert_r(mValues[idx] == value, false);
		}
	}
	mStrNames.clear();
	mU32Names.clear();
	mValues.clear();
	mTypes.clear();
	return true;
}
*/

OmnString
AosBsonTester::myPrintableStr(int len, bool with_single_quote)
{
	char buf[len+1];
	for (int k=0; k<len; k++)
	{
		char c = (char)((rand()%94)+32);
		if (c == '"' || c == '\\' || c == 46)
		{
			k--;
			continue;
		}

		if (c == '\'' && !with_single_quote)
		{
			k--;
			continue;
		}

		buf[k] = c;
	}

	buf[len] = 0;
	return buf;
}


bool
AosBsonTester::basicTest()
{
	AosRundataPtr rdata = OmnNew AosRundata();

	cout << "...... Start Basic Tester ......" << endl;

	int tries = 10000000;
	u32 count = 0;
	AosValueRslt value;

	bool rslt;
	while (tries--)
	{
		//if (count %10 == 0)
		OmnScreen << "count: " << count << endl;
        rslt = createVerifySwap(rdata.getPtr());
		aos_assert_rr(rslt, rdata, false);

		count++;
	}
	return true;
}


bool 
AosBsonTester::createVerifySwap(AosRundata *rdata)
{
		AosBSON b1;
        AosBSON b2;
        AosValueRslt value;
		i64 vv ;
		u32 vv2;
		OmnString name;
		map<OmnString,int>  StrNames;
		map<OmnString,int>  StrName2;
	    vector<AosValueRslt> mValue2;
        int idx = 0;

		u32 number = rand()% 77;
	    bool rslt;
		//	number = 3;
		while (number--)
		{
			int ll = OmnRandom::intByRange(
						1, 1, 10,
						1, 10, 50,
						10, 50, 30,
						50, 100, 20);
			
			OmnString name = myPrintableStr(ll, false);
			map<OmnString, int>::iterator itr;
			itr = StrName2.find(name);
			if(itr != StrName2.end())
				continue;
			StrName2[name] = idx;
			idx++;
			vv = OmnRandom::nextInt1(-10000, 10000);
			rslt = b1.setValue(name, vv);
			value.setValue((i64)vv);
		    mValue2.push_back(value);
		}
		number = rand()% 88;
	//	number = 2;
        idx = 0;
		while (number--)
		{
			int ll = OmnRandom::intByRange(
						1, 1, 10,
						1, 10, 50,
						10, 50, 30,
						50, 100, 20);
			
			OmnString name = myPrintableStr(ll, false); 
			map<OmnString, int>::iterator itr;
			itr = StrNames.find(name); 
			if(itr != StrNames.end())
				continue;
			StrNames[name] = idx;
			idx++;
			vv2 = OmnRandom::nextU32(1, 10000000);
			//vv = OmnRandom::nextInt1(-10000, 10000); 
			rslt = b2.setValue(name, vv2);
			value.setValue((u64)vv2);
			mValues.push_back(value);
		}
        rslt = b1.swap(b2);
		aos_assert_rr(rslt, rdata, false);
	    //verify the swap
	    map<OmnString,int>::iterator it =	StrNames.begin();
        while(it != StrNames.end())
		{
		  int idx = 0;
		  name = it->first; 
		  idx = it->second;
		  b1.getValue(name, value);
		  aos_assert_rr(value == mValues[idx], rdata, false);
		  it++;
		}
	    it = StrName2.begin();
        while(it != StrName2.end())
		{
		  int idx = 0;
		  name = it->first; 
		  idx = it->second;
		  b2.getValue(name, value);
		  aos_assert_rr(value == mValue2[idx], rdata, false);
		  it++;
		}
		StrNames.clear();
		StrName2.clear();
		mValue2.clear();
		mValues.clear();
		return true;
}


