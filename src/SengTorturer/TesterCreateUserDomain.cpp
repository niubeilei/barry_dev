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
#include "SengTorturer/TesterCreateUserDomain.h"

#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEUtil/Ptrs.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include <stdlib.h>
#include "Random/RandomUtil.h"


AosCreateUserDomainTester::AosCreateUserDomainTester(const bool regflag)
:
AosSengTester(AosSengTester_CreateUserDomain, AosTesterId::eCreateUserDomain, regflag)
{
}


AosCreateUserDomainTester::AosCreateUserDomainTester()
:
AosSengTester(AosSengTester_CreateUserDomain, "createuserdomain", AosTesterId::eCreateUserDomain)
{
}


AosCreateUserDomainTester::~AosCreateUserDomainTester()
{
}


bool 
AosCreateUserDomainTester::test()
{
	// This function creates one user domain. Creating user domains is a 
	// previliged operation. Only root can do it. 
	AosRundataPtr rdata = mThread->getRundata(); 

	aos_assert_r(determineLogin(), false);
	aos_assert_r(createRawDoc(), false);
	aos_assert_r(pickParent(), false);
	aos_assert_r(determineDocPublic(), false);
	aos_assert_r(createUserDomain(rdata), false);
	return true;
}


bool 
AosCreateUserDomainTester::determineLogin()
{
	// 1. Determine whether to use root 
	if ((rand() % 100) <= mDoNotUseRootWeight)
	{
		// Not Come True
		bool rslt = mThread->loginAsNormalUser();	
		aos_assert_r(rslt, false);
		mAllowed = false;
	}
	else
	{
		bool rslt = mThread->loginAsRoot();
		aos_assert_r(rslt, false);
		mAllowed = true;
	}
	return true;
}


bool
AosCreateUserDomainTester::createRawDoc()
{
	mRawDoc= mThread->createDoc();
	aos_assert_r(mRawDoc, false);
	mRawDoc->setAttr(AOSTEST_DOCID, mThread->getNewDocId());
	mRawDoc->setAttr(AOSTAG_OTYPE, AOSOTYPE_USERDOMAIN);

	// Determine whether to have an objid or not
	if ((rand() % 100) < mWithObjid)
	{
		mObjid = mThread->pickNewObjid();
		aos_assert_r(mObjid != "", false);
		mRawDoc->setAttr(AOSTAG_OBJID, mObjid);
	}
	else
	{
		mObjid = "";
	}
	return true;
}


bool
AosCreateUserDomainTester::pickParent()
{
	// It randomly picks a parent.
	
	// 1. Determine whether to use a user domain. 
	mParentObjid = "";
	OmnString cid = mThread->getCrtCid();

	AosStContainerPtr container = mThread->pickContainer();
	if (!container)
	{
		// It picked a container that is not a user domain. 
		mParentObjid = "";
		return true;
	}

	mParentObjid = container->mObjid;
	aos_assert_r(mParentObjid !="", false);
	mRawDoc->setAttr(AOSTAG_PARENTC, mParentObjid);

	// Check whether the parent already had a CID. 
	OmnString prefix, ccid;
	bool rslt = AosObjid::decomposeObjid(mParentObjid, prefix, ccid);
	aos_assert_r(rslt, false);
	if (ccid != "")
	{
		// The container already had a cid
		return true;
	}

	// Determine the parent container objid
	if ((rand() % 100) < mCreatePubCtnrWeight)
	{
		mRawDoc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	}
	else
	{
		if (cid != "" && (rand() % 100) < mParentWithCidWeight)
		{
			// The parent should be with a cid
			mParentObjid = AosObjid::compose(mParentObjid, cid);
		}
	}

	mRawDoc->setAttr(AOSTAG_PARENTC, mParentObjid);
	return true;
}


bool
AosCreateUserDomainTester::determineDocPublic()
{
	// Determine whether user doc is public
	mDocIsPublic = false;
	if (rand() % 50) mDocIsPublic = true;
	if (mDocIsPublic)
	{
		mRawDoc->setAttr(AOSTAG_PUBLIC_DOC, "true");
	}
	return true;
}


bool
AosCreateUserDomainTester::createUserDomain(const AosRundataPtr &rdata)
{
	// 1. It creates the user
	// 2. Upon return, the response should contain the user being created. 
	//    Check the objid, making sure the user is not in the torturer yet.
	mResp = AosSengAdmin::getSelf()->createUserDomain(
						mRawDoc, 
						mThread->getCrtSsid(), 
						mThread->getUrlDocid(), 
						mThread->getSiteid(), 
						rdata);

	if (!mAllowed)
	{
		aos_assert_r(mResp, false);
		return true;
	}

	aos_assert_r(mResp, false);
	mResp = mResp->getFirstChild("root");
	u64 docid = mResp->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);
	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByDocid(
					mThread->getSiteid(), 
					mThread->getCrtSsid(), 
					mThread->getUrlDocid(), 
					docid);				
	aos_assert_r(mServerDoc, false);

	aos_assert_r(mRawDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
			mServerDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
	mObjid = mServerDoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(mObjid!="", false);
	aos_assert_r(!mThread->getDocByObjid(mObjid), false);

	return true;
}


bool
AosCreateUserDomainTester::checkCreation(const AosRundataPtr &rdata)
{
	aos_assert_r(checkParent(rdata), false);
	aos_assert_r(checkObjid(rdata), false);
	return true;
}


bool
AosCreateUserDomainTester::checkObjid(const AosRundataPtr &rdata)
{
	// User domain objid cannot have cloudid
	OmnString prefix, cid;
	bool rslt = AosObjid::decomposeObjid(mObjid, prefix, cid);
	aos_assert_r(rslt, false);
	aos_assert_r(cid == "", false);
	return true;
}


bool
AosCreateUserDomainTester::checkParent(const AosRundataPtr &rdata)
{
	// This function is called if the creation is (supposedly) 
	// successful. 
	//
	// All user domains are sub-containers of AOSCTNR_USERDOMAIN
	aos_assert_r(mServerDoc, false);
	OmnString parent = mServerDoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(parent == mParentObjid, false);
	aos_assert_r(mServerDoc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERDOMAIN, false); 

	AosStContainerPtr ct = mThread->getContainer(mParentObjid);
	aos_assert_r(ct, false);
	AosXmlTagPtr ctnr = mThread->retrieveDocByObjid(parent);
	aos_assert_r(ctnr, false);
	aos_assert_r(ct->mObjid == ctnr->getAttrStr(AOSTAG_OBJID), false);
	aos_assert_r(ctnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERDOMAIN, false);

	return true;
}


bool
AosCreateUserDomainTester::addContainer(const AosRundataPtr &rdata)
{
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mRawDoc, false);
	aos_assert_r(mThread, false);
	aos_assert_r(mThread->addContainer(AosStContainer1::eUserCtnr, mServerDoc, rdata), false);
	return true;
}


AosXmlTagPtr
AosCreateUserDomainTester::createUserDomain(
		const AosSengTestThrdPtr &thread,
		const AosSengTesterMgrPtr &mgr, 
		const AosRundataPtr &rdata)
{
	// This function creates a new user domain
	aos_assert_r(thread->loginAsRoot(), 0);
	AosXmlTagPtr rawdoc = thread->createDoc();
	aos_assert_r(rawdoc, 0);
	rawdoc->setAttr(AOSTEST_DOCID, thread->getNewDocId());
	rawdoc->setAttr(AOSTAG_OTYPE, AOSOTYPE_USERDOMAIN);
	rawdoc->setAttr(AOSTAG_PARENTC, AOSCTNR_USERDOMAIN);

	AosXmlTagPtr resp = AosSengAdmin::getSelf()->createUserDomain(
							rawdoc, 
							thread->getCrtSsid(), 
							thread->getUrlDocid(), 
							thread->getSiteid(), 
							rdata);

	aos_assert_r(resp, 0);

	aos_assert_r(resp, 0);
	u64 docid = resp->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, 0);
	AosXmlTagPtr user_domain = AosSengAdmin::getSelf()->retrieveDocByDocid(
								thread->getSiteid(), 
								thread->getCrtSsid(), 
								thread->getUrlDocid(), 
								docid);				
	aos_assert_r(user_domain, 0);
	aos_assert_r(thread->addContainer(AosStContainer1::eUserCtnr, user_domain, rdata), 0);
	return user_domain;
}

