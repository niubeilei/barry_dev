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
// 07/13/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocClient/Testers/DocClientTester.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "DocClient/DocClient.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"
#include "Util/Buff.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/Test.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "Porting/Sleep.h"
#include "AppMgr/App.h"


AosDocClientTester::AosDocClientTester()
{
}


bool 
AosDocClientTester::start()
{
	cout << "    Start AosDocClientTester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDocClientTester::basicTest()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	rdata->setUserid(307);
	for (int i = 0; i< 1 ; i++)
	{
		OmnScreen << "i : " << i << endl;
		rdata->resetTTL();
		bool rslt = test(rdata);		
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosDocClientTester::test(const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc, xml;
	createPctrs(rdata);
	for (int i = 0; i< 1000; i++)
	{
		xml = createBinaryDoc(rdata);
		aos_assert_r(xml, false);	


u64 docid = xml->getAttrU64(AOSTAG_DOCID, 0);
int vid  = AosGetVirtualIDByDocid(docid);
OmnScreen << " docid : " << docid << " , vid : " << vid << endl;
		doc = retrieveBinaryDoc(xml, rdata);	
		aos_assert_r(doc, false);
		rdata->resetTTL();
	}
return true;
OmnScreen << "================================" << endl;
	for (int i = 0; i< 1000; i++)
	{
		OmnScreen << "i : " << i << endl;
		rdata->resetTTL();

		OmnString objid =  AOSOBJIDPRE_DFTOBJID ;
		objid << (5009 + i);
		xml = xml->clone(AosMemoryCheckerArgsBegin);
		xml->setAttr(AOSTAG_OBJID, objid);
		doc = retrieveBinaryDoc(xml, rdata);	
		aos_assert_r(doc, false);
		bool rslt = deleteBinaryDoc(doc, rdata);	
		aos_assert_r(rslt, false);
	}
	return true;
}

void 
AosDocClientTester::createPctrs(const AosRundataPtr &rdata)
{
	OmnString docstr = "<ctnr zky_objid=\"binarydoc\" zky_pctrs =\"vpd\" zky_public_doc=\"true\"";
		docstr << " zky_public_ctnr=\"true\" zky_otype=\"zky_ctnr\" zky_compress_alg=\"gzip\" />"; 
	AosXmlTagPtr dd = AosDocClient::getSelf()->createDocSafe1(rdata, docstr, "", "", true, false,
					false, false, false, true);
	OmnScreen << dd->toString() << endl;
}


AosXmlTagPtr
AosDocClientTester::createBinaryDoc(const AosRundataPtr &rdata)
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	if (mValue.length() <= 0)
		mValue = OmnRandom::lowcaseStr(900000, 1000000);
	//OmnScreen << "length : " << mValue.length() + mValue.length()<< endl;;
	buff->setOmnStr(mValue);
	buff->setOmnStr(mValue);

	OmnString docstr = "<vpd zky_a=\"1\" zky_binary_nodename = \"binary_data\" zky_otype=\"zky_binarydoc\" "; 
	docstr << "zky_public_ctnr=\"true\" zky_public_doc=\"true\" zky_pctrs=\"binarydoc\" >";
//	docstr << "<binary_data><![BDATA[" << buff->dataLen() << ":";
//	docstr.append(buff->data(), buff->dataLen());
//	docstr << "]]></binary_data></vpd>";
	docstr << "</vpd>";

	int vid = rand()%4;
	AosXmlTagPtr config_doc = AosDocClient::getSelf()->createBinaryDoc(docstr, true, buff, vid, rdata);
	aos_assert_r(config_doc, 0); 
	u64 did = config_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(AosGetVirtualIDByDocid(did) == vid, false);
	return config_doc;
}


AosXmlTagPtr
AosDocClientTester::retrieveBinaryDoc(const AosXmlTagPtr xml, const AosRundataPtr &rdata)
{
	OmnString objid = xml->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(objid != "", 0);
	AosBuffPtr buff;
	AosXmlTagPtr doc = AosDocClient::getSelf()->retrieveBinaryDoc(objid, buff, rdata);
	aos_assert_r(doc, false);
	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID, "") == objid, 0);
	aos_assert_r(buff && buff->dataLen() > 0, 0);
	buff->reset();
	OmnString str = buff->getStr("");
	aos_assert_r(str == mValue, 0);
	str = buff->getStr("");
	aos_assert_r(str == mValue, 0);
	return doc;
}

bool
AosDocClientTester::deleteBinaryDoc(const AosXmlTagPtr xml, const AosRundataPtr &rdata)
{
	OmnString objid = xml->getAttrStr(AOSTAG_OBJID, "");
	bool rslt = AosDocClient::getSelf()->deleteObj(rdata, "",  objid, "", false);
	aos_assert_r(rslt, false);
	AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByObjid(objid, rdata);
	aos_assert_r(!dd, false);
	AosBuffPtr buff;
	dd = AosDocClient::getSelf()->retrieveBinaryDoc(objid, buff, rdata);
	aos_assert_r(!dd, false);
	aos_assert_r(!buff, false);
	u64 docid = xml->getAttrU64(AOSTAG_DOCID, 0);
	rslt = AosDocClient::getSelf()->retrieveBinaryDoc(docid, buff, rdata);
	aos_assert_r(!rslt, false);
	aos_assert_r(!buff, false);
	return true;
}
