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
#include "Valueset/Testers/ValsetPatternTester.h"

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
#include "Random/RandomUtil.h"
#include <boost/regex.hpp>

AosValuesetPatternTester::AosValuesetPatternTester()
{
}


bool AosValuesetPatternTester::start()
{
	cout << "    Start AosValuesetPattern Tester ..." << endl;
	basicTest();
	return true;
}


bool AosValuesetPatternTester::basicTest()
{
	int tries = mTestMgr->getTries();
	OmnScreen << "Tries: " << tries << endl;
	if (tries <= 0) tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		aos_assert_r(testOnePattern(), false);
	}
	return true;
}


bool
AosValuesetPatternTester::testOnePattern()
{
	aos_assert_r(testNormalPattern(), false);
	return true;
}

	
bool
AosValuesetPatternTester::testNormalPattern()
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
AosValuesetPatternTester::testNormalU64()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"004\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_U64 << "\">";

	
	OmnString pattern ;
	pattern<<".*?" <<rand()%100 <<".*";


	str << "<![CDATA["<< pattern<<"]]></valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		
		u64 vv = rand()%1000000;


		AosValueRslt value(vv);
		try
		{
			std::string temp = pattern.data();
			boost::regex reg(temp);
			std::string s = value.getValueStr().data();
			if(boost::regex_match(s, reg))
			{
		    	isInenum =true;
			}
			//cout << isInenum<<":"<<s<<":"<<reg<<endl;
		}
		catch(const boost::bad_expression& e)
		{
			 rdata->setError()<<" pattern error";
			 return true;
		}
		//cout <<"?"<< isInenum<< ":"<<vv << ":" << pattern<<endl; 
		
		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << pattern  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << pattern  << endtc;
			AosTC(rdata->isOk()) <<vv<<":"<<pattern<< endtc;
		}
	}

	return true;
}


bool
AosValuesetPatternTester::testNormalInt64()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"004\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\">";

	
	OmnString pattern ;
	pattern<<".*?" <<rand()%100 <<".*";


	str << "<![CDATA["<< pattern<<"]]></valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		
		int64_t vv = rand()%100000;

		if(rand()%2==0){
			vv *=-1;
		}
		
		AosValueRslt value(vv);

		try
		{
			std::string temp = pattern.data();
			boost::regex reg(temp);
			std::string s = value.getValueStr().data();
			if(boost::regex_match(s, reg))
			{
		    	isInenum =true;
			}
		}
		catch(const boost::bad_expression& e)
		{
			 rdata->setError()<<" pattern error";
			 return true;
		}
		
		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << pattern  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << pattern  << endtc;
			AosTC(rdata->isOk()) <<vv<<":"<<pattern<< endtc;
		}
	}

	return true;
}


bool
AosValuesetPatternTester::testNormalString()
{
	OmnString str = "<valueenum ";
	str << AOSTAG_VALUESET_TYPE << "=\"004\" "
		<<AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\">";

	
	int len1 = (rand() % 5)+1;
	char data[12];
	AosRandomLetterStr(len1, data);
	OmnString pattern ;
	pattern<<".*?" <<data <<".*";


	str << "<![CDATA["<< pattern<<"]]></valueenum>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInenum = false;
		
		int len1 = (rand() % 20)+1;
		char data3[22];
		AosRandomLetterStr(len1, data3);
		OmnString vv = data3;

		if(rand()%2==0){
			vv<<data;
		}

		try
		{
			std::string temp = pattern.data();
			boost::regex reg(temp);
			std::string s = vv.data();
			if(boost::regex_match(s, reg))
			{
		    	isInenum =true;
			}
		}
		catch(const boost::bad_expression& e)
		{
			 rdata->setError()<<" pattern error";
			 return true;
		}
		
		AosValueRslt value(vv);
		if (isInenum)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << pattern  << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << pattern  << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}
