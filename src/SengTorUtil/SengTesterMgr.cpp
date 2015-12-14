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
#include "SengTorUtil/SengTesterMgr.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTester.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StUtil.h"
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
#include "Thrift/AosJimoAPI.h" 

#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "Thrift/Jimo_types.h"

#include "JQLStatement/JqlStmtUserMgr.h"           
#include "Util/OmnNew.h"
#include "ThriftClientCPP/Headers.h"
 
extern shared_ptr<AosJimoAPIClient> gThriftClient;
JmoRundata			AosSengTesterMgr::smRunData;
JmoCallData			AosSengTesterMgr::mCallData;

const u32 sgSiteid = 100;

JmoRundata AosSengTesterMgr::smJmoRundata;

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
	
	JmoRundata rdata;
	int64_t site_id = 100;
	string username = "root";
	string password = "12345";
	string userdomain = "zky_sysuser";
	/*
	OmnString objid = getObjid(username);                          
    OmnString doc;
	doc <<  "<loginobj opr=\"login\" "
		<< "container=\"" << userdomain << "\" "
		<< "login_group=\"" << userdomain << "\" "
		<< "zky_uname=\"" << username << "\">"
		<< "<zky_passwd__n><![CDATA[" << password << "]]></zky_passwd__n>"
		<< "</loginobj>";     
	login(doc);
	
	AosXmlTagPtr doc_xml = AosXmlParser::parse(doc AosMemoryCheckerArgs);  
	doc_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");                    
	doc_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");                   
	doc_xml->setAttr(AOSTAG_OBJID, objid); 
	mRootObjid = doc_xml->getAttrStr(AOSTAG_OBJID);
	*/

	JmoCallData call_data;
	login(call_data, site_id, userdomain, username, password);

	mThrds = OmnNew AosSengTestThrdPtr[num_thrds];


	/*
 	if (createUser())
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
	
	*/
}


AosSengTesterMgr::~AosSengTesterMgr()
{
	delete [] mThrds;
}


bool
AosSengTesterMgr::config(const AosXmlTagPtr &config)
{
	for (u32 i=0; i<mTesters.size(); i++)
	{
		mTesters[i]->configTester(config);
	}

	AosSengTester::configStatic(config);
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


bool 
AosSengTesterMgr::startTest()
{
	OmnScreen << "    Start Tester ..."<<endl;
	AosSengTesterMgrPtr thisptr(this, false);
	for (int i=0; i<mNumThrds; i++)
	{
		mThrds[i] = OmnNew AosSengTestThrd(thisptr, mNumTries, i, mTesters);
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

OmnString
AosSengTesterMgr::getObjid(OmnString name)
{
	OmnString objid = "";
	objid << AOSZTG_USER_CTNR << "_" << name; 
	
	return objid;
}

bool
AosSengTesterMgr::createUser()
{
	JmoRundata rdata;
	JmoCallData handle;
	string username = "root";
	string password = "12345";
	string user_domain = "yunyuyan_account";
	
	OmnString objid = getObjid(username);                          
	OmnString doc;
	doc << "<embedobj zky_sdocemail=\"sdmcl_userregistemail\" "
		<< " zky_uname=\"" << username << "\""
		<< " zky_usrctnr=\"user_domain\" zky_pctrs=\"user_domain\">"
		<< "    <zky_passwd__n1><![CDATA[" << password<< "]]></zky_passwd__n1>"
		<< "    <zky_passwd__n><![CDATA[" << password<<"]]></zky_passwd__n>"
		<< "</embedobj>";
	AosXmlTagPtr doc_xml = AosXmlParser::parse(doc AosMemoryCheckerArgs);
	//generate the xml data firstly
	doc_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");                    
	doc_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");                   
	doc_xml->setAttr(AOSTAG_OBJID, objid); 

	gThriftClient->createUser(rdata, handle, doc_xml->toString()); 
	if (rdata.rcode != JmoReturnCode::SUCCESS)
	{
		OmnAlarm << "Failed to create User doc.";
		return false;
	}
	cout << "User \"" << username << "\" created successfully.";
	return true;
}


bool
AosSengTesterMgr::createSuperUser()
{
	OmnString requester_passwd = "als;kdlkqewrcvqwtiware346d";
	AosXmlTagPtr userdoc;

	// To create super user
	// bool rslt = AosSengAdmin::getSelf()->createSuperUser(requester_passwd, userdoc, sgSiteid);	
	// API XXXXXXXXXXXXXXXXXXXXX
	// aos_assert_r(rslt, false);
	OmnNotImplementedYet;
	return false;
	/*
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
	*/
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

	/* API XXXXXXXXXXXXXXXXXXX
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByDocid(sgSiteid, ssid, mRootDocid, false, mRootUrlDocid);
	aos_assert_r(doc, "");
	OmnString cid = doc->getAttrStr(AOSTAG_CLOUDID);
	mRootCid = cid;
	return cid;
	*/
	OmnNotImplementedYet;
	return "";
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
	/* API XXXXXXXXXXXX
	return AosSengAdmin::getSelf()->getAccessRcd(
			siteid, docid, mRootSsid, false, false, mRootUrlDocid);
	*/
	OmnNotImplementedYet;
	return 0;
}


bool
AosSengTesterMgr::loginAsNormalUser(JmoCallData &call_data) 
{
	JmoRundata rdata;
	gThriftClient->loginNew(rdata, mSiteId, mAPIKey, mAppID, mRootCtnr, mNormUser, mNormPasswd);
	aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);
	call_data = rdata.call_data;
	return true;
}

bool
AosSengTesterMgr::login(
		JmoCallData &call_data, 
		const u64 site_id,
		const OmnString &container,
		const OmnString &user_name,
		const OmnString &password)
{
	gThriftClient->loginNew(smRunData, site_id, mAPIKey, mAppID, container, user_name, password);
	
	aos_assert_r(smRunData.rcode == JmoReturnCode::SUCCESS, false);
	mCallData = smRunData.call_data;

	return true;
}


/*
bool
AosSengTesterMgr::login(
		JmoCallData &call_data, 
		const u64 site_id,
		const OmnString &container,
		const OmnString &user_name,
		const OmnString &password)
{
	JmoRundata rdata;
	gThriftClient->loginNew(rdata, site_id, mAPIKey, mAppID, container, user_name, password);
	aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);

	call_data = rdata.call_data;
	return true;
}
*/


bool
AosSengTesterMgr::login(const OmnString conf)
{
	//JmoRundata rdata;
	/*
	i64 site_id = 100;
	string api_key = "api_key";
	string app_id= "app_id";
	string user_name = "root";
	string password = "12345";
	string user_domain = "zky_sysuser";
	gThriftClient->loginNew(smJmoRundata, site_id, api_key, app_id, user_domain, user_name, password);
	aos_assert_r(smJmoRundata.rcode == JmoReturnCode::SUCCESS, false); 
	mCallData = smJmoRundata.call_data;
	*/
	gThriftClient->login(smRunData, conf);
	aos_assert_r(smRunData.rcode == jimoapi::JmoReturnCode::SUCCESS, false); 

	mCallData.session_id = smRunData.session_id;
	aos_assert_r(mCallData.session_id != "", false);

	return true;
}


bool
AosSengTesterMgr::loginAsRoot()
{
	/* API XXXXXXXXXXXXXXXXXX
	if (mRootSsid!="") return true;
	OmnString ssid;
	AosXmlTagPtr userdoc;
	u64 userid;
	u64 urldocid;
	JmoRundata rdata;
	gThriftClient->loginNew(rdata, mSiteId, mAPIKey, mAppID, mRootCtnr, mRootUsername, mRootPasswd);
	aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);

	mCallData = rdata.call_data;
	aos_assert_r(mCallData.session_id != "", false);
	
	mRootSsid = ssid;
	mRootCid = userdoc->getAttrStr(AOSTAG_CLOUDID);
	mRootUserdoc = userdoc;
	mRootUrlDocid = urldocid;
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


AosXmlTagPtr	
AosSengTesterMgr::retrieveXmlDoc(const u32 siteid, const OmnString &objid)
{
	/* API *************
	return AosSengAdmin::getSelf()->retrieveDocByObjid(
			siteid, mRootSsid, mRootUrlDocid, objid, true);
	*/
	OmnNotImplementedYet;
	return 0;
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


bool 
AosSengTesterMgr::addTester(const AosSengTesterPtr &tester)
{
	aos_assert_r(tester, false);
	mTesters.push_back(tester);
	
	AosSengTesterMgrPtr thisptr(this, false);
	tester->setTesterMgr(thisptr);

	return true;
}

