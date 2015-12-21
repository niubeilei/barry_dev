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
	mTries = 1;
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
	OmnString config = "<datarecord type=\"buff\" zky_name=\"t1_schm_zt4g_idxmgr_idx_t1_key_field1__new_idx\"><datafields><datafield type=\"str\" zky_name=\"value\"></datafield><datafield type=\"bin_u64\" zky_name=\"key\" ></datafield></datafields></datarecord>";
	AosXmlParser parser; 
	AosXmlTagPtr rcd_xml = parser.parse(config, "" AosMemoryCheckerArgs);;

	OmnString str;
	str << "<zky_buffarray zky_stable = \"false\" >"
		<< "<CompareFun record_fields_num=\"2\" cmpfun_reserve=\"false\" cmpfun_size=\"108\" cmpfun_type=\"custom\" record_type=\"buff\"><cmp_fields><field cmp_size=\"-1\" cmp_datatype=\"record\" cmp_pos=\"1\" field_type=\"u64\"/></cmp_fields><aggregations><aggregation agr_pos=\"0\" agr_type=\"record\" field_type=\"agrstr\" agr_fun=\"sum\"/></aggregations></CompareFun>"
		<< 	"</zky_buffarray>";

	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(cmp_tag, false);

	mBuffArrayVar = AosBuffArrayVar::create(cmp_tag, rdata);

	map<u64, vector<OmnString> >  maps;
	map<u64, vector<OmnString> >::iterator itr;
	vector<OmnString> v;
	u64 key;
	OmnString value;
	int num = 5000;
	vector<u64> keys;
	keys.push_back(OmnRandom::nextU64());
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
		value = OmnRandom::digitStr(0, 20);
		/*
		key = i%10;
		value = "";
		value << "aaa" << i;
		*/

OmnScreen << "appendEntry: key = " << key << ", value = " << value << endl;

		itr = maps.find(key);
		if (itr == maps.end())
		{
			v.clear();
			v.push_back(value);
			maps[key] = v;
		}
		else
		{
			itr->second.push_back(value);
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
		if (value == "")
			valuerslt1.setNull();
		else
		{
			valuerslt1.setStr(value);
		}
		valuerslt2.setU64(key);
		bool outofmem = false;
		recordobj->setFieldValue(0,valuerslt1, outofmem, rdata);
		recordobj->setFieldValue(1,valuerslt2, outofmem, rdata);
		mBuffArrayVar->appendEntry(recordobj.getPtr(), rdata);
	}

	mBuffArrayVar->sort();
	mBuffArrayVar->mergeData();

	//check
	AosBuffPtr buff = mBuffArrayVar->getHeadBuff();
	const char * entry = buff->data();
	const char * record = NULL;
	i64 field_offset;
	const char * data = NULL;
	int len = 0;
	//OmnString result;

	i64 numRcds = mBuffArrayVar->getNumEntries();
	i64 cmpSize = mBuffArrayVar->getCompareFunc()->size;
OmnScreen << "check..............." << endl;
OmnScreen << "append entry num:" << num << ", after merge numRcds:" << numRcds << endl;
	for (int i=0; i<numRcds; i++)
	{
		record = (const char *)(*(i64*)(entry+sizeof(int))) + (*(int*)entry);

		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 0;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		len = *(int*)(record + *(u16*)(entry+field_offset) + sizeof(u32) - sizeof(int));
		OmnString value(data, len);

		field_offset = sizeof(int) + sizeof(i64) + sizeof(u16) * 1;
		data = (record + *(u16*)(entry+field_offset) + sizeof(u32));
		key = *(u64*)data;
		entry = &entry[cmpSize];

OmnScreen << "key:" << key << ", value:" << value << endl;
		itr = maps.find(key);
		aos_assert_r(itr != maps.end(), false);
		if (value == "")
		{
			aos_assert_r(itr->second.size() == 1, false);
			aos_assert_r(itr->second[0] == "", false);
		}
		else
		{

			OmnString split;
			split <<  '\001';
			vector<OmnString> values;
			AosSplitStr(value, split.data(), values, 1000);
			aos_assert_r(values.size() == itr->second.size(), false);
		}

		/*
		result = "";
		for (size_t i = 0; i < itr->second.size(); i++)
		{
			if(i>0)
				result << char(0x01);
			result << itr->second[i];
		}
		*/
//OmnScreen << "value:" << value << ", result:" << result << endl;
		//aos_assert_r(value == result, false);
	}

	return true;
}

//#endif
