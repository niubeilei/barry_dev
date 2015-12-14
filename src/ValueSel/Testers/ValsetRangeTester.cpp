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
#include "Valueset/Testers/ValsetRangeTester.h"

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
#include "Valueset/ValsetRange.h"
#include "SEUtil/XmlGenerateRule.h"


AosValuesetRangeTester::AosValuesetRangeTester()
{
}


bool AosValuesetRangeTester::start()
{
	cout << "    Start AosValuesetRange Tester ..." << endl;
	basicTest();
	return true;
}


bool AosValuesetRangeTester::basicTest()
{
	
	AosValuesetRange rage(false);
	
	OmnString xml = "<valueset name='valueset'>";
		xml << "<required>";                                               
		xml << "<attrs>";
		xml << "<attr name=\"pane_type\"><![CDATA[[rangae]]]></attr>";
	    xml << "<attr name=\"start\"><![CDATA[(0#100)]]></attr>";
		xml << "<attr name=\"end\"><![CDATA[(0#100)]]></attr>";
		xml << "</attrs>";
		xml << "</required>";
		xml << "<optional>";
		xml << "<attrs><attr name=\"datatype\"><![CDATA[[string#int]]]></attr></attrs>";
		xml << "</optional>";
		xml << "</valueset>";
	AosXmlParser p;
	AosXmlTagPtr ruletag = p.parse(xml, "");

//	AosXmlGenerateRulePtr rule = new AosXmlGenerateRule(ruletag);
	AosXmlGenerateRulePtr rule = new AosXmlGenerateRule("valueset_rang");

	int tries = mTestMgr->getTries();
	OmnScreen << "Tries: " << tries << endl;
	if (tries <= 0) tries = 1000000;
	for (int i=0; i<tries; i++)
	{
cout <<  rage.getXml(rule) << endl;
		//aos_assert_r(testOneRange(), false);
	}
	return true;
}


bool
AosValuesetRangeTester::testOneRange()
{
	int vv = rand() % 100;
	if (vv < 80)
	{
		aos_assert_r(testNormalRange(), false);
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
AosValuesetRangeTester::testNormalRange()
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
AosValuesetRangeTester::testNormalU64()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";
	
	u64 start = rand()%10000;
	u64 end = rand()%10000;
	if (start > end)
	{
		u64 tt = start;
		start = end;
		end = tt;
	}

	u64 step = rand()%50+1;
	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_U64 << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << end << "\" ";
	bool hasstep = false;
	if(rand()%3==0)
	{
		str<< AOSTAG_STEP << "=\""<< step <<"\"";
		hasstep = true;
	}
	str<< "/>";
	
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);


	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		u64 vv = rand()%10000;
		AosValueRslt value(vv);
		bool isInRange = false;
		if (vv >= start && vv <= end )
		{
			if(hasstep){
				if((vv-start)%step == 0)
				{
					isInRange = true;
				}
			}
			else
			{
				isInRange = true;
			}
		}
		if(isInRange)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << ":" << end << "step:"<<step<<endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << ":" << end << "step:"<<step<<endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetRangeTester::testNormalInt64()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";
	
	int64_t start = rand()%10000;
	int64_t end = rand()%10000;
	if (start > end)
	{
		int64_t tt = start;
		start = end;
		end = tt;
	}

	int64_t step = rand()%50+1;
	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << end << "\" ";
	bool hasstep = false;
	if(rand()%3==0)
	{
		str<< AOSTAG_STEP << "=\""<< step <<"\"";
		hasstep = true;
	}
	str<< "/>";
	
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);


	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		int64_t vv = rand()%10000;
		AosValueRslt value(vv);
		bool isInRange = false;
		if (vv >= start && vv <= end )
		{
			if(hasstep){
				if((vv-start)%step == 0)
				{
					isInRange = true;
				}
			}
			else
			{
				isInRange = true;
			}
		}
		if(isInRange)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << ":" << end << "step:"<<step<<endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << ":" << end << "step:"<<step<<endtc;
			AosTC(rdata->isOk()) << endtc;
		}
	}

	return true;
}


bool
AosValuesetRangeTester::testNormalString()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";

	OmnString start = OmnRandom::nextPrintableStr(1, 10);
	OmnString end = OmnRandom::nextPrintableStr(1, 10);
	if (start > end)
	{
		OmnString tt = start;
		start = end;
		end = tt;
	}

	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << end << "\"/>";
	
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	for (int i= 0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		OmnString vv = OmnRandom::nextPrintableStr(1, 10);
		AosValueRslt value(vv);
		if ((strcmp(vv.data(), start.data()) >= 0) && 
			(strcmp(vv.data(), end.data()) <= 0))
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << ":" << end << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << ":" << end << endtc;
		}
		AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
	}

	return true;
}


bool
AosValuesetRangeTester::testNormalEqual()
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
AosValuesetRangeTester::testNormalIncorrect()
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
AosValuesetRangeTester::testEqualU64()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";

	u64 start = rand();

	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_U64 << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << start << "\"/>";
	
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	u64 vv;
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		if ((rand() % 3) == 0)
		{
			vv = start;
		}
		else
		{
			vv = rand();
		}

		AosValueRslt value(vv);
		if (vv == start)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
	}

	return true;
}


bool
AosValuesetRangeTester::testEqualInt64()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";

	int64_t start = rand();

	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << start << "\"/>";
	
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	int64_t vv;
	for (int i=0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		if ((rand() % 3) == 0)
		{
			vv = start;
		}
		else
		{
			vv = rand();
		}

		AosValueRslt value(vv);
		if (vv == start)
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << endtc;
			AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
		}
	}

	return true;
}


bool
AosValuesetRangeTester::testEqualString()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";

	OmnString start = OmnRandom::nextPrintableStr(1, 10);
	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << start << "\"/>";
	
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");
	aos_assert_r(sdoc, false);

	AosRundataPtr rdata = OmnNew AosRundata();
	OmnString vv;
	for (int i= 0; i<eNormalTries; i++)
	{
		rdata->resetForReuse(0);
		if ((rand() % 3) == 0)
		{
			vv = start;
		}
		else
		{
			vv = OmnRandom::nextPrintableStr(1, 10);
		}
		AosValueRslt value(vv);
		if ((strcmp(vv.data(), start.data()) == 0)) 
		{
			AosTC(AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << endtc;
		}
		else
		{
			AosTC(!AosValueset::checkValuesetStatic(value, sdoc, rdata))
				<< vv << ":" << start << endtc;
		}
		AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
	}

	return true;
}


bool
AosValuesetRangeTester::testIncorrectU64()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";

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
			<< vv << ":" << rdata->getErrmsg() << endtc;
		AosTC(!rdata->isOk()) << endtc;
	}

	return true;
}


bool
AosValuesetRangeTester::testIncorrectInt64()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";

	int64_t start = rand();
	int64_t end = rand();
	if (start < end)
	{
		u64 tt = start;
		start = end;
		end = tt;
	}

	if (start <= end) return true;

	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_INT64 << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << end << "\"/>";
	
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
			<< vv << ":" << rdata->getErrmsg() << endtc;
		AosTC(!rdata->isOk()) << endtc;
	}

	return true;
}


bool
AosValuesetRangeTester::testIncorrectString()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";

	OmnString start = OmnRandom::nextPrintableStr(1, 10);
	OmnString end = OmnRandom::nextPrintableStr(1, 10);
	if (strcmp(start.data(), end.data()) < 0)
	{
		OmnString tt = start;
		start = end;
		end = tt;
	}

	str << AOSTAG_DATA_TYPE << "=\"" << AOSVALUE_STRING << "\" "
		<< AOSTAG_START << "=\"" << start << "\" "
		<< AOSTAG_END << "=\"" << end << "\"/>";
	
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
			<< vv << ":" << rdata->getErrmsg() << endtc;
		AosTC(!rdata->isOk()) << endtc;
	}

	return true;
}


bool
AosValuesetRangeTester::testIncorrectXml()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_VALUESET_TYPE << "=\"001\" ";

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

	return true;
}

