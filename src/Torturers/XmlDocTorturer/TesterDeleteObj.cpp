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
#include "Torturers/XmlDocTorturer/TesterDeleteObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Torturers/XmlDocTorturer/TesterLogin.h"
#include "SengTorUtil/TesterXmlDoc.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/Ptrs.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StUtil.h"
#include "Torturers/XmlDocTorturer/XmlTagRand.h"
#include "Torturers/XmlDocTorturer/TesterCreateObj.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"

#include "Thrift/AosJimoAPI.h"

extern shared_ptr<AosJimoAPIClient> gThriftClient;

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
	aos_assert_r(deleteDoc(), false);

	//aos_assert_r(pickDoc(), false);	
	//aos_assert_r(checkDoc(), false);
	//aos_assert_r(deleteDoc(), false);
	//aos_assert_r(checkDelete(), false);
	
	return true;
}


bool
AosDeleteObjTester::pickDoc()
{
	/*
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
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosDeleteObjTester::checkDoc()
{
	/*
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
	*/
	OmnNotImplementedYet;
	return false;
}


/*
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
	OmnNotImplementedYet;
	return false;
}
*/


bool
AosDeleteObjTester::deleteDoc()
{
	JmoRundata _return;
	JmoCallData call_data = mMgr->getCallData();
	vector<string> Entries = AosCreateObjTester::mEntries;

	for(vector<string>::iterator iter = Entries.begin(); iter != Entries.end(); iter++)
	{
		string d = "";
		string doc_objid = d + *iter;
		aos_assert_r(doc_objid != "", false);
		gThriftClient->deleteDocByObjid(_return, call_data, doc_objid);
		aos_assert_r(_return.rcode == JmoReturnCode::SUCCESS, false);
	}
	
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
	/*
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
	*/
	OmnNotImplementedYet;
	return false;
}

