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
#include "SengTorturer/SengTesterMgr.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTester.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/StUtil.h"
#include "SengTorturer/TesterLogin.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
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

AosSengTesterMgr::AosSengTesterMgr(
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
	//aos_assert(loginAsRoot());
a:
	if (!loginAsRoot())
	{
		goto a;
	}
}


AosSengTesterMgr::~AosSengTesterMgr()
{
	delete [] mThrds;
}


bool
AosSengTesterMgr::config(const AosXmlTagPtr &config)
{
	AosSengTester::config(config);
	return true;
}


bool 
AosSengTesterMgr::start()
{
	OmnScreen << "    Start Tester ..."<<endl;
	AosSengTesterMgrPtr thisptr(this, false);
	for (int i=0; i<mNumThrds; i++)
	{
		mThrds[i] = OmnNew AosSengTestThrd(thisptr, mNumTries, i);
	}
	return true;
}


AosSengTestThrdPtr
AosSengTesterMgr::getThread(int tid) const
{
	aos_assert_r(tid >= 0 && tid < AosTesterId::eMax, NULL);
	return mThrds[tid];
}


const AosTestFileMgrPtr
AosSengTesterMgr::getFileMgr() const 
{
	return mFileMgr;
}


bool
AosSengTesterMgr::createSuperUser()
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


void
AosSengTesterMgr::setRootSsid(const OmnString &ssid) 
{
	mRootSsid = ssid;
}


OmnString  
AosSengTesterMgr::getRootCid()
{
	if (mRootDocid == AOS_INVDID) return "";
	OmnString ssid = mRootSsid;
	if (ssid == "") return "";
	if (mRootCid != "") return mRootCid;

	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByDocid(sgSiteid, ssid, mRootDocid, false, mRootUrlDocid);
	aos_assert_r(doc, "");
	OmnString cid = doc->getAttrStr(AOSTAG_CLOUDID);
	mRootCid = cid;
	return cid;
}


bool
AosSengTesterMgr::canCreateUnderLostFound(const OmnString &cid)
{
	//if (cid == AOSCLOUDID_ROOT1) return true;
	return false;
}


AosXmlTagPtr
AosSengTesterMgr::getAccessRcd( const u32 siteid, const u64 &docid)
{
	return AosSengAdmin::getSelf()->getAccessRcd(
			siteid, docid, mRootSsid, false, false, mRootUrlDocid);
}


bool
AosSengTesterMgr::loginAsRoot()
{
	if (mRootSsid!="") return true;
	OmnString ssid;
	AosXmlTagPtr userdoc;
	u64 userid;
	u64 urldocid;
	bool success = AosSengAdmin::getSelf()->login(mRootUsername, mRootPasswd,
								mRootCtnr, ssid, urldocid, userid, userdoc, sgSiteid);
	aos_assert_r(success, false);
	aos_assert_r(ssid != "", false);
	aos_assert_r(userdoc, false);
	aos_assert_r(urldocid, false);
	
	mRootSsid = ssid;
	mRootCid = userdoc->getAttrStr(AOSTAG_CLOUDID);
	mRootUserdoc = userdoc;
	mRootUrlDocid = urldocid;
	return true;
}


AosXmlTagPtr	
AosSengTesterMgr::retrieveXmlDoc(const u32 siteid, const OmnString &objid)
{
	return AosSengAdmin::getSelf()->retrieveDocByObjid(
			siteid, mRootSsid, mRootUrlDocid, objid, true);
}


u32
AosSengTesterMgr::getDftSiteid()
{
	return sgSiteid;
}

u64
AosSengTesterMgr::getSpecialDocid()
{
	aos_assert_r(mSpecialDocid < 5000, 0);
	return mSpecialDocid ++;
}

