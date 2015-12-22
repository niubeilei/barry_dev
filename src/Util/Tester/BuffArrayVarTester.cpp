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
#include "Util/Tester/BuffArrayVarTester.h"
//#if 0
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Random/RandomUtil.h"
#include "Util/BuffArray.h"
#include "Util/BuffArrayVar.h"
#include "Util/CompareFun.h"
//#include "Util/SortNew.h"

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"  
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "API/AosApiS.h"

using namespace std;

AosBuffArrayVarTester::AosBuffArrayVarTester()
{
	mName = "AosBuffArrayVarTester";
	mTries = 5;
}

struct u64comp
{
	bool operator()(const char *s1, const char *s2)
	{
		return (*(u64 *)s1) < (*(u64 *)s2);
	}
};


bool 
AosBuffArrayVarTester::start()
{
	createBuffArray();
	config();
	return basicTest();
}


bool
AosBuffArrayVarTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	if (!conf) return true;

	AosXmlTagPtr tag = conf->getFirstChild("testers");
	if (!tag) return true;

	return true;
}


bool
AosBuffArrayVarTester::basicTest()
{
	mTries;
	bool rslt;
	int i ;
	while (mTries--)
	{
OmnScreen << "------------------tries------------------" << mTries << endl;
		//rslt = addEntries();
		//checkSort();
		/* for (i = 0 ; i < mTries; i++)
		 {
		 	while(i < 206)
		 	{
		 		rslt = addEntries();
		 		i++;
		 	}
		 	checkSort();
		*/
		//testcase1();

		
		//switch (rand() % 2)
		switch (2)
		{
		case 0:
			 rslt = addEntries();
			 break;

		case 1:
			 //rslt = addEntries();
			 checkInsert();
			 break;

		case 2:
			 rslt = addRecordEntry2();
			 break;
		
		//case 1:
		//	 rslt = checkmodifyEntries();
		//	 break;

		//case 1:
		//	 rslt = addEntries();
		//	 checkSort();
		//	 aos_assert_r(rslt, false);
		//	 break;
		
		//case 1:
		//	 rslt = checkEntries();
		//	 break;

		}
		aos_assert_r(rslt, false);
	}
	return true;
}




bool
AosBuffArrayVarTester::createBuffArray()
{
	mBuffArrayVar = OmnNew AosBuffArrayVar(true);
	OmnString str;
/*
	str << "<zky_buffarray zky_stable = \"false\" >"
//		<< "<CompareFun cmpfun_alphabetic=\"true\" cmpfun_reserve=\"false\" cmpfun_type=\"varstr\" cmpfun_size=\"4\"></CompareFun>"
		<< "<CompareFun cmpfun_reserve=\"false\" cmpfun_size=\"108\" cmpfun_type=\"varstr\"><cmp_fields><field cmp_size=\"100\" cmp_datatype=\"str\" cmp_pos=\"0\"/><field cmp_size=\"8\" cmp_datatype    =\"u64\" cmp_pos=\"100\"/></cmp_fields><aggregations></aggregations></CompareFun>"
		<< 	"</zky_buffarray>";



	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(cmp_tag, false);

	AosRundataPtr rdata = OmnApp::getRundata();
	mBuffArrayVar->create(cmp_tag, rdata);
*/
	return true;
}


bool
AosBuffArrayVarTester::addRecordEntry()
{
	//string value1 = OmnRandom::digitStr(1, 100);
	//string v1 = OmnRandom::digitStr(1, 100);
	
	map<string, u64> fields;
	for(int i=0; i<100; i++)
	{
		string s;
		stringstream ss(s);
		ss << "aaa" << i;
		string field1 = ss.str();
		u64 field2  = OmnRandom::nextU64();
		fields[field1] = field2;
OmnScreen << "appendEntry: field1= " << field1 << ", value2 = " << field2 << endl;
	}

	AosRundata* rdata = OmnApp::getRundata().getPtr();

	OmnString config = "<datarecord type=\"buff\" zky_name=\"t1_schm_zt4g_idxmgr_idx_t1_key_field1__new_idx\"><datafields><datafield type=\"str\" zky_name=\"key\"></datafield><datafield type=\"bin_u64\" zky_name=\"value\" ></datafield></datafields></datarecord>";
	AosXmlParser parser; 
	AosXmlTagPtr rcd_xml = parser.parse(config, "" AosMemoryCheckerArgs);;

	OmnString str;
	str << "<zky_buffarray zky_stable = \"false\" >"
//		<< "<CompareFun cmpfun_alphabetic=\"true\" cmpfun_reserve=\"false\" cmpfun_type=\"varstr\" cmpfun_size=\"4\"></CompareFun>"
		<< "<CompareFun record_fields_num=\"2\" cmpfun_reserve=\"false\" cmpfun_size=\"108\" cmpfun_type=\"custom\"><cmp_fields><field cmp_size=\"-1\" cmp_datatype=\"record\" cmp_pos=\"0\" field_type=\"buffstr\"/><field cmp_size=\"8\" cmp_datatype=\"record\" cmp_pos=\"1\" field_type=\"u64\"/></cmp_fields><aggregations></aggregations></CompareFun>"
		<< 	"</zky_buffarray>";

	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(cmp_tag, false);

	mBuffArrayVar = AosBuffArrayVar::create(cmp_tag, rdata);


	int index = 0;
	map<string, u64>::const_iterator mm = fields.begin();
	for (; mm != fields.end(); ++mm) {                        
		string key = mm->first;
		u64 value = mm->second;
		if(index == 0)
		{
			key="aaa2";
		}
		else
		{
			key="aaa2";
		}
		key="aaa";
		index++;
		cout << key << "=" << value << endl;
		AosDataRecordObjPtr recordobj = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata AosMemoryCheckerArgs);
		aos_assert_r(recordobj, false);
		AosBuffPtr buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs); 
		buff->setDataLen(1000);
		char * data = buff->data();
		int length = buff->dataLen();
		int status = 0;
		recordobj->setData(data+4, length-4, 0, status);
		AosValueRslt valuerslt1;
		AosValueRslt valuerslt2;
		valuerslt1.setStr(key);
		valuerslt2.setU64(value);
		bool outofmem = false;
		recordobj->setFieldValue(0,valuerslt1, outofmem, rdata);
		recordobj->setFieldValue(1,valuerslt2, outofmem, rdata);
		mBuffArrayVar->appendEntry(recordobj.getPtr(), rdata);
	}

	mBuffArrayVar->sort();
	mBuffArrayVar->printBody();
	return true;
}

bool
AosBuffArrayVarTester::addEntries()
{
	//string str = OmnRandom::printableStr(1, 100, false);
	string str = OmnRandom::digitStr(1, 100);
	int len = str.length();
cout << "(((((((((((((((((((((((((((((((((((((" << endl;
OmnScreen << "appendEntry: len = " << len << " \n" << str << endl;
	AosRundata* rdata = OmnApp::getRundata().getPtr();
	mBuffArrayVar->appendEntry(str.data(), len, rdata);
	sgEntries.push_back(str.data());
	return true;
}


bool
AosBuffArrayVarTester::checkSort()
{
	// Print all the contents
	int idx = 0;
	char *str, *next_str;
	int str_len, next_str_len;
	mBuffArrayVar->reset();
	/*if (mBuffArrayVar->numEntries() == 206)
	{
		while (mBuffArrayVar->hasMore())
		{
			mBuffArrayVar->nextEntry(0, str, str_len);
			OmnString sss(str, str_len);
			OmnString ss = "mBuffArrayVar->appendEntry(0, ";
			ss << "\"" << sss << "\"" << ", " << sss.length() << ");";
			cout << ss << endl;
		}
	}
	*/

	mBuffArrayVar->sort();
	mBuffArrayVar->reset();

	bool rslt = mBuffArrayVar->getEntry(0, str, str_len);
	// cout << "======str=====:" << OmnString(str,1) << endl;
	aos_assert_r(str_len > 0, false);
    for(int i=0; i<mBuffArrayVar->getNumEntries()-1; i++)	
	{
		rslt = mBuffArrayVar->getEntry(i+1, next_str, next_str_len);
		// cout << "=======next_str:====" << OmnString(next_str,1)<< endl;
		
 		aos_assert_r(rslt, false);
		aos_assert_r(next_str_len > 0, false);
		int len = str_len > next_str_len ? next_str_len : str_len;
		int rslt = strncmp(str, next_str, len);

		//for alphabetic 
		/*
		if(str_len > next_str_len)
			rslt = 1;
		else if (str_len < next_str_len)
			rslt = -1;
		else rslt;
		*/

		if((rslt == 0) && str_len != next_str_len)
		{
			if(str_len < next_str_len) rslt = -1;
			else rslt = 1;
		}
		
		aos_assert_r(rslt <= 0, false);
		str = next_str; str_len= next_str_len;

	}
	OmnScreen << "finished" << endl;
	return true;
}



bool
AosBuffArrayVarTester::checkEntries()
{
	int pos = rand()%sgEntries.size();
	//u32 value_num = rand()%sgEntries.size();
	string value1 = sgEntries[pos];
	OmnScreen << "value1" << value1 << endl; 

	int len = -1;
	char* value2;
	bool rslt = mBuffArrayVar->getEntry(pos,value2,len);
	//string v2(value2, len);
	
	aos_assert_r(strncmp(value2, value1.data(), len) == 0, false);

	return true;
}

	
bool
AosBuffArrayVarTester::checkmodifyEntries()
{
	
	int pos = rand()%sgEntries.size();
	cout << "====== pos: ====" << pos;
	//set the new value
	string newdata = "zky_levixxxxx";
	
	int len = 0;
	char* data;
	bool rslt = mBuffArrayVar->getEntry(pos,data,len);
	cout <<" ==============data==========" << data << endl;
	
	//modify the data
	AosRundata* rdata = OmnApp::getRundata().getPtr();
	mBuffArrayVar->modifyEntry(pos, newdata.data(), newdata.length(), rdata);
	
	//get the modify data
	char* modifydata;
	rslt = mBuffArrayVar->getEntry(pos,modifydata,len);
	cout << "====== modifydata: ======" << modifydata; 
	aos_assert_r(strncmp(newdata.data(), modifydata, len) == 0, false);

	return true;
}

bool 
AosBuffArrayVarTester::checkInsert()
{	
	//sort
	mBuffArrayVar->reset();
	mBuffArrayVar->sort();
	
	//insert the data
	OmnString str_data = OmnRandom::digitStr(1, 100);
	//OmnString str_data = OmnRandom::printableStr(1, 100, false);
	AosRundata* rdata = OmnApp::getRundata().getPtr();
	mBuffArrayVar->insertEntry(str_data.data(), str_data.length(), rdata);
	
	//check the insert
	mBuffArrayVar->reset();
	char* str;
	char* next_str;
	int str_len = 0;
	int next_str_len = 0;

	bool rslt = mBuffArrayVar->getEntry(0, str, str_len);
	// cout << "======str=====:" << OmnString(str,1) << endl;
	aos_assert_r(str_len > 0, false);
	for( int i=0; i<mBuffArrayVar->getNumEntries()-1; i++)
	{
		rslt = mBuffArrayVar->getEntry(i+1, next_str, next_str_len);
		// cout << "=======next_str:====" << OmnString(next_str,1)<< endl;
		
 		aos_assert_r(rslt, false);
		aos_assert_r(next_str_len > 0, false);
		int len = str_len > next_str_len ? next_str_len : str_len;
		int rslt = strncmp(str, next_str, len);

		//for alphabetic 
		
		if(str_len > next_str_len)
			rslt = 1;
		else if (str_len < next_str_len)
			rslt = -1;
		else rslt;
		
		/*
		if((rslt == 0) && str_len != next_str_len)
		{
			if(str_len < next_str_len) rslt = -1;
			else rslt = 1;
		}
		*/
		
		aos_assert_r(rslt <= 0, false);
		str = next_str; str_len= next_str_len;

	}
	OmnScreen << "finished" << endl;
	return true;
}


bool
AosBuffArrayVarTester::testcase1()
{
	mBuffArrayVar->reset();
	
	AosRundata* rdata = OmnApp::getRundata().getPtr();
	mBuffArrayVar->appendEntry("0276854702161301704304830800783788284443757685856364635624673733530084140016570006461111550611", 94, rdata);
	mBuffArrayVar->appendEntry("4", 1, rdata);
	mBuffArrayVar->appendEntry("237387575721861615533518327084415025881261368217778137060548080402768886701620207108603668547365", 96, rdata);
	mBuffArrayVar->appendEntry("42311708641750", 14, rdata);
	checkSort();

	return true;
}

bool
AosBuffArrayVarTester::addRecordEntry2()
{
	AosRundata* rdata = OmnApp::getRundata().getPtr();
	//str u64 u64
	//OmnString config = "<datarecord type=\"buff\" zky_name=\"t1_schm_zt4g_idxmgr_idx_t1_key_field1__new_idx\"><datafields><datafield type=\"str\" zky_name=\"value1\"></datafield><datafield type=\"bin_u64\" zky_name=\"value2\"></datafield><datafield type=\"bin_u64\" zky_name=\"key\" ></datafield></datafields></datarecord>";
	
	//u64 u64 str
	OmnString config = "<datarecord type=\"buff\" zky_name=\"t1_schm_zt4g_idxmgr_idx_t1_key_field1__new_idx\"><datafields><datafield type=\"bin_u64\" zky_name=\"key\"></datafield><datafield type=\"bin_u64\" zky_name=\"value2\"></datafield><datafield type=\"str\" zky_name=\"value1\" ></datafield></datafields></datarecord>";
	
	//u64 str str
	//OmnString config = "<datarecord type=\"buff\" zky_name=\"t1_schm_zt4g_idxmgr_idx_t1_key_field1__new_idx\"><datafields><datafield type=\"bin_u64\" zky_name=\"key\"></datafield><datafield type=\"str\" zky_name=\"value2\"></datafield><datafield type=\"str\" zky_name=\"value1\" ></datafield></datafields></datarecord>";
	AosXmlParser parser; 
	AosXmlTagPtr rcd_xml = parser.parse(config, "" AosMemoryCheckerArgs);;

	OmnString str;
	//str u64 u64
	//str << "<zky_buffarray zky_stable = \"false\" >"
	//	<< "<CompareFun record_fields_num=\"3\" cmpfun_reserve=\"false\" cmpfun_size=\"108\" cmpfun_type=\"custom\" record_type=\"buff\"><cmp_fields><field cmp_size=\"-1\" cmp_datatype=\"record\" cmp_pos=\"2\" field_type=\"u64\"/></cmp_fields><aggregations><aggregation agr_pos=\"0\" agr_type=\"record\" field_type=\"agrstr\" agr_fun=\"sum\"/><aggregation agr_pos=\"1\" agr_type=\"record\" field_type=\"u64\" agr_fun=\"sum\"/></aggregations></CompareFun>"
	//	<< 	"</zky_buffarray>";
	
	//u64 u64 str
	str << "<zky_buffarray zky_stable = \"false\" >"
		<< "<CompareFun record_fields_num=\"3\" cmpfun_reserve=\"false\" cmpfun_size=\"108\" cmpfun_type=\"custom\" record_type=\"buff\"><cmp_fields><field cmp_size=\"-1\" cmp_datatype=\"record\" cmp_pos=\"0\" field_type=\"u64\"/></cmp_fields><aggregations><aggregation agr_pos=\"1\" agr_type=\"record\" field_type=\"u64\" agr_fun=\"sum\"/><aggregation agr_pos=\"2\" agr_type=\"record\" field_type=\"agrstr\" agr_fun=\"sum\"/></aggregations></CompareFun>"
		<< 	"</zky_buffarray>";
	
	//u64 str str
	//str << "<zky_buffarray zky_stable = \"false\" >"
	//	<< "<CompareFun record_fields_num=\"3\" cmpfun_reserve=\"false\" cmpfun_size=\"108\" cmpfun_type=\"custom\" record_type=\"buff\"><cmp_fields><field cmp_size=\"-1\" cmp_datatype=\"record\" cmp_pos=\"0\" field_type=\"u64\"/></cmp_fields><aggregations><aggregation agr_pos=\"1\" agr_type=\"record\" field_type=\"agrstr\" agr_fun=\"sum\"/><aggregation agr_pos=\"2\" agr_type=\"record\" field_type=\"agrstr\" agr_fun=\"sum\"/></aggregations></CompareFun>"
	//	<< 	"</zky_buffarray>";

	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(cmp_tag, false);

	mBuffArrayVar = AosBuffArrayVar::create(cmp_tag, rdata);

	map<u64, vector<OmnString> >  keyValue1Map;
	map<u64, vector<u64> >  keyValue2Map;
	//map<u64, vector<OmnString> >  keyValue2Map;
	
	map<u64, vector<OmnString> >::iterator itr1;
	map<u64, vector<u64> >::iterator itr2;
	//map<u64, vector<OmnString> >::iterator itr2;

	vector<OmnString> vc_value1;
	vector<u64> vc_value2;
	//vector<OmnString> vc_value2;
	u64 key;
	OmnString value1;
	u64 value2;
	//OmnString value2;
	int num = 500000;
	vector<u64> keys;
	keys.push_back(OmnRandom::nextU64());
OmnScreen << OmnGetTime(AosLocale::eChina) << ", appendEntry started " << endl;
	for(int i=0; i<num; i++)
	{
		switch (rand() %2)
		{
		case 0:
			key = OmnRandom::nextU64();
			keys.push_back(key);
			break;
		case 1:
			{
				int i = rand() % keys.size();
				key = keys[i];
			}
			break;
		}
		value1 = OmnRandom::digitStr(0, 20);
		//value2 = OmnRandom::digitStr(0, 20);
		value2 = OmnRandom::nextU64();
		/*
		key = i;
		value1 = "";
		value1 << "aaa" << i;

		value2 = "";
		value2 << i;
		*/

//OmnScreen << "appendEntry: key = " << key << ", value1 = " << value1 << ", value2 = " << value2 << endl;

		itr1 = keyValue1Map.find(key);
		itr2 = keyValue2Map.find(key);
		if (itr1 == keyValue1Map.end())
		{
			aos_assert_r(itr2 == keyValue2Map.end(), false);
			vc_value1.clear();
			vc_value2.clear();
			vc_value1.push_back(value1);
			vc_value2.push_back(value2);
			keyValue1Map[key] = vc_value1;
			keyValue2Map[key] = vc_value2;
		}
		else
		{
			aos_assert_r(itr2 != keyValue2Map.end(), false);
			itr1->second.push_back(value1);
			itr2->second.push_back(value2);
		}

		AosDataRecordObjPtr recordobj = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata AosMemoryCheckerArgs);
		aos_assert_r(recordobj, false);
		AosBuffPtr buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs); 
		buff->setDataLen(1000);
		char * data = buff->data();
		int length = buff->dataLen();
		int status = 0;
		recordobj->setData(data+4, length-4, 0, status);
		AosValueRslt valuerslt1;
		AosValueRslt valuerslt2;
		AosValueRslt valuerslt3;
		if (value1 == "")
			valuerslt1.setNull();
		else
		{
			valuerslt1.setStr(value1);
		}
		valuerslt2.setU64(value2);
		/*
		if (value2 == "")
			valuerslt2.setNull();
		else
		{
			valuerslt2.setStr(value2);
		}
		*/

		valuerslt3.setU64(key);
		bool outofmem = false;
		//str u64 u64
		//recordobj->setFieldValue(0,valuerslt1, outofmem, rdata);
		//recordobj->setFieldValue(1,valuerslt2, outofmem, rdata);
		//recordobj->setFieldValue(2,valuerslt3, outofmem, rdata);

		//u64 u64 str
		recordobj->setFieldValue(0,valuerslt3, outofmem, rdata);
		recordobj->setFieldValue(1,valuerslt2, outofmem, rdata);
		recordobj->setFieldValue(2,valuerslt1, outofmem, rdata);
		mBuffArrayVar->appendEntry(recordobj.getPtr(), rdata);
	}
OmnScreen << OmnGetTime(AosLocale::eChina) << ", appendEntry finished" << endl;

	mBuffArrayVar->sort();

	int t1 = OmnGetSecond();
OmnScreen << OmnGetTime(AosLocale::eChina) << ", mergedata started" << endl;
	mBuffArrayVar->mergeData();
	int t2 = OmnGetSecond();
OmnScreen << OmnGetTime(AosLocale::eChina) << ", mergedata finished" << endl;
OmnScreen << "mergeData:" << num << ", take time:" << t2-t1 << endl;

	//check
	AosBuffPtr buff = mBuffArrayVar->getHeadBuff();
	const char * entry = buff->data();
	const char * record = NULL;
	i64 field_offset;
	const char * data = NULL;
	int len = 0;

	i64 numRcds = mBuffArrayVar->getNumEntries();
	i64 cmpSize = mBuffArrayVar->getCompareFunc()->size;
OmnScreen << "check started" << endl;
OmnScreen << "append entry num:" << num << ", after merge numRcds:" << numRcds << endl;
	for (int i=0; i<numRcds; i++)
	{
		record = (const char *)(*(i64*)(entry+sizeof(int))) + (*(int*)entry);

		//str u64 u64
		/*
		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 0;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		len = *(int*)(record + *(u16*)(entry+field_offset) + sizeof(u32) - sizeof(int));
		OmnString value1(data, len);

		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 1;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		u64 value2 = *(u64*)data;

		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 2;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		u64 key = *(u64*)data;
		*/
		//u64 u64 str 
		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 0;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		u64 key = *(u64*)data;

		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 1;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		u64 value2 = *(u64*)data;

		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 2;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		len = *(int*)(record + *(u16*)(entry+field_offset) + sizeof(u32) - sizeof(int));
		OmnString value1(data, len);

		//u64 str str 
		/*
		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 0;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		u64 key = *(u64*)data;

		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 1;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		len = *(int*)(record + *(u16*)(entry+field_offset) + sizeof(u32) - sizeof(int));
		OmnString value2(data, len);

		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 2;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		len = *(int*)(record + *(u16*)(entry+field_offset) + sizeof(u32) - sizeof(int));
		OmnString value1(data, len);
		*/

		entry = &entry[cmpSize];

//OmnScreen << "key:" << key << ", value1:" << value1 << ", value2:" << value2 << endl;
		itr1 = keyValue1Map.find(key);
		itr2 = keyValue2Map.find(key);
		aos_assert_r(itr1 != keyValue1Map.end() && itr2 != keyValue2Map.end(), false);
		if (value1 == "")
		{
			aos_assert_r(itr1->second.size() == 1 && itr2->second.size() == 1, false);
			aos_assert_r(itr1->second[0] == value1, false);
			aos_assert_r(itr2->second[0] == value2, false);
		}
		else
		{
			OmnString split;
			split <<  '\001';
			vector<OmnString> values;
			AosSplitStr(value1, split.data(), values, 1000);
			aos_assert_r(values.size() == itr1->second.size(), false);

			//AosSplitStr(value2, split.data(), values, 1000);
			//aos_assert_r(values.size() == itr2->second.size(), false);
			u64 sum_v2 = 0;
			for (size_t i = 0; i<itr2->second.size(); i++)
			{
				sum_v2 += itr2->second[i];
			}
			aos_assert_r(value2 == sum_v2, false);
		}
	}
OmnScreen << "check finished" << endl;
	return true;
}

