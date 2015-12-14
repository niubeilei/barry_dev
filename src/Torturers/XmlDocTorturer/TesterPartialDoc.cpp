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
// 07/20/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Torturers/XmlDocTorturer/TesterPartialDoc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "Torturers/XmlDocTorturer/XmlTagRand.h"
#include "Torturers/XmlDocTorturer/TesterLogin.h"
#include "Torturers/XmlDocTorturer/Ptrs.h"
#include "Tester/TestPkg.h"
#include "SengTorUtil/StDoc.h"
#include "Tester/TestPkg.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"
#include "Util/UtUtil.h"
#include "Rundata/Rundata.h"

AosPartialDocTester::AosPartialDocTester(
		const bool regflag)
:
AosSengTester(AosSengTester_PartialDoc, AosTesterId::ePartialDoc, regflag)
{
}


AosPartialDocTester::AosPartialDocTester()
:
AosSengTester(AosSengTester_PartialDoc, "partialdoc", AosTesterId::ePartialDoc)
{
}


AosPartialDocTester::~AosPartialDocTester()
{
}


bool 
AosPartialDocTester::test()
{
	AosRundataPtr rdata = mThread->getRundata();
	//mRundata = rdata;
	aos_assert_r(QueryDoc(), false);
	if (mObjid == "") return true;
	aos_assert_r(modifyPartialdDoc(), false);
	aos_assert_r(checkResults(), false);
	aos_assert_r(modifyCompleteDoc(), false);
	aos_assert_r(checkModifyCompleteResults(), false);
	return true;
}

bool
AosPartialDocTester::QueryDoc()
{
	mObjid= pickDoc(10);
	if (mObjid == "") return true;
	OmnString fnames;
	int i = 2;
	if (i ==1)
	{
		fnames << AOSTAG_OBJID << "|$|" << AOSTAG_OBJID << "|$|" << "1" << "|$$|" 
			<< "zky_a" << "|$|" << "zky_aa" << "|$|" << "1" << "|$$|"
			<< "zky_b" << "|$|" << "zky_bb" << "|$|" << "1" << "|$$|"
			<< "zky_c" << "|$|" << "zky_cc" << "|$|" << "1" << "|$$|"
			<< "zky_d" << "|$|" << "zky_dd" << "|$|" << "1" << "|$$|"
			<< "zky_e" << "|$|" << "zky_ee" << "|$|" << "1" << "|$$|"
			<< "zky_f" << "|$|" << "zky_ff" << "|$|" << "1" << "|$$|"
			<< "abcde/zky_g" << "|$|" << "zky_gg" << "|$|" << "1" << "|$$|"
				<< "abcde/zky_h" << "|$|" << "zky_hh" << "|$|" << "1" << "|$$|"
			<< "abcde/zky_i" << "|$|" << "zky_ii" << "|$|" << "1" << "|$$|"
			<< "abcde/zky_j" << "|$|" << "zky_jj" << "|$|" << "1";
	}
	if (i == 0)	
	{
		fnames << AOSTAG_OBJID << "|$|" << AOSTAG_OBJID << "|$|" << "1" << "|$$|" 
			<< "aaaaaa" << "|$|" << "aaaaaa2" << "|$|" << "2" << "|$$|"
			<< "bbbbbb" << "|$|" << "bbbbbb2" << "|$|" << "2" << "|$$|"
			<< "cccccc" << "|$|" << "cccccc2" << "|$|" << "2" << "|$$|"
			<< "dd/d1" << "|$|" << "dd2" << "|$|" << "2" << "|$$|"
			<< "ee/e1" << "|$|" << "ee2" << "|$|" << "2" << "|$$|"
			<< "ffffff" << "|$|" << "ffffff2" << "|$|" << "2"; 

	}
	if (i == 2)
	{
		fnames << AOSTAG_OBJID << "|$|" << AOSTAG_OBJID << "|$|" << "1" << "|$$|" 
			<< "aaaaaa" << "|$|" << "aaaaaa2" << "|$|" << "2" << "|$$|"
			<< "bbbbbb" << "|$|" << "bbbbbb2" << "|$|" << "2" << "|$$|"
			<< "zky_c" << "|$|" << "zky_cc" << "|$|" << "1" << "|$$|"
			<< "zky_d" << "|$|" << "zky_dd" << "|$|" << "1";
	}
	
	AosXmlTagPtr resproot;// = AosSengAdmin::getSelf()->queryDocByObjid(mThread->getSiteid(), mObjid, fnames, -1, -1);
	aos_assert_r(resproot, false);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	aos_assert_r(child1, false);
	AosXmlTagPtr child2 = child1->getFirstChild();
	aos_assert_r(child2, false);
	OmnString data = child2->toString();
	AosXmlParser parser;
	AosXmlTagPtr record= parser.parse(data, "" AosMemoryCheckerArgs);
	record ->setAttr("zky_aa", "aa");
	record->setAttr("zky_bb", "bb");
	if (i==1)
	{
		record->setAttr("zky_cc", "cc");
		record->setAttr("zky_gg", "gg");
		record->setAttr("zky_ii", "ii");
		record->removeAttr("zky_dd");
		record->removeAttr("zky_ff");
		record->removeAttr("zky_hh");
		record->removeAttr("zky_jj");

		record->setAttr("zky_a1", "a1");
		record->setAttr("zky_b1", "b1");
		record->setAttr("zky_c1", "c1");

		mPartilaDoc = record;
		return true;
	}
	if (i == 0)
	{
		record->xpathSetAttr("aaaaaa2/_#text", "a111111");
		record->xpathSetAttr("bbbbbb2/_#text", "b222222");
		record->xpathSetAttr("cccccc2/_#text", "c333333");
		record->xpathSetAttr("dd2/_#text",  "d444444");
		record->xpathSetAttr("ee2/_#text",  "e555555");
		record->xpathSetAttr("ffffff2/_#text", "f666666");	

		record->setNodeText("gggggg2", "g777777", false);
		record->setNodeText("hhhhhh2", "h888888", false);
		mPartilaDoc = record;
		return true;
	}
	if (i == 2)
	{

		record->xpathSetAttr("aaaaaa2/_#text", "a111111");
		record->xpathSetAttr("bbbbbb2/_#text", "b222222");
		record->setAttr("zky_cc", "cc");
		record->removeAttr("zky_dd");
		record->setNodeText("gggggg2", "g777777", false);
		record->setNodeText("hhhhhh2", "h888888", false);
		mPartilaDoc = record;
		return true;
	}
	return false;
}

bool
AosPartialDocTester::modifyPartialdDoc()
{
	bool rslt = mThread->modifyObj(mPartilaDoc, false);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosPartialDocTester::checkResults()
{
	AosXmlTagPtr doc = mThread->retrieveDocByObjid(mObjid); 
	aos_assert_r(doc, false);

	bool exist;
	int i = 2;
	if (i==1)
	{
		aos_assert_r(doc->getAttrStr("zky_a") == "aa", false);
		aos_assert_r(doc->getAttrStr("zky_b") == "bb", false);
		aos_assert_r(doc->getAttrStr("zky_c") == "cc", false);
		aos_assert_r(doc->getAttrStr("zky_d") == "", false);
		aos_assert_r(doc->getAttrStr("zky_f") == "", false);
		aos_assert_r(doc->getAttrStr("zky_a1") == "a1", false);
		aos_assert_r(doc->getAttrStr("zky_b1") == "b1", false);
		aos_assert_r(doc->getAttrStr("zky_c1") == "c1", false);
		aos_assert_r(doc->xpathQuery("abcde/zky_g", exist, "") == "gg", false);
		aos_assert_r(doc->xpathQuery("abcde/zky_i", exist, "") == "ii", false);
		aos_assert_r(doc->xpathQuery("abcde/zky_h", exist, "") == "", false);
		aos_assert_r(doc->xpathQuery("abcde/zky_j", exist, "") == "", false);
		return true;
	}

	if (i == 0)
	{
		aos_assert_r(doc->getAttrStr("zky_aa") == "aa", false);
		aos_assert_r(doc->getAttrStr("zky_bb") == "bb", false);
		aos_assert_r(doc->xpathQuery("aaaaaa/_#text", exist, "") == "a111111", false);
		aos_assert_r(doc->xpathQuery("bbbbbb/_#text", exist, "") == "b222222", false);
		aos_assert_r(doc->xpathQuery("cccccc/_#text", exist, "") == "c333333", false);
		aos_assert_r(doc->xpathQuery("dd/d1/_#text", exist, "") == "d444444", false);
		aos_assert_r(doc->xpathQuery("ee/e1/_#text", exist, "") == "e555555", false);
		aos_assert_r(doc->xpathQuery("ffffff/_#text", exist, "") == "f666666", false);
		aos_assert_r(doc->xpathQuery("gggggg2/_#text", exist, "") == "g777777", false);
		aos_assert_r(doc->xpathQuery("hhhhhh2/_#text", exist, "") == "h888888", false);
		return true;
	}
	
	if (i == 2)
	{
		aos_assert_r(doc->getAttrStr("zky_aa") == "aa", false);
		aos_assert_r(doc->getAttrStr("zky_bb") == "bb", false);
		aos_assert_r(doc->xpathQuery("aaaaaa/_#text", exist, "") == "a111111", false);
		aos_assert_r(doc->xpathQuery("bbbbbb/_#text", exist, "") == "b222222", false);
		aos_assert_r(doc->getAttrStr("zky_c") == "cc", false);
		aos_assert_r(doc->getAttrStr("zky_d") == "", false);
		aos_assert_r(doc->xpathQuery("gggggg2/_#text", exist, "") == "g777777", false);
		aos_assert_r(doc->xpathQuery("hhhhhh2/_#text", exist, "") == "h888888", false);
		return true;
	}
	return false;
}

bool
AosPartialDocTester::modifyCompleteDoc()
{
	bool rslt = mThread->modifyObj(mCompleteDoc, false);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosPartialDocTester::checkModifyCompleteResults()
{
	AosXmlTagPtr doc = mThread->retrieveDocByObjid(mObjid); 
	aos_assert_r(doc, false);

	int i = 0;
	if (i == 1)
	{
		aos_assert_r(doc->getAttrStr("zky_a") != "aa", false);
		aos_assert_r(doc->getAttrStr("zky_b") != "bb", false);
		aos_assert_r(doc->getAttrStr("zky_c") != "cc", false);
		aos_assert_r(doc->getAttrStr("zky_d") != "", false);
		aos_assert_r(doc->getAttrStr("zky_f") != "", false);
		bool exist;
		aos_assert_r(doc->xpathQuery("abcde/zky_g", exist, "") != "gg", false);
		aos_assert_r(doc->xpathQuery("abcde/zky_i", exist, "") != "ii", false);
		aos_assert_r(doc->xpathQuery("abcde/zky_h", exist, "") != "", false);
		aos_assert_r(doc->xpathQuery("abcde/zky_j", exist, "") != "", false);
		return true;
	}
	bool exist;
	aos_assert_r(doc->getAttrStr("zky_aa") !="aa", false);
	aos_assert_r(doc->getAttrStr("zky_bb") != "bb", false);
	aos_assert_r(doc->xpathQuery("aaaaaa/_#text", exist, "") != "a111111", false);
	aos_assert_r(doc->xpathQuery("bbbbbb/_#text", exist, "") != "b222222", false);
	aos_assert_r(doc->xpathQuery("cccccc/_#text", exist, "") != "c333333", false);
	aos_assert_r(doc->xpathQuery("dd/d1/_#text", exist, "") != "d444444", false);
	aos_assert_r(doc->xpathQuery("ee/e1/_#text", exist, "") != "e555555", false);
	aos_assert_r(doc->xpathQuery("ffffff/_#text", exist, "") != "f666666", false);
	aos_assert_r(doc->xpathQuery("gggggg2/_#text", exist, "") != "g777777", false);
	aos_assert_r(doc->xpathQuery("hhhhhh2/_#text", exist, "") != "h888888", false);
	return true;


}

OmnString
AosPartialDocTester::pickDoc(int loop)
{
	OmnString objid;
	int i;
	int total = mThread->getStDocSize();
	for(i = 1; i<total; i++)
	{
		AosStDocPtr doc = mThread->pickDoc();
		if (doc)
		{
			objid= doc->objid;
			AosStContainerPtr container = mThread->getContainer(doc->locdid);
			AosStUser *user = mThread->getUser(doc->locdid);
			const AosStAccessRecord *ar = mThread->getLocalAccessRecord(doc->locdid);
			if ((!container) && (!user) && (!ar))
			{
				//Not allow delete container and user
				break;
			}
		}
	}
	if (i >= total)
	{
		return "";
	}

	AosXmlTagPtr doc = mThread->retrieveDocByObjid(objid); 
	aos_assert_r(doc, ""); 
	if (doc->getAttrStr("zky_zyx") != "1111")
	{
		if (loop > 0)
		{
			return "";
		}
		pickDoc(loop);
	}
	mCompleteDoc= doc;
	return objid;                              
}

/*
bool
AosPartialDocTester::modifyDoc()
{
	mRawDoc->setAttr("zky_zyx", 1111);
	int i = 2;
	if (i== 1)
	{
		mRawDoc->setAttr("zky_a", "111");
		mRawDoc->setAttr("zky_b", "111");
		mRawDoc->setAttr("zky_c", "111");
		mRawDoc->setAttr("zky_d", "111");
		mRawDoc->setAttr("zky_e", "111");
		mRawDoc->setAttr("zky_f", "111");
		OmnString node = "<abcde/>";
		AosXmlParser parser;
		AosXmlTagPtr nodexml = parser.parse(node, "");
		nodexml->setAttr("zky_g", "111");
		nodexml->setAttr("zky_h", "111");
		nodexml->setAttr("zky_i", "111");
		nodexml->setAttr("zky_j", "111");
		nodexml->setAttr("zky_k", "111");
		nodexml->setAttr("zky_l", "111");
		nodexml->setAttr("zky_m", "111");
		mRawDoc->addNode(nodexml);
		return true;
	}

	if (i == 0)
	{
		mRawDoc->setNodeText("aaaaaa", "zzzzzz", true);
		mRawDoc->setNodeText("bbbbbb", "yyyyyy", true);
		mRawDoc->setNodeText("cccccc", "xxxxxx", false);
		mRawDoc->setNodeText("dd/d1",  "uuuuuu", true);
		mRawDoc->setNodeText("ee/e1",  "vvvvvv", true);
		mRawDoc->setNodeText("ffffff", "wwwwww", false);
		return true;
	}

	if (i == 2)
	{
		mRawDoc->setAttr("zky_c", "111");
		mRawDoc->setAttr("zky_d", "111");
		mRawDoc->setNodeText("aaaaaa", "zzzzzz", true);
		bool exist;
		aos_assert_r(mRawDoc->xpathQuery("aaaaaa/_#text", exist, "") == "zzzzzz", false);
		mRawDoc->setNodeText("bbbbbb", "yyyyyy", true);
		aos_assert_r(mRawDoc->xpathQuery("bbbbbb/_#text", exist, "") == "yyyyyy", false);
	}
}
*/
