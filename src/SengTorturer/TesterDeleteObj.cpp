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
#include "SengTorturer/TesterDeleteObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SengTorturer/TesterLogin.h"
#include "SengTorturer/SengTesterXmlDoc.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/StUtil.h"
#include "SengTorturer/XmlTagRand.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"


AosDeleteObjTester::AosDeleteObjTester(const bool regflag)
:
AosSengTester(AosSengTester_DeleteObj, AosTesterId::eDeleteObj, regflag),
mUseKnownCtnrWeight(50),
mUseDuplicatedCtnrWeight(50),
mUsePubCtnrWeight(50),
mPrivateDocWeight(50),
mUseUniqueObjidWeight(50),
mCreateDocWeight(50),
mWithCidWeight(50),
mCanDelete(false)
{
}


AosDeleteObjTester::AosDeleteObjTester()
:
AosSengTester(AosSengTester_DeleteObj, "del_obj", AosTesterId::eDeleteObj),
mUseKnownCtnrWeight(50),
mUseDuplicatedCtnrWeight(50),
mUsePubCtnrWeight(50),
mPrivateDocWeight(50),
mUseUniqueObjidWeight(50),
mCreateDocWeight(50),
mWithCidWeight(50),
mCanDelete(false)
{
}


AosDeleteObjTester::~AosDeleteObjTester()
{
}


bool 
AosDeleteObjTester::test()
{
	aos_assert_r(pickDoc(), false);	
	aos_assert_r(checkDoc(), false);
	aos_assert_r(deleteDoc(), false);
	aos_assert_r(checkDelete(), false);
	return true;
}


bool
AosDeleteObjTester::pickDoc()
{
	mRawDoc = 0;
	mObjid = "";
	mLocalDocid = 0;
	mServerDocid = 0;
	int i = 1;
	int total = mThread->getStDocSize();
	AosStDocPtr doc;
	while(i++ <total)
	{
		doc = mThread->pickDoc();
		if (!doc) continue;
		mObjid = doc->objid;
		mLocalDocid = doc->locdid;
		mServerDocid = doc->svrdid;

		AosStContainerPtr container = mThread->getContainer(mLocalDocid);
		AosStUser *user = mThread->getUser(mLocalDocid);
		AosStAccessRecord *ar = mThread->getLocalAccessRecord(mLocalDocid);
		if (!container && !user && !ar)
		{	
			//Not allow delete container and user
			break;
		}
	}	
	if (i >= total) return true;
AosXmlTagPtr dd = AosSengAdmin::getSelf()->retrieveDocByDocid(
		        mThread->getSiteid(), mThread->getCrtSsid(), 
				mThread->getUrlDocid(), mServerDocid,  false);
	mRawDoc = mThread->readLocalDocFromFile(mLocalDocid);
	aos_assert_r(mRawDoc, false);
	return true;
}


bool
AosDeleteObjTester::checkDoc()
{
	if (!mRawDoc) return true;
	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
			mThread->getSiteid(), mThread->getCrtSsid(),
			mThread->getUrlDocid(), mServerDocid, false);
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mServerDoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_CONTAINER, false);
	aos_assert_r(mServerDoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_USERACCT, false);
	aos_assert_r(mServerDoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_ACCESS_RCD, false);
	aos_assert_r(mServerDoc->getAttrStr(AOSTAG_OTYPE) != AOSTAG_CONTAINER, false);
	aos_assert_r(mRawDoc, false);
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mServerDoc->isDocSame(mRawDoc), false);
	aos_assert_r(mServerDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID) == mLocalDocid, false);
	aos_assert_r(mServerDoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == mServerDocid, false);
	return true;
}


bool
AosDeleteObjTester::deleteDoc()
{
	if (!mRawDoc) return true;
	OmnString docid;
	docid << mServerDocid;
	aos_assert_r(docid!="", false);
	mCanDelete = determineCanDelete();
	bool rslt = AosSengAdmin::getSelf()->removeDocFromServer(
						mThread->getSiteid(), 
						mThread->getCrtSsid(), 
						mThread->getUrlDocid(),
						"", 
						mServerDocid);
	if (!mCanDelete)
	{
		aos_assert_r(!rslt, false);
		return true;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDeleteObjTester::determineCanDelete()
{
	if (mThread->getCrtSsid() == "") return false;
	//check the access
	if (mThread->isRoot()) return true;
	OmnString cid = mThread->getCrtCid();
	return (mThread->canDoOpr(mServerDoc->getAttrU64(AOSTEST_DOCID, 0), AosSecOpr::eDelete));
}


bool
AosDeleteObjTester::checkDelete()
{
	if (!mRawDoc) return true;
	if (!mCanDelete) return true;
	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
			mThread->getSiteid(), mThread->getCrtSsid(), 
			mThread->getUrlDocid(), mServerDocid, true);
	aos_assert_r(!mServerDoc, false);
	aos_assert_r(mLocalDocid, false);
//OmnScreen << "local docid :" << mLocalDocid << endl;
	aos_assert_r(mThread->removeDocToFile(mLocalDocid), false);
	aos_assert_r(mThread->isDeleteToFile(mLocalDocid),false);
	aos_assert_r(mThread->removeDoc(mLocalDocid, mObjid, mServerDocid), false);
	//aos_assert_r(!mThread->pickDoc(mLocalDocid), false);
	aos_assert_r(!mThread->getDocByObjid(mObjid), false);
	OmnString server_docid;
	server_docid << mServerDocid;
	aos_assert_r(mThread->getServeridByLocalid(server_docid) ==0, false);
	return true;

}

