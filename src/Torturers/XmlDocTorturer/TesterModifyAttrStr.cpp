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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Torturers/XmlDocTorturer/TesterModifyAttrStr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "Tester/TestPkg.h"
#include "Torturers/XmlDocTorturer/Ptrs.h"
#include "Torturers/XmlDocTorturer/TesterLogin.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "Util/UtUtil.h"


AosModifyAttrStrTester::AosModifyAttrStrTester(const bool regflag)
:
AosSengTester(AosSengTester_ModifyAttrStr, AosTesterId::eModifyAttrStr, regflag),
mCanModify(false)
{
}


AosModifyAttrStrTester::AosModifyAttrStrTester()
:
AosSengTester(AosSengTester_ModifyAttrStr, "mod_obj", AosTesterId::eModifyAttrStr),
mCanModify(false)
{
}


AosModifyAttrStrTester::~AosModifyAttrStrTester()
{
}


bool 
AosModifyAttrStrTester::test()
{
	//AosRundataPtr rdata = OmnNew AosRundata();
	aos_assert_r(pickDoc(), false);
	aos_assert_r(randPickModifyAttr(), false);
	aos_assert_r(sendModfiyAttr(), false);
	aos_assert_r(checkModify(), false);
	aos_assert_r(modifyDocToMemory(), false);
	return true;
}


bool
AosModifyAttrStrTester::pickDoc()
{

/*
	mRawDoc = 0;
	mLocalDocid = 0;
	mServerDocid = 0;	
	mServerDoc = 0;
	int i = 1;
	int total = mThread->getStDocSize();
	while (i++ < total)
	{
		AosStDocPtr doc = mThread->pickDoc();
		if (!doc) continue;
		mObjid = doc->objid;
		mLocalDocid = doc->locdid;
		mServerDocid = doc->svrdid;
		AosStContainerPtr container = mThread->getContainer(mLocalDocid);
		AosStUser *user = mThread->getUser(mLocalDocid);
		const AosStAccessRecord *ar = mThread->getLocalAccessRecord(mLocalDocid);
		if (!container && !user && !ar)
		{
			//Not allow delete container and user
			break;
		}
	}

	if (i >= total) return true;
	mRawDoc = mThread->readLocalDocFromFile(mLocalDocid);
	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
			mThread->getSiteid(), mThread->getCrtSsid(),mServerDocid, false, mThread->getUrlDocid());
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mRawDoc, false);
	aos_assert_r(mLocalDocid == mServerDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
	aos_assert_r(mServerDoc->isDocSame(mRawDoc), false);
	return true;
*/
	OmnNotImplementedYet;
	return false;
}

bool
AosModifyAttrStrTester::randPickModifyAttr()
{
	if (!mRawDoc) return true;
	aos_assert_r(mServerDoc, false);
	mSpecialAttr = false;
	AosXmlParser parser;
	AosXmlTagPtr doc =  parser.parse(mServerDoc->toString(), "" AosMemoryCheckerArgs);
	mAttrName = "", mAttrValue = ""; 
	if (rand()%2 < 90)
	{
		if (rand()%2 <50)
		{	
			AosXmlTagRand tagrand;
			tagrand.addAttr(doc, mAttrName, mAttrValue);	
		}
		else
		{
			OmnString oldvalue;
			AosXmlTagRand tagrand;
			tagrand.randRootAttr(doc, mAttrName, oldvalue);
			mAttrValue = AosGeneratePasswd();
			aos_assert_r(oldvalue != "", false);
		}
		aos_assert_r(mAttrName != "", false);
		aos_assert_r(mAttrValue != "", false);
		return true;
	}
	//modify special attr
	//Following attributes cannot be modified
	//AOSTAG_VERSION 
	//AOSTAG_SITEID 
	//AOSTAG_OBJID
	
	mSpecialAttr = true;
	int i = rand() % 3;
	if (i == 0) mAttrName = AOSTAG_VERSION;
	if (i == 1) mAttrName = AOSTAG_SITEID;
	if (i == 2) mAttrName = AOSTAG_OBJID;
	//mOldValue = doc->getAttrStr(mAttrName, "");
	mAttrValue = AosGeneratePasswd();
	return true;
}

bool
AosModifyAttrStrTester::sendModfiyAttr()
{
/*
	if (!mRawDoc) return true;
	aos_assert_r(mRawDoc, false);
	bool rslt = AosSengAdmin::getSelf()->modifyAttr(
			mThread->getSiteid(), 
			mThread->getCrtSsid(), 
			mThread->getUrlDocid(),
			mObjid, 
			mAttrName, 
			mAttrValue,
			"", 
			"false", 
			"true"
			);
	mCanModify = canModify();
	if (!mCanModify)
	{
		aos_assert_r(!rslt, false);
		return true;
	}
	//retrieve by docid
	aos_assert_r(rslt, false);
	return true;

*/
	OmnNotImplementedYet;
	return false;
}

bool
AosModifyAttrStrTester::canModify()
{
	if (mThread->getCrtSsid() == "") return false;
	if (mSpecialAttr) return false;
	if (mThread->isRoot()) return true;
	return true;
	//check the access
	//OmnString cid = mThread->getCrtCid();
	//return (mThread->canDoOpr(mServerDoc->getAttrU64(AOSTEST_DOCID, 0),
	//			AosSecOpr::eWrite, cid));
}

bool
AosModifyAttrStrTester::checkModify()
{
/*
	if (!mRawDoc) return true;
	if (!mCanModify) return true;
	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
			mThread->getSiteid(), mThread->getCrtSsid(),mServerDocid, false, mThread->getUrlDocid());
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mObjid == mServerDoc->getAttrStr(AOSTAG_OBJID, ""), false);
	aos_assert_r(mServerDoc->getAttrStr(mAttrName, "") == mAttrValue, false);
	return true;
*/
	OmnNotImplementedYet;
	return false;
}

bool
AosModifyAttrStrTester::modifyDocToMemory()
{
	if (!mRawDoc) return true;
	if (!mCanModify)return true;
	aos_assert_r(mLocalDocid, false);
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mThread->modifyDocToMemory(mLocalDocid, mServerDoc), false);
	return true;
}

