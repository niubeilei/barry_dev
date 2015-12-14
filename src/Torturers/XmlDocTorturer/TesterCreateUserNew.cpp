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
#include "Torturers/XmlDocTorturer/TesterCreateUserNew.h"

#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/Ptrs.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include <stdlib.h>
#include "Random/RandomUtil.h"

#define WeightTag 90

AosCreateUserNewTester::AosCreateUserNewTester(const bool regflag)
:
AosSengTester(AosSengTester_CreateUserNew, AosTesterId::eCreateUserNew, regflag),
mUserParentCtnrWeight(50),
mCreatePubCtnrWeight(50),
mWithCidWeight(50),
mParentWithCidWeight(50),
mGroupId(0),
mLocalDocid(0),
mParent_is_public(true),
mParent_with_cid(true),
mThreadId(0),
mWithUserDomain(false),
mWithObjid(false),
mRandomContainerWeight(50),
mUserIsPublic(false)
{
}


AosCreateUserNewTester::AosCreateUserNewTester()
:
AosSengTester(AosSengTester_CreateUserNew, "user", AosTesterId::eCreateUserNew),
mUserParentCtnrWeight(50),
mCreatePubCtnrWeight(50),
mWithCidWeight(50),
mParentWithCidWeight(50),
mGroupId(0),
mLocalDocid(0),
mParent_is_public(true),
mParent_with_cid(true),
mThreadId(0),
mWithUserDomain(false),
mWithObjid(false),
mRandomContainerWeight(50),
mUserIsPublic(false)
{
}


AosCreateUserNewTester::~AosCreateUserNewTester()
{
}


bool 
AosCreateUserNewTester::test()
{
	// Tank 2011/06/09
	createUserAccount1(mThread);
	return true;
	AosRundataPtr rdata = mThread->getRundata();
	mResp = 0;
	mLocalDocid = mThread->getNewDocId();

	bool allowed;
	aos_assert_r(createRawUserDoc(), false);
	aos_assert_r(pickParents(), false);
	aos_assert_r(determinePublic(), false);
	aos_assert_r(createUser(), false);
	aos_assert_r(addUserGroup(), false);
	aos_assert_r(checkPermission(allowed), false);
	if (!allowed) return true;
	aos_assert_r(mResp, false);
	aos_assert_r(checkCreation(rdata), false);
	//aos_assert_r(checkIsLogin(), false);
	aos_assert_r(addUser(), false);
	return true;
}


bool
AosCreateUserNewTester::createRawUserDoc()
{
	mRawUser = mThread->createDoc();
	aos_assert_r(mRawUser, false);
	mRawUser->setAttr(AOSTEST_DOCID, mLocalDocid);
	mRawUser->setAttr(AOSTAG_OTYPE, AOSOTYPE_USERACCT);
	mPasswd = AosGeneratePasswd();
	mRawUser->setNodeText(AOSTAG_PASSWD, mPasswd, true);

	// Create User Name
	mUsername = mThread->getNewUserName();
	aos_assert_r(mUsername !="", false);
	mRawUser->setAttr(AOSTAG_USERNAME, mUsername);

	// Determine whether to have an objid or not
	if ((rand() % 100) < mWithObjid)
	{
		mObjid = mThread->pickNewObjid();
		aos_assert_r(mObjid != "", false);
		mRawUser->setAttr(AOSTAG_OBJID, mObjid);
	}
	else
	{
		mObjid = "";
	}

// OmnString email = "454630293@qq.com"; 
// mRawUser->setAttr(AOSTAG_EMAIL, email);
	
	return true;
}


bool
AosCreateUserNewTester::pickParents()
{
	// It randomly picks a user domain. It may or may not be able to 
	// pick. If not, creating user account should fail.
	
	// 1. Determine whether to use a user domain. 
	mParentObjid = "";
	if (rand() % 100 > 50)
	{
		// Do not use user domain
		mWithUserDomain = false;
		return true;
	}

	OmnString cid = mThread->getCrtCid();
	AosStContainerPtr container;

	
	if ((rand() % 100) < mRandomContainerWeight)
	{
		container = mThread->pickContainer();
		if (container && container->mType != AosStContainer1::eUserCtnr)
		{
			// It picked a container that is not a user domain. 
			mWithUserDomain = false;
			mParentObjid = container->mObjid;
			return true;
		}
	}

	AosStContainer1::Type type = AosStContainer1::eUserCtnr;
	container = mThread->pickContainer(type);
	if (!container)
	{
		// No user domain found.
		mWithUserDomain = false;
		return true;
	}

	aos_assert_r(container->mType == type, false);
	mParentObjid = container->mObjid;
	aos_assert_r(mParentObjid !="", false);
	mRawUser->setAttr(AOSTAG_PARENTC, mParentObjid);

	// Need to check whether the parent already had a CID. 
	OmnString prefix, ccid;
	bool rslt = AosObjid::decomposeObjid(mParentObjid, prefix, ccid);
	aos_assert_r(rslt, false);
	if (ccid != "")
	{
		// The container already had a cid
		mParent_is_public = false;
		mParent_with_cid = true;
		return true;
	}

	// Determine the parent container objid
	mParent_is_public = false;
	mParent_with_cid = false;
	mWithUserDomain = true;
	if ((rand() % 100) < mCreatePubCtnrWeight)
	{
		mParent_is_public = true;
		mRawUser->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	}
	else
	{
		mParent_is_public = false;
		mRawUser->setAttr(AOSTAG_CTNR_PUBLIC, "false");
		if (cid != "" && mParentObjid != "" && 
			(rand() % 100) < mParentWithCidWeight)
		{
			// The parent should be with a cid
			mParentObjid = AosObjid::compose(mParentObjid, cid);

			// Check whether the container already exists
			AosStContainerPtr container = mThread->getContainer(mParentObjid);
			if (!container || container->mType != AosStContainer1::eUserCtnr)
			{
				mWithUserDomain = false;
			}
			mParent_with_cid = true;
		}
	}

	return true;
}


bool
AosCreateUserNewTester::determinePublic()
{

	// Determine whether user doc is public
	mUserIsPublic = false;
	if (rand() % 50) mUserIsPublic = true;
	if (mUserIsPublic)
	{
		mRawUser->setAttr(AOSTAG_PUBLIC_DOC, "true");
	}

	return true;
}


bool
AosCreateUserNewTester::createUser()
{
	/*
	// 1. It creates the user
	// 2. Upon return, the response should contain the user being created. 
	//    Check the objid, making sure the user is not in the torturer yet.
	mResp = AosSengAdmin::getSelf()->createUserAcct(
					mRawUser, 
					mThread->getCrtSsid(),
					mThread->getCrtUrlDocid(), 
					mThread->getSiteid());

	if (mResp)
	{
		mResp = mResp->getFirstChild("accounts");
		aos_assert_r(mResp, false);
		mResp = mResp->getFirstChild("account");
		aos_assert_r(mResp, false);
		u64 docid = mResp->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(docid, false);
		mServerUser = AosSengAdmin::getSelf()->retrieveDocByDocid(
			mThread->getSiteid(), mThread->getCrtSsid(), docid, false, mThread->getUrlDocid());	
		aos_assert_r(mServerUser, false);
		aos_assert_r(mRawUser->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
				mServerUser->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
		mObjid = mServerUser->getAttrStr(AOSTAG_OBJID);
		aos_assert_r(mObjid!="", false);
		aos_assert_r(!mThread->getDocByObjid(mObjid), false);
	}

	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool 
AosCreateUserNewTester::addUserGroup()
{
	mUserGroup = "";
	for (int i = 0; i < rand()%3 ;i++)
	{
		if (mGroupId == 0) mGroupId = 1;
		if (rand()%100>95)
		{
			OmnString group;
			group << "sengto_" << mThread->getThreadId() << "_" << rand()%mGroupId; 
			mUserGroup <<  group << ",";
		}
		else
		{
			OmnString group;
			group << "sengto_" << mThread->getThreadId() << mGroupId++;
			mUserGroup << group << "," ;	
		}
	}
	mUserGroup.setLength(mUserGroup.length()-1);
	mRawUser->setAttr(AOSOTYPE_GROUP, mUserGroup);
	return true;
}


bool
AosCreateUserNewTester::checkCreation(const AosRundataPtr &rdata)
{
	aos_assert_r(checkParents(rdata), false);
	aos_assert_r(checkObjid(rdata), false);
	aos_assert_r(mUserGroup == mResp->getAttrStr(AOSOTYPE_GROUP,""), false);
	return true;
}


bool
AosCreateUserNewTester::checkObjid(const AosRundataPtr &rdata)
{
	OmnString prefix, cid;
	bool rslt = AosObjid::decomposeObjid(mObjid, prefix, cid);
	if (mUserIsPublic)
	{
		aos_assert_r(rslt, false);
		aos_assert_r(cid == "", false);
		return true;
	}

	aos_assert_r(cid != "", false);
	return true;
}


bool
AosCreateUserNewTester::checkPermission(bool &allowed)
{
	/*
	// It checks whether the container allows users to create new 
	// members. It is allowed if the Add Member permission is set
	// to this user. 
	allowed = false;
	if (!mWithUserDomain)
	{
		// Creation should fail.
		aos_assert_r(!mResp, false);
		return true;
	}

	// If the parent does not exist locally, the creation should
	// fail.
	AosStDocPtr local_parent = mThread->getDocByObjid(mParentObjid);
	if (!local_parent)
	{
		aos_assert_r(!mResp, false);
		return true;
	}

	// Retrieve the parent container's access record
	AosStAccessRecord* acrd = 0;// = mThread->getLocalAccessRecord(mParentObjid);
	if (!acrd)
	{
		// The parent does not have an access record. By default, 
		// creating user accounts is not allowed.
		aos_assert_r(!mResp, false);
		return true;
	}

	OmnString crt_cid = mThread->getCrtCid();
	OmnString owner_cid = mThread->getUserCid(acrd->mOwnLocdid);
	if (!acrd->canDoOpr(AosSecOpr::eCreateAcct, owner_cid, crt_cid))
	{
		// Access is denied.
		aos_assert_r(!mResp, false);
		return true;
	}

	// New user accounts can be craeted by the current user.
	allowed = true;
	aos_assert_r(mResp, false);
	mResp = mResp->getFirstChild("accounts");
	aos_assert_r(mResp, false);
	mResp = mResp->getFirstChild("account");
	aos_assert_r(mResp, false);
	u64 docid = mResp->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	// Retrieve the user doc
	mServerUser = AosSengAdmin::getSelf()->retrieveDocByDocid(
		mThread->getSiteid(), mThread->getCrtSsid(), docid, false, mThread->getUrlDocid());
	aos_assert_r(mServerUser, false);

	// The local docid should be the same
	aos_assert_r(mRawUser->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
			mServerUser->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);

	// The objid should not exist locally yet.
	mObjid = mServerUser->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(mObjid!="", false);
	aos_assert_r(!mThread->getDocByObjid(mObjid), false);

	// Check Username
	OmnString username = mServerUser->getAttrStr(AOSTAG_USERNAME);
	aos_assert_r(username != "" && username == mUsername, false);

	// Check password
	OmnString passwd = mServerUser->getAttrStr(AOSTAG_PASSWD);
	aos_assert_r(passwd != "" && passwd == mPasswd, false);

	// Check cid
	OmnString cid = mServerUser->getAttrStr(AOSTAG_CLOUDID);
	aos_assert_r(cid != "", false);
	OmnString rr;
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByCloudid(
		mThread->getSiteid(), cid, mThread->getUrlDocid());
	//AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByCloudid(
	//	mThread->getSiteid(), mThread->getCrtSsid(), cid, rr);
	aos_assert_r(doc, false);
	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == 
			mServerUser->getAttrU64(AOSTAG_DOCID, 0), false);
	aos_assert_r(doc->getAttrU64(AOSTAG_OBJID, 0) == 
			mServerUser->getAttrU64(AOSTAG_OBJID, 0), false);
	aos_assert_r(doc->getAttrU64(AOSTAG_CLOUDID, 0) == 
			mServerUser->getAttrU64(AOSTAG_CLOUDID, 0), false);

	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosCreateUserNewTester::checkParents(const AosRundataPtr &rdata)
{
	// This function is called if the creation is (supposedly) 
	// successful. 
	aos_assert_r(mServerUser, false);
	OmnString parent = mServerUser->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(parent != "", false);
	aos_assert_r(parent == mParentObjid, false);

	AosStContainerPtr ct = mThread->getContainer(mParentObjid);
	aos_assert_r(ct, false);
	AosXmlTagPtr ctnr = mThread->retrieveDocByObjid(parent);
	aos_assert_r(ctnr, false);
	aos_assert_r(ct->mObjid == ctnr->getAttrStr(AOSTAG_OBJID), false);
	aos_assert_r(ctnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERDOMAIN, false);

	// The container should have been created. The following is invalid. 
	// Chen Ding, 06/08/2011
	// u64 ldid = mThread->JudgeLocalDocid(ctnr);
	// ctnr->setAttr(AOSTEST_DOCID, ldid);
	// aos_assert_r(ctnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, false);
	// AosStContainer1::Type type =  AosStContainer1::eUserCtnr;;
	// aos_assert_r(mThread->addContainer(type, ctnr, rdata), false);
	// bool rslt = AosSengAdmin::getSelf()->sendModifyReq(
	// 		mThread->getSiteid(),mThread->getCrtSsid(), 
	// 		(char *)ctnr->getData(), rdata, false, 
	// 		mThread->getUrlDocid());
	// aos_assert_r(rslt, false);
	return true;
}


bool
AosCreateUserNewTester::addUser()
{
	aos_assert_r(mServerUser, false);
	aos_assert_r(mRawUser, false);
	aos_assert_r(mThread, false);
	u64 locdid = mServerUser->getAttrU64(AOSTEST_DOCID, 0);
	aos_assert_r(locdid == mLocalDocid, false);
	aos_assert_r(mThread->addUser(mLocalDocid, mServerUser), false);
	return true;
}


bool
AosCreateUserNewTester::checkLogin()
{
	/*
	// This function uses the newly created user account to login.
	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	OmnString usename = mServerUser->getAttrStr(AOSTAG_USERNAME);
	OmnString passwd = mServerUser->getAttrStr(AOSTAG_PASSWD);
	OmnString parent_objid = mServerUser->getAttrStr(AOSTAG_PARENTC);
	u32 siteid = mServerUser->getAttrU32(AOSTAG_SITEID, 0);
	OmnString cid = mServerUser->getAttrStr(AOSTAG_CLOUDID);
	aos_assert_r(cid!="", false);
	u64 urldocid;
	bool success = AosSengAdmin::getSelf()->login(
						usename, 
						passwd,	
						parent_objid, 
						ssid, 
						urldocid, 
						userid, 
						userdoc, 
						siteid, 
						cid);
	aos_assert_r(success, false);
	aos_assert_r(ssid != "", false);

	// User an incorrect password to login
	OmnString pp = AosGeneratePasswd();
	if (pp != passwd)
	{
		success = AosSengAdmin::getSelf()->login(
						usename, 
						pp,	
						parent_objid, 
						ssid, 
						urldocid, 
						userid, 
						userdoc, 
						siteid, 
						cid);
		aos_assert_r(!success, false);
		aos_assert_r(ssid == "", false);
	}

	success = AosSengAdmin::getSelf()->login(
						usename, 
						passwd,	
						parent_objid, 
						ssid, 
						urldocid, 
						userid, 
						userdoc, 
						siteid, 
						cid);
	aos_assert_r(!success, false);
	aos_assert_r(ssid == "", false);
	return success;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosCreateUserNewTester::createUserAccount(
		const AosSengTestThrdPtr &thread)
{
	/*
	thread->loginAsRoot();

	AosXmlTagPtr raw_doc = thread->createDoc();
	aos_assert_r(raw_doc, false);
	raw_doc->setAttr(AOSTEST_DOCID, thread->getNewDocId());
	raw_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_USERACCT);
	OmnString password = AosGeneratePasswd();
	raw_doc->setNodeText(AOSTAG_PASSWD, password, true);

	// Create User Name
	OmnString username = thread->getNewUserName();
	aos_assert_r(username !="", false);
	raw_doc->setAttr(AOSTAG_USERNAME, username);

	OmnString objid = thread->pickNewObjid();
	aos_assert_r(objid != "", false);
	raw_doc->setAttr(AOSTAG_OBJID, objid);

	OmnString cid = thread->getCrtCid();
	AosStContainer1::Type type = AosStContainer1::eUserCtnr;
	AosStContainerPtr container = thread->pickContainer(type);
	aos_assert_r(container, false);
	aos_assert_r(container->mType == type, false);
	OmnString parent_objid = container->mObjid;
	aos_assert_r(parent_objid !="", false);
	raw_doc->setAttr(AOSTAG_PARENTC, parent_objid);

	AosXmlTagPtr resp = AosSengAdmin::getSelf()->createUserAcct(
								raw_doc, 
								thread->getCrtSsid(), 
								thread->getUrlDocid(), 
								thread->getSiteid());
	aos_assert_r(resp, false);
	
	resp = resp->getFirstChild("accounts");
	aos_assert_r(resp, false);
	resp = resp->getFirstChild("account");
	aos_assert_r(resp, false);
	u64 docid = resp->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);
	AosXmlTagPtr server_doc = AosSengAdmin::getSelf()->retrieveDocByDocid(
									thread->getSiteid(), 
									thread->getCrtSsid(), 
									thread->getUrlDocid(), 
									docid, 
									false);
	aos_assert_r(server_doc, false);
	aos_assert_r(raw_doc->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
				server_doc->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
	objid = server_doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid !="", false);

	aos_assert_r(thread->addUser(server_doc->getAttrU64(AOSTEST_DOCID, 0), server_doc), false);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosCreateUserNewTester::createUserAccount1(
		const AosSengTestThrdPtr &thread)
{
	/*
//	thread->loginAsRoot();

	// Create Doc
	AosXmlTagPtr raw_doc = thread->createDoc();
	aos_assert_r(raw_doc, false);

	raw_doc->setAttr(AOSTEST_DOCID, thread->getNewDocId());
	raw_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_USERACCT);
	OmnString password = AosGeneratePasswd();
	raw_doc->setNodeText(AOSTAG_PASSWD, password, true);

	// Create User Name
	OmnString username = thread->getNewUserName();
	aos_assert_r(username !="", false);
	raw_doc->setAttr(AOSTAG_USERNAME, username);

	OmnString objid = thread->pickNewObjid();
	aos_assert_r(objid != "", false);
	raw_doc->setAttr(AOSTAG_OBJID, objid);

	OmnString cid = thread->getCrtCid();
	AosStContainer1::Type type = AosStContainer1::eUserCtnr;
	AosStContainerPtr container = thread->pickContainer(type);
	aos_assert_r(container, false);
	aos_assert_r(container->mType == type, false);
	OmnString parent_objid = container->mObjid;
	aos_assert_r(parent_objid !="", false);
	raw_doc->setAttr(AOSTAG_PARENTC, parent_objid);

	AosXmlTagPtr resp = AosSengAdmin::getSelf()->createUserAcct(
								raw_doc, 
								thread->getCrtSsid(), 
								thread->getUrlDocid(), 
								thread->getSiteid());
	aos_assert_r(resp, false);
	
	resp = resp->getFirstChild("accounts");
	aos_assert_r(resp, false);
	resp = resp->getFirstChild("account");
	aos_assert_r(resp, false);
	
	u64 docid = resp->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);
	AosXmlTagPtr server_doc = AosSengAdmin::getSelf()->retrieveDocByDocid(
									thread->getSiteid(), 
									thread->getCrtSsid(), 
									thread->getUrlDocid(), 
									docid, 
									false);
	aos_assert_r(server_doc, false);
	
	aos_assert_r(raw_doc->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
				server_doc->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
	
	objid = server_doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid !="", false);

	// Check Not Add AccessRecord
	AosStAccessRecord* acrd = 0;//= thread->getLocalAccessRecord(parent_objid);
	if (!acrd)
	{
		aos_assert_r(!resp, false);
		return true;
	}
	
	OmnString crt_cid = thread->getCrtCid();
	OmnString owner_cid = thread->getUserCid(acrd->mOwnLocdid);
	if (!acrd->canDoOpr(AosSecOpr::eCreateAcct, owner_cid, crt_cid))
	{
		// Access is denied.
		aos_assert_r(!resp, false);
		return true;
	}

	aos_assert_r(!thread->getDocByObjid(objid), false);

	// Check Username
	OmnString username1 = server_doc->getAttrStr(AOSTAG_USERNAME);
	aos_assert_r(username1 != "" && username1 == username, false);

	// Check password
	//OmnString passwd = server_doc->getAttrStr(AOSTAG_PASSWD);
	OmnString passwd = server_doc->getNodeText(AOSTAG_PASSWD);
	aos_assert_r(passwd != "" && passwd == password, false);

	// Check cid
	OmnString cid1 = server_doc->getAttrStr(AOSTAG_CLOUDID);
	aos_assert_r(cid1 != "", false);
	OmnString rr;
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByCloudid(
							thread->getSiteid(), 
							cid1, 
							thread->getUrlDocid());
	//AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByCloudid(  // retrieveDocByCloudid Has Not Come Ture!
	//	thread->getSiteid(), thread->getCrtSsid(), cid1, rr);
	aos_assert_r(doc, false);
	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == 
			server_doc->getAttrU64(AOSTAG_DOCID, 0), false);
	aos_assert_r(doc->getAttrU64(AOSTAG_OBJID, 0) == 
			server_doc->getAttrU64(AOSTAG_OBJID, 0), false);
	aos_assert_r(doc->getAttrU64(AOSTAG_CLOUDID, 0) == 
			server_doc->getAttrU64(AOSTAG_CLOUDID, 0), false);

	aos_assert_r(thread->addUser(server_doc->getAttrU64(AOSTEST_DOCID, 0), server_doc), false);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}

