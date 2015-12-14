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
// How to torture: 
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/Testers1/DocScannerTester.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DataScanner/DataScanner.h"
#include "DataScanner/DocScanner.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosDocScannerTester::AosDocScannerTester()
{
	mName = "DocScannerTester";
}


AosDocScannerTester::~AosDocScannerTester()
{
}


bool 
AosDocScannerTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDocScannerTester::basicTest()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	for (int i = 0; i< 1; i++)
	{
OmnScreen << "tries :" << i << endl;
		aos_assert_r(test(rdata), false);
		mNum ++;
	}
	return true;
}


AosXmlTagPtr
AosDocScannerTester::config(const AosRundataPtr &rdata)
{
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);

	AosXmlTagPtr scanner_conf = app_conf->getFirstChild("scanner"); 
	mCreateDoc = scanner_conf->getAttrBool("create", false);
	return scanner_conf;
}

bool
AosDocScannerTester::createScannerObj(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &scanner_conf)
{
	AosXmlTagPtr query = scanner_conf->getFirstChild("query");
	//modifyConfig(query);
	//mScanner = AosDataScanner::createDataScanner(scanner_conf, rdata);
	AosDocScanner *ss = OmnNew AosDocScanner(scanner_conf, rdata);
	mScanner = ss;
	//mScanner = AosDataScanner::createDataScanner(scanner_conf, rdata);
	aos_assert_rr(mScanner, rdata, false);
	return true;
}


bool
AosDocScannerTester::createDoc(const AosRundataPtr &rdata)
{
	if (!mCreateDoc) return true;
	rdata->setUserid(307);
	OmnString docstr;
	u64 docid;
	u64 end_docid = 0;
	AosXmlTagPtr dd;
	for (int i = 0; i < 550; i++)
	{
OmnScreen << "----------- :" << i << endl;
		docstr << "<vpd zky_a = '11' zky_pctrs='vpd' zky_public_doc='true' zky_publc_ctnr='true'/>";
		dd = AosCreateDoc(docstr, true, rdata); 
		aos_assert_r(dd, false);
		docid = dd->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(docid, false);
		mDocids.insert(make_pair(docid, 1));
		docstr = "";
//OmnScreen <<"i : " << i << ", docid :" << docid << ", pctrs :" << dd->getAttrStr("zky_pctrs", "")<< endl;
		end_docid = docid;
	}
OmnScreen << "=========== : " << end_docid << endl;
	return true;
}

bool
AosDocScannerTester::test(const AosRundataPtr &rdata)
{
	AosXmlTagPtr conf = config(rdata);
	aos_assert_r(tryDocid(rdata, conf), false);
	aos_assert_r(createDoc(rdata), false);
OmnSleep(5);
	aos_assert_r(createScannerObj(rdata, conf), false);
	aos_assert_r(proc(rdata), false);
	return true;
}

bool
AosDocScannerTester::proc(const AosRundataPtr &rdata)
{
	vector<AosDataScannerObjPtr> scanners;
	mScanner->split(scanners, 0, rdata);
	for (u32 i = 0; i< scanners.size(); i++)
	{
		while (1)
		{
			AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			aos_assert_r(scanners[i]->getNextBlock(buff, rdata), false);
			if (buff->dataLen()<= 0) break;

			aos_assert_r(verifyDoc(buff, rdata), false);
		}
	}
//	for (u32 i = 0; i< buffs.size(); i++)
//	{
//OmnScreen << "buff index :" << i << endl;
//		aos_assert_r(verifyDoc(buffs[i], rdata), false);
//OmnScreen << " ================= " << endl;
//	}
	
	map<u64, u64>::iterator itr;
	for (itr = mDocids.begin(); itr != mDocids.end(); itr++)
	{
OmnScreen <<"docid: " <<itr->first << " vv:"<<itr->second << endl;
		aos_assert_r(itr->second == 2, false);
	}
	return true;
}


bool
AosDocScannerTester::verifyDoc(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_r(buff->dataLen() > 0, false)
	OmnString ss;
	while(buff->getCrtIdx() < buff->dataLen())
	{
		ss = buff->getStr("");
		aos_assert_r(ss != "", false);
		AosXmlParser parser;
		AosXmlTagPtr doc = parser.parse(ss, "" AosMemoryCheckerArgs);
		aos_assert_r(doc, false);
		u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
//OmnScreen << "docid :" << docid << endl;
		aos_assert_r(docid, false);
		u64 vv = mDocids[docid];
		if (vv == 1)
		{
			mDocids[docid] = 2;
		}
		else
		{
			OmnAlarm << "Failed " << docid << " : " << vv << enderr;
		}
	}
	return true;
}


void
AosDocScannerTester::modifyConfig(const AosXmlTagPtr &config)
{
	OmnString objid = "vpd";
	objid << mNum;
	config->setNodeText("cond/zky_value", objid, false);
}

bool
AosDocScannerTester::tryDocid(const AosRundataPtr &rdata, const AosXmlTagPtr &conf)
{
	u64 crt_docid = conf->getAttrU64("zky_crtdocid", 0);
	aos_assert_r(crt_docid, false);
	int num_docs = 0;
	for (u64 docid = 5008; docid <= crt_docid; docid++)
	{
		mDocids[docid] = 1; 
		num_docs ++;
	}
OmnScreen << " num_docs: " << num_docs << endl;
	return true;
}
