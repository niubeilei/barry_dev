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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocServer/BinaryDocTesters/BinaryDocTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "DocClient/DocidMgr.h"
#include "DocServer/DocSvr.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "DocServer/BinaryDocTesters/Ptrs.h"
#include "Thread/ThreadPool.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "Util1/Time.h"
#include "Util/ReadFile.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

OmnThreadPool sgThreadPool1("test");

AosBinaryDocTester::AosBinaryDocTester()
{
}


AosBinaryDocTester::~AosBinaryDocTester()
{
}


bool 
AosBinaryDocTester::start()
{
	AosRundataPtr rdata = OmnApp::getRundata()->clone(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	basicTest(rdata);
	return true;
}


bool
AosBinaryDocTester::basicTest(const AosRundataPtr &rdata)
{
	int thrds = 4;
	for (int i = 0; i < thrds; i++) 
	{
	    mValue[i] = OmnRandom::lowcaseStr(90000, 100000);
		AosBinaryDocTesterPtr thisptr(this, true);
		OmnThrdShellProcPtr req = OmnNew TestThrd(rdata, i, thisptr);
		sgThreadPool1.proc(req);
	}
	return true;
}


bool
AosBinaryDocTester::proc(const int idx, const AosRundataPtr &rdata)
{
	int num  = 0;
	int total = 10000000;
	for (int i = 0; i < total; i++)
	{
if (num++ % 1000 == 0)
	OmnScreen << "Thread  " << idx << " , total: " << total << " , num: " << num << endl;
		u64 docid =	createData(idx, rdata);
		checkDoc(docid, idx, rdata);
	}

	checkDoc(idx, rdata);
	OmnScreen << idx << "======================= Finished ====================" << endl;
}


u64
AosBinaryDocTester::createData(const int idx, const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = generateData(idx, rdata);
	aos_assert_r(doc, false);
	
	createBinaryDoc(doc, rdata);
	return doc->getAttrU64(AOSTAG_DOCID, 0);
}


AosXmlTagPtr
AosBinaryDocTester::generateData(const int idx, const AosRundataPtr &rdata)
{
	OmnString objid = "";
	u64 docid = AosDocidMgr::getSelf()->nextDocid(objid, rdata);
	mDocids[idx].push_back(docid);
	AosXmlTagPtr bin_doc;	
	OmnString docstr =  "<doc ";
	docstr << AOSTAG_DOCID << "=\"" << docid << "\" "
		<< AOSTAG_SITEID << "=\"" << rdata->getSiteid() << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_BINARYDOC << "\" "
		<< "zky_binarydoc = \"blockfile\""
		<< "source_fname = \"test_" << docid << ".txt" << "\" " 
		<< AOSTAG_BINARY_NODENAME << "=\"" << "binarynode" << "\" "
		<< AOSTAG_OBJID << "=\"" << "_ztdoc_" << docid << "\" >"
		<< "</doc>";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);                

	 //mValue = OmnRandom::lowcaseStr(900000, 1000000);
	//OmnScreen << "length : " << mValue.length() + mValue.length()<< endl;;

	buff->setOmnStr(mValue[idx]);
	buff->setOmnStr(mValue[idx]);

	doc->setTextBinary("binarynode", buff);

	return doc;
}


bool
AosBinaryDocTester::createBinaryDoc(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC) 
	{
		OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
		AosBuffPtr buff = doc->getNodeTextBinaryCopy(nodename AosMemoryCheckerArgs);
		aos_assert_r(buff && buff->dataLen() >0, false);

		OmnString signature;

		if (nodename == "")
		{
			doc->removeNodeTexts();
		}
		else
		{
			doc->removeNode(nodename, false, false);
		}
		rslt = AosDocSvr::getSelf()->createBinaryDoc(doc, buff, signature, rdata, 0);
		aos_assert_r(rslt, false);
		aos_assert_r(signature != "", false);
		doc->setAttr(AOSTAG_BINARY_DOC_SIGNATURE, signature);
	}

	rslt = AosDocSvr::getSelf()->createDoc(rdata, doc, 0);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBinaryDocTester::checkDoc(
		const u64 &docid,
		const int idx, 
		const AosRundataPtr &rdata)
{
//	for (int i = 0; i < mDocids[idx].size(); i++)
//	{
//		u64 docid = mDocids[idx][i];
		aos_assert_r(docid, false);

		AosBuffPtr doc_buff;
		AosXmlTagPtr doc = retrieveBinaryDoc(docid, doc_buff, rdata);
		aos_assert_r(doc, false);
		aos_assert_r(doc_buff && doc_buff->dataLen() > 0, false);
		doc_buff->reset();
		OmnString str = doc_buff->getOmnStr("");
		aos_assert_r(str == mValue[idx], false);
		str = doc_buff->getOmnStr("");
		aos_assert_r(str == mValue[idx], false);
//	}
	return true;
}


bool
AosBinaryDocTester::checkDoc(
		const int idx, 
		const AosRundataPtr &rdata)
{
	for (int i = 0; i < mDocids[idx].size(); i++)
	{
		u64 docid = mDocids[idx][i];
		aos_assert_r(docid, false);

		AosBuffPtr doc_buff;
		AosXmlTagPtr doc = retrieveBinaryDoc(docid, doc_buff, rdata);
		aos_assert_r(doc, false);
		aos_assert_r(doc_buff && doc_buff->dataLen() > 0, false);
		doc_buff->reset();
		OmnString str = doc_buff->getOmnStr("");
		aos_assert_r(str == mValue[idx], false);
		str = doc_buff->getOmnStr("");
		aos_assert_r(str == mValue[idx], false);
	}
	return true;
}





AosXmlTagPtr
AosBinaryDocTester::retrieveBinaryDoc(
		const u64 &docid, 
		AosBuffPtr &doc_buff, 
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(docid, rdata);    
	if(!doc)    return 0;

	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, 0);

	if (doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		doc = doc->clone(AosMemoryCheckerArgsBegin);
	}

	aos_assert_r(doc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") != "", 0);
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, 0);
	AosBuffPtr buff;
	bool rslt = AosDocSvr::getSelf()->retrieveBinaryDoc(doc, buff, rdata);
	if (!rslt)  return 0;
	aos_assert_r(buff && buff->dataLen() > 0, doc);
	OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME);
	doc->setTextBinary(nodename, buff);

	doc->removeAttr(AOSTAG_BINARY_DOC_SIGNATURE);


	AosXmlTagPtr xml = doc;
	aos_assert_r(xml->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, 0);
	aos_assert_r(xml->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") == "", 0);
	nodename = xml->getAttrStr(AOSTAG_BINARY_NODENAME, "");            
	doc_buff = xml->getNodeTextBinaryCopy(nodename AosMemoryCheckerArgs);
	aos_assert_r(doc_buff && doc_buff->dataLen() > 0, 0);

	if (nodename == "")
	{
		xml->removeNodeTexts();
	}
	else
	{
		xml->removeNode(nodename, false, false);
	}
	return xml;
}

