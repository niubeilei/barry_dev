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
#include "DocSelector/Testers/DocSelectorTester.h"

#include "DocSelector/DocSelector.h"
#include "alarm_c/alarm.h"
#include "ActOpr/ValueRslt.h"
#include "Debug/Debug.h"
#include "DocSelector/Ptrs.h"
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
#include "SearchEngine/DocMgr.h"


AosDocSelectorTester::AosDocSelectorTester()
{
}


bool AosDocSelectorTester::start()
{
	cout << "    Start AosDocSelector Tester ..." << endl;
	basicTest();
	return true;
}


bool AosDocSelectorTester::basicTest()
{
	int tries = mTestMgr->getTries();
	OmnScreen << "Tries: " << tries << endl;
	if (tries <= 0) tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		OmnScreen << "Tries: " << i << endl;
		aos_assert_r(testArith(), false);
	}
	return true;
}


bool
AosDocSelectorTester::testArith()
{
	int vv = rand() % 100;
	if (vv < 80)
	{
		aos_assert_r(testNormalArith(), false);
		return true;
	}

	aos_assert_r(testNormalIncorrect(), false);
	return true;
}

	
bool
AosDocSelectorTester::testNormalArith()
{
	// Determine data type
	int vv = rand() % 100;
/*	if (vv <= 70)
	{
		aos_assert_r(testNormalString(), false);
		return true;
	}
*/
	if (vv <= 50)
	{
		aos_assert_r(testNormalU64(), false);
		return true;
	}
	
	aos_assert_r(testNormalInt64(), false);
	return true;
}


bool
AosDocSelectorTester::testNormalU64()
{
	// get zky_objid from docid
	//
	AosXmlTagPtr thedoc;
	for (int i=0; i<eNormalTries; i++)
	{
		//<zky_docselector zky_docselector_type="objid" 
		// zky_doc_objid="gicdesc_image_h"></zky_docselector>
		u64 did = (rand()%(200000-5000)) + 5000;
		thedoc = AosDocMgr::getSelf()->getDoc(did, "");
		if(!thedoc)
		{
			continue;
		}

		OmnString objid = thedoc->getAttrStr(AOSTAG_OBJID); 

		OmnString str = "<";
		str << AOSTAG_DOCSELECTOR << " ";
		str << AOSTAG_DOCSELECTOR_TYPE << "=\"objid\" ";
		str << AOSTAG_DOC_OBJID<<"=\""<< objid <<"\" >";
		str << "</"<<AOSTAG_DOCSELECTOR<<">";

		AosXmlParser parser;
		AosXmlTagPtr sdoc = parser.parse(str, "");
		aos_assert_r(sdoc, false);
		AosRundataPtr rdata = OmnNew AosRundata();
		rdata->setSiteid("100");
		rdata->resetForReuse(0);
		
		AosXmlTagPtr doc = AosDocSelector::doSelectDoc(sdoc, rdata);
		if(!doc)
		{
			rdata->setError() << "Missing Smart Doc";
			return true;
		}
		OmnString oid = doc->getAttrStr(AOSTAG_OBJID);

		if(objid != oid)
		{
			rdata->setError() << "Objid is not the same !";
			return false;
		}
		
		AosTC(AosDocSelector::doSelectDoc(sdoc, rdata))
		<< ":" << endtc;
		
		AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
	}
	return true;
}


bool
AosDocSelectorTester::testNormalInt64()
{
	AosXmlTagPtr thedoc;
	for (int i=0; i<eNormalTries; i++)
	{
		//<zky_docselector zky_docselector_type="objid" 
		// zky_doc_objid="gicdesc_image_h"></zky_docselector>
		//	OmnString docid = "114326"; 
	
		u64 did = (rand()%(200000-5000)) + 5000;
		thedoc = AosDocMgr::getSelf()->getDoc(did, "");
		if(!thedoc)
		{
			continue;
		}

		OmnString id;
		id << did;
		OmnString docid = id; 

		OmnString str = "<";
		str << AOSTAG_DOCSELECTOR << " ";
		str << AOSTAG_DOCSELECTOR_TYPE << "=\"docid\" ";
		str << AOSTAG_DOC_DOCID<<"=\""<< docid <<"\" >";
		str << "</"<<AOSTAG_DOCSELECTOR<<">";

		AosXmlParser parser;
		AosXmlTagPtr sdoc = parser.parse(str, "");
		aos_assert_r(sdoc, false);
		AosRundataPtr rdata = OmnNew AosRundata();
		rdata->setSiteid("100");
		rdata->resetForReuse(0);
		
		AosXmlTagPtr doc = AosDocSelector::doSelectDoc(sdoc, rdata);
		if(!doc)
		{
			rdata->setError() << "Missing Doc";
			return false;
		}
		OmnString oid = doc->getAttrStr(AOSTAG_DOCID);

		if(docid != oid)
		{
			rdata->setError() << "Docid is not the same !";
			return false;
		}
		
		AosTC(AosDocSelector::doSelectDoc(sdoc, rdata))
		<< ":" << endtc;
		
		AosTC(rdata->isOk()) << rdata->getErrmsg() << endtc;
	}
	return true;
}


bool
AosDocSelectorTester::testNormalString()
{

	return true;
}

bool
AosDocSelectorTester::testNormalIncorrect()
{
	// This function tests the cases that the sdoc is incorrect. 
	int vv = rand() % 100;
	if (vv <= 60)
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
AosDocSelectorTester::testEqualInt64()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_TYPE << "=\"001\" ";

	int64_t start = rand();

	str << AOSTAG_DATATYPE << "=\"" << AOSVALUE_INT64 << "\" "
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
AosDocSelectorTester::testEqualString()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_TYPE << "=\"001\" ";

	OmnString start = OmnRandom::nextPrintableStr(1, 10);
	str << AOSTAG_DATATYPE << "=\"" << AOSVALUE_STRING << "\" "
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
AosDocSelectorTester::testIncorrectU64()
{
	for (int i=0; i<eNormalTries; i++)
	{
		u64 left = rand();

		int len2 = (rand() % 10)+1;
		char data2[12];
		AosRandomLetterStr(len2, data2);
		OmnString right = data2;

		OmnString str = "<cond ";
		str << AOSTAG_DOCSELECTOR_TYPE << "=\"003\" ";
		if(i%6 == 0)
		{
			str << AOSTAG_OPERATOR << "=\"==\" >";
		}
		else if(i%6 == 1)
		{
			str << AOSTAG_OPERATOR << "=\"<\" >";
		}
		else if(i%6 == 2)
		{
			str << AOSTAG_OPERATOR << "=\">\" >";
		}
		else if(i%6 == 3)
		{
			str << AOSTAG_OPERATOR << "=\"!=\" >";
		}
		else if(i%6 == 4)
		{
			str << AOSTAG_OPERATOR << "=\"<=\" >";
		}
		else if(i%6 == 5)
		{
			str << AOSTAG_OPERATOR << "=\">=\" >";
		}
		
		str << "<" << AOSTAG_LHS << " " << AOSTAG_DATA_TYPE<< "=\"u64\"" << " "<< AOSTAG_VALUE_TYPE << "=\"const\">";   
		str << left;
		str << "</"<< AOSTAG_LHS << ">";

		str << "<" << AOSTAG_RHS << " "  << AOSTAG_DATA_TYPE<< "=\"string\"" << " " << AOSTAG_VALUE_TYPE << "=\"const\">";   
		str << right;
		str << "</"<< AOSTAG_RHS << ">";
		str << "</cond>";

		AosXmlParser parser;
		AosXmlTagPtr sdoc = parser.parse(str, "");
		aos_assert_r(sdoc, false);
		AosRundataPtr rdata = OmnNew AosRundata();
		rdata->resetForReuse(0);

//		AosTC(!AosCondition::evalCondStatic(sdoc, rdata))
//		<< right<<":"<< left<< endtc;
	}
	return true;
}


bool
AosDocSelectorTester::testIncorrectInt64()
{
	for (int i=0; i<eNormalTries; i++)
	{
		int64_t left = rand();

		int len2 = (rand() % 10)+1;
		char data2[12];
		AosRandomLetterStr(len2, data2);
		OmnString right = data2;

		OmnString str = "<cond ";
		str << AOSTAG_DOCSELECTOR_TYPE << "=\"003\" ";
		if(i%6 == 0)
		{
			str << AOSTAG_OPERATOR << "=\"==\" >";
		}
		else if(i%6 == 1)
		{
			str << AOSTAG_OPERATOR << "=\"<\" >";
		}
		else if(i%6 == 2)
		{
			str << AOSTAG_OPERATOR << "=\">\" >";
		}
		else if(i%6 == 3)
		{
			str << AOSTAG_OPERATOR << "=\"!=\" >";
		}
		else if(i%6 == 4)
		{
			str << AOSTAG_OPERATOR << "=\"<=\" >";
		}
		else if(i%6 == 5)
		{
			str << AOSTAG_OPERATOR << "=\">=\" >";
		}
		
		str << "<" << AOSTAG_LHS << " " << AOSTAG_DATA_TYPE<< "=\"int64\"" << " "<< AOSTAG_VALUE_TYPE << "=\"const\">";   
		str << left;
		str << "</"<< AOSTAG_LHS << ">";

		str << "<" << AOSTAG_RHS << " "  << AOSTAG_DATA_TYPE<< "=\"string\"" << " " << AOSTAG_VALUE_TYPE << "=\"const\">";   
		str << right;
		str << "</"<< AOSTAG_RHS << ">";
		str << "</cond>";

		AosXmlParser parser;
		AosXmlTagPtr sdoc = parser.parse(str, "");
		aos_assert_r(sdoc, false);
		AosRundataPtr rdata = OmnNew AosRundata();
		rdata->resetForReuse(0);

//		AosTC(!AosCondition::evalCondStatic(sdoc, rdata))
//		<< right<<":"<< left<< endtc;
	}
	return true;
}


bool
AosDocSelectorTester::testIncorrectString()
{
	for (int i=0; i<eNormalTries; i++)
	{
		int64_t left = rand();
		u64 right = rand();

		OmnString str = "<cond ";
		str << AOSTAG_DOCSELECTOR_TYPE << "=\"003\" ";
		if(i%6 == 0)
		{
			str << AOSTAG_OPERATOR << "=\"==\" >";
		}
		else if(i%6 == 1)
		{
			str << AOSTAG_OPERATOR << "=\"<\" >";
		}
		else if(i%6 == 2)
		{
			str << AOSTAG_OPERATOR << "=\">\" >";
		}
		else if(i%6 == 3)
		{
			str << AOSTAG_OPERATOR << "=\"!=\" >";
		}
		else if(i%6 == 4)
		{
			str << AOSTAG_OPERATOR << "=\"<=\" >";
		}
		else if(i%6 == 5)
		{
			str << AOSTAG_OPERATOR << "=\">=\" >";
		}
		
		str << "<" << AOSTAG_LHS << " " << AOSTAG_DATA_TYPE<< "=\"int64\"" << " "<< AOSTAG_VALUE_TYPE << "=\"const\">";   
		str << left;
		str << "</"<< AOSTAG_LHS << ">";

		str << "<" << AOSTAG_RHS << " "  << AOSTAG_DATA_TYPE<< "=\"u64\"" << " " << AOSTAG_VALUE_TYPE << "=\"const\">";   
		str << right;
		str << "</"<< AOSTAG_RHS << ">";
		str << "</cond>";

		AosXmlParser parser;
		AosXmlTagPtr sdoc = parser.parse(str, "");
		aos_assert_r(sdoc, false);
		AosRundataPtr rdata = OmnNew AosRundata();
		rdata->resetForReuse(0);

//		AosTC(!AosCondition::evalCondStatic(sdoc, rdata))
//		<< right<<":"<< left<< endtc;
	}

	return true;
}


bool
AosDocSelectorTester::testIncorrectXml()
{
	OmnString str = "<valuerange ";
	str << AOSTAG_TYPE << "=\"001\" ";

	u64 start = rand();
	u64 end = rand(); 
	if (start < end) 
	{
		u64 tt = start;
		start = end;
		end = tt;
	}

	str << AOSTAG_DATATYPE << "=\"" << AOSVALUE_XMLDOC << "\" "
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

