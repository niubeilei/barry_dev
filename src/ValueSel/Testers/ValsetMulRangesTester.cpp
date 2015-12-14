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
#include "Valueset/Testers/ValsetMulRangesTester.h"

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
#include <vector>

AosValuesetMulRangesTester::AosValuesetMulRangesTester()
{
}


bool AosValuesetMulRangesTester::start()
{
	cout << "    Start AosValuesetMulRanges Tester ..." << endl;
	basicTest();
	return true;
}


bool AosValuesetMulRangesTester::basicTest()
{
	int tries = mTestMgr->getTries();
	OmnScreen << "Tries: " << tries << endl;
	if (tries <= 0) tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		aos_assert_r(testOneMulRanges(), false);
	}
	return true;
}


bool
AosValuesetMulRangesTester::testOneMulRanges()
{
	int vv = rand() % 100;
	if (vv < 80)
	{
		aos_assert_r(testNormalMulRanges(), false);
		return true;
	}
	if (vv < 95)
	{
		aos_assert_r(testNormalEqual(), false);
		return true;
	}

	aos_assert_r(testNormalIncorrect(), false);
	return true;
}

	
bool
AosValuesetMulRangesTester::testNormalMulRanges()
{
	// Determine data type
	int vv = rand() % 100;
	if (vv <= 40)
	{
		aos_assert_r(testNormalU64(), false);
		return true;
	}
	if (vv <= 80)
	{
		aos_assert_r(testNormalInt64(), false);
		return true;
	}
	
	aos_assert_r(testNormalString(), false);
	return true;
}


bool
AosValuesetMulRangesTester::testNormalU64()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	int child_num = rand()%10+1;
	
	std::vector<u64> start_val;
	std::vector<u64> end_val;
	std::vector<u64> step_val;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		u64 start = rand();
		u64 end = rand();
		u64 step = rand()%5+1;
		if (start > end)
		{
			u64 tt = start;
			start = end;
			end = tt;
		}
		start_val.push_back(start);
		end_val.push_back(end);
		step_val.push_back(step);
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_U64 << "\" "
			<< AOSTAG_STEP << "=\""<< step <<"\""
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << end << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInRange = false;
		u64 vv = rand();
		for(int j=0;j<child_num;j++){
			if(vv >= start_val[j] && vv <= end_val[j] && (vv-start_val[j])%step_val[j]==0){
				isInRange = true;
				break;
			}
		}
		
		
		AosValueRslt value(vv);
		if (isInRange)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv  <<"------------"<< endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetMulRangesTester::testNormalInt64()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	int child_num = rand()%10+1;
	
	std::vector<int64_t> start_val;
	std::vector<int64_t> end_val;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		int64_t start = rand()/10000;
		int64_t end = rand()/10000;
		if ((rand() % 5) == 0) start *= -1;
		if ((rand() % 5) == 0) end *= -1;
		if (start > end)
		{
			int64_t tt = start;
			start = end;
			end = tt;
		}
		start_val.push_back(start);
		end_val.push_back(end);
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\" "
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << end << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInRange = false;
		int64_t vv = rand()/10000;
		if ((rand() % 5) == 0) vv *= -1;
		for(int j=0;j<child_num;j++){
			if(vv >= start_val[j] && vv <= end_val[j]){
				isInRange = true;
				break;
			}
		}
		
		
		AosValueRslt value(vv);
		if (isInRange)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv  <<"------------"<< endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetMulRangesTester::testNormalString()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	int child_num = rand()%10+1;
	
	std::vector<OmnString> start_val;
	std::vector<OmnString> end_val;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		OmnString start = OmnRandom::nextPrintableStr(1, 10);
		OmnString end = OmnRandom::nextPrintableStr(1, 10);
		if (start > end)
		{
			OmnString tt = start;
			start = end;
			end = tt;
		}
		start_val.push_back(start);
		end_val.push_back(end);
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\" "
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << end << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInRange = false;
		OmnString vv = OmnRandom::nextPrintableStr(1, 10);;
		for(int j=0;j<child_num;j++){
			if ((strcmp(vv.data(), start_val[j].data()) >= 0) && 
				(strcmp(vv.data(), end_val[j].data()) <= 0))
			{
				isInRange = true;
				break;
			}
		}
		
		
		AosValueRslt value(vv);
		if (isInRange)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv  <<"------------"<< endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetMulRangesTester::testNormalEqual()
{
	// It tests the cases that the start and end are the same
	// Determine data type
	int vv = rand() % 100;
	if (vv <= 40)
	{
		aos_assert_r(testEqualU64(), false);
		return true;
	}

	if (vv <= 80)
	{
		aos_assert_r(testEqualInt64(), false);
		return true;
	}
	
	aos_assert_r(testEqualString(), false);
	return true;
}


bool
AosValuesetMulRangesTester::testNormalIncorrect()
{
	// This function tests the cases that the sdoc is incorrect. 
	int vv = rand() % 100;
	if (vv <= 40)
	{
		aos_assert_r(testIncorrectU64(), false);
		return true;
	}

	if (vv <= 80)
	{
		aos_assert_r(testIncorrectInt64(), false);
		return true;
	}
	
	if (vv <= 95)
	{
		aos_assert_r(testIncorrectString(), false);
		return true;	
	}

	aos_assert_r(testIncorrectXml(), false);
	return true;
}


bool
AosValuesetMulRangesTester::testEqualU64()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	int child_num = rand()%10+1;
	
	std::vector<u64> start_val;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		u64 start = rand()%10000;
		start_val.push_back(start);
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_U64 << "\" "
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << start << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInRange = false;
		u64 vv = rand()%10000;
		for(int j=0;j<child_num;j++){
			if(vv==start_val[j]){
				isInRange = true;
				break;
			}
		}
		if(!isInRange){
			if ((rand() % 3) == 0)
			{
				vv = start_val[rand()%child_num];
				isInRange = true;
			}
		}
		
		AosValueRslt value(vv);
		if (isInRange)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv  <<"------------"<< endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetMulRangesTester::testEqualInt64()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	int child_num = rand()%10+1;
	
	std::vector<int64_t> start_val;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		int64_t start = rand()%10000;
		if(rand()%3==0) start*= -1;
		start_val.push_back(start);
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\" "
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << start << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInRange = false;
		int64_t vv = rand()%10000;
		if(rand()%3==0) vv *= -1;
		for(int j=0;j<child_num;j++){
			if(vv==start_val[j]){
				isInRange = true;
				break;
			}
		}
		if(!isInRange){
			if ((rand() % 3) == 0)
			{
				vv = start_val[rand()%child_num];
				isInRange = true;
			}
		}
		
		AosValueRslt value(vv);
		if (isInRange)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv  <<"------------"<< endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetMulRangesTester::testEqualString()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	int child_num = rand()%10+1;
	
	std::vector<OmnString> start_val;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		OmnString start = OmnRandom::nextPrintableStr(1, 10);
		start_val.push_back(start);
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\" "
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << start << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		bool isInRange = false;
		OmnString vv = OmnRandom::nextPrintableStr(1, 10);
		for(int j=0;j<child_num;j++){
			if(vv==start_val[j]){
				isInRange = true;
				break;
			}
		}
		if(!isInRange){
			if ((rand() % 3) == 0)
			{
				vv = start_val[rand()%child_num];
				isInRange = true;
			}
		}
		
		AosValueRslt value(vv);
		if (isInRange)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv  <<"------------"<< endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetMulRangesTester::testIncorrectU64()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	
	int child_num = rand()%10+1;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		u64 start = rand();
		u64 end = rand();
		if (start < end)
		{
			u64 tt = start;
			start = end;
			end = tt;
		}
		
		if (start <= end) return true;
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_U64 << "\" "
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << end << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		u64 vv = rand();
		AosValueRslt value(vv);
		
		AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
		AosTC(rdata->isOk()) << endtc;
	}

	return true;
}


bool
AosValuesetMulRangesTester::testIncorrectInt64()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	
	int child_num = rand()%10+1;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		int64_t start = rand();
		int64_t end = rand();
		if (start < end)
		{
			int64_t tt = start;
			start = end;
			end = tt;
		}
		
		if (start <= end) return true;
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\" "
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << end << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		int64_t vv = rand();
		AosValueRslt value(vv);
		
		AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
		AosTC(rdata->isOk()) << endtc;
	}

	return true;
}


bool
AosValuesetMulRangesTester::testIncorrectString()
{
	OmnString str = "<valueranges ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\">";
	
	int child_num = rand()%10+1;
	for(int i=0;i<child_num;i++){
		str<< "<range ";
		OmnString start = OmnRandom::nextPrintableStr(1, 10);
		OmnString end = OmnRandom::nextPrintableStr(1, 10);
		if (strcmp(start.data(), end.data()) < 0)
		{
			OmnString tt = start;
			start = end;
			end = tt;
		}
		
		if (strcmp(start.data(), end.data()) <= 0) return true;
		
		str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\" "
			<< AOSTAG_START << "=\"" << start << "\" "
			<< AOSTAG_END << "=\"" << end << "\"/>";
	}
	
	str<<"</valueranges>";

	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		OmnString vv = OmnRandom::nextPrintableStr(1, 10);
		AosValueRslt value(vv);
		
		AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				//<< vv << ":" << start << ":" << end << endtc;
				<< vv << endtc;
		AosTC(rdata->isOk()) << endtc;
	}

	return true;
}


bool
AosValuesetMulRangesTester::testIncorrectXml()
{
/*	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"002\" ";

	u64 start = rand();
	u64 end = rand(); 
	if (start < end) 
	{
		u64 tt = start;
		start = end;
		end = tt;
	}

	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_XMLDOC << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << end << "\"/>";
	
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		u64 vv = rand();
		AosValueRslt value(vv);
		AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
			<< (u64)vv << ":" << rdata->getErrmsg() << endtc;
		AosTC(!rdata->isOk()) << endtc;
	}
*/
	return true;
}

