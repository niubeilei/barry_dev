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
// Modification History:
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Valueset/Testers/ValsetEnumTester.h"

#include "alarm_c/alarm.h"
#include "ActOpr/ValueRslt.h"
#include "Debug/Debug.h"
#include "Valueset/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/Test.h"
#include "Random/RandomUtil.h"
#include "SEUtil/ValueDefs.h"
#include "SEUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "Valueset/Valueset.h"

AosValuesetEnumTester::AosValuesetEnumTester()
{
}


bool AosValuesetEnumTester::start()
{
	cout << "    Start AosValuesetEnum Tester ..." << endl;
	basicTest();
	return true;
}


bool AosValuesetEnumTester::basicTest()
{
	int tries = mTestMgr->getTries();
	OmnScreen << "Tries: " << tries << endl;
	if (tries <= 0) tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		aos_assert_r(testOneEnum(), false);
	}
	return true;
}


bool
AosValuesetEnumTester::testOneEnum()
{
	 OmnString testStr = "$zkyfml{  sdf45  ,  1sss    2d*^*&sf3  ,  6546  }";
	   OmnString p = "$zkyfml{";
	      OmnString vv1,vv2,vv3;
		      OmnStrParser::parseForm1(testStr,p,0,vv1,vv2,vv3);
			       cout <<"-------------45-123--6546----"<<vv1<<"=="<<vv2<<"=="<<vv3<<endl;

	
	
	if(rand()%2==0)
	{
		aos_assert_r(testNormalEnum(), false);
	}
	else
	{
		aos_assert_r(testNormalEnum2(), false);
	}
	return true;
}

	
bool
AosValuesetEnumTester::testNormalEnum()
{
	// Determine data type
	int vv = rand() % 100;
	if (vv <= 30)
	{
		aos_assert_r(testNormalU64(), false);
		return true;
	}

	if (vv <= 60)
	{
		aos_assert_r(testNormalInt64(), false);
		return true;
	}
	
	aos_assert_r(testNormalString(), false);
	return true;
}


bool
AosValuesetEnumTester::testNormalU64()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"003\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_U64 << "\" "
		<<AOSTAG_ENUMTYPE << "=\""<< AOSVALUE_ENUM_TYPE1<<"\">";

	int text_length = rand()%100+1;
	
	vector<u64> eum;
	OmnString enumtext;
	for(int i=0;i<text_length;i++){
		u64 text = rand()%10000;
		eum.push_back(text);
		enumtext << text;
		if((i+1)!=text_length){
			enumtext << ",";
		}
	}

	str << enumtext << "</valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		u64 vv = rand();
		for(int j=0;j<text_length;j++){
			if(vv==eum[j]){
				isInenum = true;
				break;
			}
			
		}
		
		if(!isInenum){
			if(rand()%2==0){
				vv = eum[rand()%text_length];
				isInenum = true;
			}
		}

		AosValueRslt value(vv);

		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << enumtext  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << enumtext  << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetEnumTester::testNormalInt64()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"003\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\" "
		<<AOSTAG_ENUMTYPE << "=\""<< AOSVALUE_ENUM_TYPE1<<"\">";

	int text_length = rand()%100+1;
	
	vector<int64_t> eum;
	OmnString enumtext;
	for(int i=0;i<text_length;i++){
		int64_t text = rand()%10000;
		eum.push_back(text);
		enumtext << text;
		if((i+1)!=text_length){
			enumtext << ",";
		}
	}

	str << enumtext << "</valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		int64_t vv = rand();
		for(int j=0;j<text_length;j++){
			if(vv==eum[j]){
				isInenum = true;
				break;
			}
			
		}
		
		if(!isInenum){
			if(rand()%2==0){
				vv = eum[rand()%text_length];
				isInenum = true;
			}
		}

		AosValueRslt value(vv);

		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << enumtext  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << enumtext  << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetEnumTester::testNormalString()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"003\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\" "
		<<AOSTAG_ENUMTYPE << "=\""<< AOSVALUE_ENUM_TYPE1<<"\">";

	int text_length = rand()%100+1;
	
	vector<OmnString> eum;
	OmnString enumtext;
	for(int i=0;i<text_length;i++){
		
		int len1 = (rand() % 10)+1;
		char data3[12];
		AosRandomLetterStr(len1, data3);
		OmnString text = data3;

		eum.push_back(text);
		enumtext << text;
		if((i+1)!=text_length){
			enumtext << ",";
		}
	}

	str << enumtext << "</valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		
		int len1 = (rand() % 10)+1;
		char data3[12];
		AosRandomLetterStr(len1, data3);
		OmnString vv = data3;
		
		for(int j=0;j<text_length;j++){
			if(vv==eum[j]){
				isInenum = true;
				break;
			}
			
		}
		
		if(!isInenum){
			if(rand()%2==0){
				vv = eum[rand()%text_length];
				isInenum = true;
			}
		}

		AosValueRslt value(vv);

		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << enumtext  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << enumtext  << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}
bool
AosValuesetEnumTester::testNormalEnum2()
{
	// Determine data type
	int vv = rand() % 100;
	if (vv <= 30)
	{
		aos_assert_r(test2NormalU64(), false);
		return true;
	}

	if (vv <= 60)
	{
		aos_assert_r(test2NormalInt64(), false);
		return true;
	}
	
	aos_assert_r(test2NormalString(), false);
	return true;
}


bool
AosValuesetEnumTester::test2NormalU64()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"003\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_U64 << "\" "
		<<AOSTAG_ENUMTYPE << "=\""<< AOSVALUE_ENUM_TYPE2<<"\">";

	int text_length = rand()%100+1;
	
	vector<u64> eum;
	OmnString enumtext;
	for(int i=0;i<text_length;i++){
		u64 text = rand()%10000;
		eum.push_back(text);
		enumtext <<"<value "<<AOSTAG_VALUE_TYPE<<"=\"const\" "<<AOSTAG_DATA_TYPE<<"=\""<<AOSVALUE_U64<<"\">"<<text<<"</value>";
	}

	str << enumtext << "</valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		u64 vv = rand();
		for(int j=0;j<text_length;j++){
			if(vv==eum[j]){
				isInenum = true;
				break;
			}
			
		}
		
		if(!isInenum){
			if(rand()%2==0){
				vv = eum[rand()%text_length];
				isInenum = true;
			}
		}

		AosValueRslt value(vv);

		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" <<  isInenum<<":::"<<enumtext  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" <<  isInenum<<":::"<<enumtext  << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetEnumTester::test2NormalInt64()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"003\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\" "
		<<AOSTAG_ENUMTYPE << "=\""<< AOSVALUE_ENUM_TYPE2<<"\">";

	int text_length = rand()%100+1;
	
	vector<int64_t> eum;
	OmnString enumtext;
	for(int i=0;i<text_length;i++){
		int64_t text = rand()%10000;
		eum.push_back(text);
		enumtext <<"<value "<<AOSTAG_VALUE_TYPE<<"=\"const\" "<<AOSTAG_DATA_TYPE<<"=\""<<AOSVALUE_INT64<<"\">"<<text<<"</value>";
	}

	str << enumtext << "</valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		int64_t vv = rand();
		for(int j=0;j<text_length;j++){
			if(vv==eum[j]){
				isInenum = true;
				break;
			}
			
		}
		
		if(!isInenum){
			if(rand()%2==0){
				vv = eum[rand()%text_length];
				isInenum = true;
			}
		}

		AosValueRslt value(vv);

		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" <<  isInenum<<":::"<<enumtext  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" <<  isInenum<<":::"<<enumtext  << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetEnumTester::test2NormalString()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"003\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\" "
		<<AOSTAG_ENUMTYPE << "=\""<< AOSVALUE_ENUM_TYPE2<<"\">";

	int text_length = rand()%100+1;
	
	vector<OmnString> eum;
	OmnString enumtext;
	for(int i=0;i<text_length;i++){
		
		int len1 = (rand() % 10)+1;
		char data3[12];
		AosRandomLetterStr(len1, data3);
		OmnString text = data3;
		
		eum.push_back(text);
		enumtext <<"<value "<<AOSTAG_VALUE_TYPE<<"=\"const\" "<<AOSTAG_DATA_TYPE<<"=\""<<AOSVALUE_STRING<<"\">"<<text<<"</value>";
	}

	str << enumtext << "</valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		
		int len1 = (rand() % 10)+1;
		char data3[12];
		AosRandomLetterStr(len1, data3);
		OmnString vv = data3;
		
		for(int j=0;j<text_length;j++){
			if(vv==eum[j]){
				isInenum = true;
				break;
			}
			
		}
		
		if(!isInenum){
			if(rand()%2==0){
				vv = eum[rand()%text_length];
				isInenum = true;
			}
		}

		AosValueRslt value(vv);

		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" <<  isInenum<<":::"<<enumtext  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" <<  isInenum<<":::"<<enumtext  << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}

