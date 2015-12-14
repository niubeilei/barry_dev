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
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "QueryTorturer/QueryTesterMgr.h"

//#include "SengTorturer/SengTesterMgr.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "QueryTorturer/QueryTesterThrd.h"
#include "SearchEngineAdmin/SengAdmin.h"
//#include "SengTorturer/SengTesterThrd.h"
//#include "SengTorturer/SengTester.h"
//#include "SengTorturer/SengTesterFileMgr.h"
//#include "SengTorturer/StUtil.h"
//#include "SengTorturer/TesterLogin.h"
#include "SecUtil/SecTypes.h"
//#include "SecUtil/AcctType.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "Porting/Sleep.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

const u32 sgSiteid = 100;



AosQueryTesterMgr::AosQueryTesterMgr(
		const int num_thrds, 
		const int num_tries)
:
mLock(OmnNew OmnMutex()),
//mFileMgr(OmnNew AosTestFileMgr()),
mNumThrds(num_thrds),
//mRootDocid(AOS_INVDID),
mNumTries(num_tries)
//mSpecialDocid(1)
{
	aos_assert(num_thrds > 0 && num_thrds < eMaxThrds);
	if (createSuperUser())
//	if(1)
	{	
		// Wait a little bit for the server to create the super user account.
		OmnWait::getSelf()->wait(100);
		mThrds = OmnNew AosQueryTesterThrdPtr[num_thrds];
	}
	else
	{
	 	throw -1;
	}
	aos_assert(loginAsRoot());
}


AosQueryTesterMgr::~AosQueryTesterMgr()
{
	delete [] mThrds;
}


bool
AosQueryTesterMgr::createSuperUser()
{
	OmnString requester_passwd = "als;kdlkqewrcvqwtiware346d";
	AosXmlTagPtr userdoc;
	bool rslt = AosSengAdmin::getSelf()->createSuperUser(requester_passwd, userdoc, sgSiteid);	
	aos_assert_r(rslt, false);
	aos_assert_r(userdoc, false);

	// Set mRootPasswd
	AosXmlTagPtr passwdnode = userdoc->getFirstChild("passwd");
	mRootPasswd = passwdnode->getNodeText();
	aos_assert_r(mRootPasswd != "", false);

	// Set mRootCtnr
	mRootCtnr = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(mRootCtnr != "", false);

	// Set mRooUsername
	mRootUsername = userdoc->getAttrStr(AOSTAG_USERNAME);
	aos_assert_r(mRootUsername != "", false);

	// Set mRootOBjid
	mRootObjid = userdoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(mRootObjid != "", false);

	// Set mRootDocid
	mRootDocid = userdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(mRootDocid != AOS_INVDID, false);

	// Set mRootHomeCtnr
	mRootHomeCtnr = userdoc->getAttrStr(AOSTAG_CTNR_HOME);
	aos_assert_r(mRootHomeCtnr != "", false);

	mRootCid = userdoc->getAttrStr(AOSTAG_CLOUDID);

	return true;
}

bool
AosQueryTesterMgr::loginAsRoot()
{
	if (mRootSsid!="") return true;
	OmnString ssid;
	AosXmlTagPtr userdoc;
	u64 userid;
	u64 urldocid;
	bool success = AosSengAdmin::getSelf()->login(
								mRootUsername, 
								mRootPasswd,
								mRootCtnr, 
								ssid, 
								urldocid,
								userid, 
								userdoc, 
								sgSiteid);
	aos_assert_r(success, false);
	aos_assert_r(ssid != "", false);
	aos_assert_r(userdoc, false);
	
	mRootSsid = ssid;
	mRootUrldocid = urldocid;
	mRootCid = userdoc->getAttrStr(AOSTAG_CLOUDID);
	mRootUserdoc = userdoc;
	return true;
}

bool 
AosQueryTesterMgr::start()
{
	OmnScreen << "    Start Tester ..."<<endl;
	AosQueryTesterMgrPtr thisptr(this, false);
	for (int i=0; i<mNumThrds; i++)
	{
		mThrds[i] = OmnNew AosQueryTesterThrd(thisptr, mNumTries, i);
		mThrds[i]->setSiteId(sgSiteid);
		mThrds[i]->setSsid(mRootSsid);
		mThrds[i]->setUrldocid(mRootUrldocid);
		mThrds[i]->start();		
	}
	return true;
}


/*
AosQueryTesterMgr::AosQueryTesterMgr(
		const int num_thrds, 
		const int num_tries)
:
mLock(OmnNew OmnMutex()),
mFileMgr(OmnNew AosTestFileMgr()),
mNumThrds(num_thrds),
mRootDocid(AOS_INVDID),
mNumTries(num_tries),
mSpecialDocid(1)
{
	aos_assert(num_thrds > 0 && num_thrds < eMaxThrds);
	if (createSuperUser())
	{	
		// Wait a little bit for the server to create the super user account.
		OmnWait::getSelf()->wait(100);
		mThrds = OmnNew AosSengTestThrdPtr[num_thrds];
	}
	else
	{
	 	throw -1;
	}
	aos_assert(loginAsRoot());
}


AosQueryTesterMgr::~AosQueryTesterMgr()
{
	delete [] mThrds;
}


bool
AosQueryTesterMgr::config(const AosXmlTagPtr &config)
{
	//AosSengTester::config(config);
	return true;
}



AosSengTestThrdPtr
AosQueryTesterMgr::getThread(int tid) const
{
	aos_assert_r(tid >= 0 && tid < AosTesterId::eMax, NULL);
	return mThrds[tid];
}


const AosTestFileMgrPtr
AosQueryTesterMgr::getFileMgr() const 
{
	return mFileMgr;
}




void
AosQueryTesterMgr::setRootSsid(const OmnString &ssid) 
{
	mRootSsid = ssid;
}


OmnString  
AosQueryTesterMgr::getRootCid()
{
	if (mRootDocid == AOS_INVDID) return "";
	OmnString ssid = mRootSsid;
	if (ssid == "") return "";
	if (mRootCid != "") return mRootCid;

	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByDocid(sgSiteid, ssid, mRootDocid);
	aos_assert_r(doc, "");
	OmnString cid = doc->getAttrStr(AOSTAG_CLOUDID);
	mRootCid = cid;
	return cid;
}


bool
AosQueryTesterMgr::canCreateUnderLostFound(const OmnString &cid)
{
	//if (cid == AOSCLOUDID_ROOT1) return true;
	return false;
}


AosXmlTagPtr
AosQueryTesterMgr::getAccessRcd(
		const OmnString &siteid, 
		const u64 &docid)
{
	return AosSengAdmin::getSelf()->getAccessRcd(siteid, docid, mRootSsid, false, false);
}




AosXmlTagPtr	
AosQueryTesterMgr::retrieveXmlDoc(const OmnString &siteid, const OmnString &objid)
{
	return AosSengAdmin::getSelf()->retrieveDocByObjid(
			siteid, mRootSsid, objid, true);
}


OmnString
AosQueryTesterMgr::getDftSiteid()
{
	return sgSiteid;
}

u64
AosQueryTesterMgr::getSpecialDocid()
{
	aos_assert_r(mSpecialDocid < 100, 0);
	return mSpecialDocid ++;
}

*/
#endif
