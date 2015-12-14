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
#include "Torturers/XmlDocTorturer/TesterLogin.h"

#include "Thrift/AosJimoAPI.h"
#include "ThriftClientCPP/Headers.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include <stdlib.h>

#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <iostream>
using namespace std;

extern shared_ptr<AosJimoAPIClient> gThriftClient;


AosLoginTester::AosLoginTester(const bool regflag)
:
AosSengTester(AosSengTester_Login, AosTesterId::eLogin, regflag),
mUseCidLoginWeight(50),
mValidUserWeight(50),
mUserRightPasswdWeight(50),
mUserUsernameWeight(50)
{
}


AosLoginTester::AosLoginTester(const AosLoginTester &rhs)
:
AosSengTester(rhs)
{
	mUseCidLoginWeight = rhs.mUseCidLoginWeight;
	mValidUserWeight = rhs.mValidUserWeight;
	mUserRightPasswdWeight = rhs.mUserRightPasswdWeight;
	mUserUsernameWeight = rhs.mUserUsernameWeight;
}


AosLoginTester::AosLoginTester()
:
AosSengTester(AosSengTester_Login, "login", AosTesterId::eLogin),
mUseCidLoginWeight(50),
mValidUserWeight(50),
mUserRightPasswdWeight(50),
mUserUsernameWeight(50)
{
}


AosLoginTester::~AosLoginTester()
{
}


bool 
AosLoginTester::configTester(const AosXmlTagPtr &config)
{
	//	<config ...>
	//		<testers>
	//			<tester name="login" weight="30" 
	//				use_cid_login_weight="45"
	//				valid_user_weight="60"
	//				
	//		</testers>
	//	</config>
	if (!config) return true;
	AosXmlTagPtr testers = config->getFirstChild("testers");
	if (!testers) return true;
	AosXmlTagPtr conf = testers->getFirstChild("login");
	if (!conf) return true;

	if (!AosSengTester::configTester(conf)) return false;

	mUseCidLoginWeight = conf->getAttrInt("use_cid_login_weight", mUseCidLoginWeight);
	aos_assert_r(mUseCidLoginWeight >= 0, false);

	mValidUserWeight = conf->getAttrInt("valid_user_weight", mValidUserWeight);
	aos_assert_r(mValidUserWeight >= 0, false);
	
	mUserRightPasswdWeight= conf->getAttrInt("user_right_passwd_weight", mUserRightPasswdWeight);
	aos_assert_r(mUserRightPasswdWeight>= 0, false);
	
	mUserUsernameWeight= conf->getAttrInt("user_user_name_weight", mUserUsernameWeight);
	aos_assert_r(mUserUsernameWeight>= 0, false);

	return true;
}


AosSengTesterPtr
AosLoginTester::clone()
{
	return OmnNew AosLoginTester();
	return OmnNew AosLoginTester(*this);
}

OmnString
AosLoginTester::getObjid(OmnString name)        
{
	OmnString objid = "";
	objid << AOSZTG_USER_CTNR << "_" << name;

	return objid;
}

bool 
AosLoginTester::test()
{
	JmoRundata rdata;
	JmoCallData handle;
	string username = "root";
	string password = "12345";
	string user_domain = "zky_sysuser";

	OmnString objid = getObjid(username);                          
	OmnString doc;
	doc <<  "<loginobj opr=\"login\" "
		<< "container=\"" << user_domain << "\" "                                 
		<< "login_group=\"" << user_domain << "\" "
		<< "zky_uname=\"" << username << "\">"
		<< "<zky_passwd__n><![CDATA[" << password << "]]></zky_passwd__n>"
	    << "</loginobj>";     

	gThriftClient->login(rdata, doc);
	aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);
	
	return true;
/*
	// 1. Determine the number of tries
	// 1. Pick a user container (correct or incorrect container)
	// 2. Pick a user from that container (correct or incorrect user)
	// 3. Determine whether to use a correct or incorrect password
	// 4. Determine whether to use cloud id or user name to login
 	// int num_tries = rand() % eNumLogin;
 	int num_tries = 10;
	for (int i=0; i<num_tries; i++)
	{
		OmnString ctnr_objid;
		OmnString username;
		OmnString cid;
		OmnString passwd;
		u64 user_docid = 0;
		bool user_valid = false;

		AosStContainerPtr container = mThread->pickContainer();
		if (!container)
		{
			ctnr_objid = OmnRandom::letterStr(eMinCtnrObjidLen, eMaxCtnrObjidLen);
			AosStDocPtr doc = mThread->getDocByObjid(ctnr_objid);
			if (doc)
			{
				u64 local_docid = doc->locdid;
				container = mThread->getContainer(local_docid);
			}
		}

		if (container)
		{
			if (container->mType == AosStContainer1::eUserCtnr)
			{
				// Determine whether to pick a valid user from the container
				if (container->mMembers.size() > 0 && (rand() % 100) < mValidUserWeight)
				{
					user_docid = mThread->pickUser(container);
					aos_assert_r(user_docid > 0, false);
					user_valid = true;
				}
				else
				{
					// Randomly generate a docid
					user_docid = rand();

					// Check whether it is a valid user
					user_valid = mThread->isUserValid(container, user_docid);
				}
			}
			else
			{
				// Determine whether to pick a valid user from the container
				if (container->mMembers.size() > 0 && (rand() % 100) < mValidUserWeight)
				{
					user_docid = mThread->pickUser(container);
					aos_assert_r(user_docid > 0, false);
				}
				else
				{
					// Randomly generate a docid
					user_docid = rand();
				}
			}
			ctnr_objid = container->mObjid;
		}
		else
		{
			if ((rand() % 100) < mUserUsernameWeight)
			{
				AosStUser *user = mThread->pickUser();
				if (user)
				{
					username = user->username;
					passwd = user->passwd;
				}
				else
				{
					username = mThread->createInvalidUsername();
					passwd = OmnRandom::letterStr(eMinPasswdLen, eMaxPasswdLen);
				}
			}
			else
			{
				username = mThread->createInvalidUsername();
				passwd = OmnRandom::letterStr(eMinPasswdLen, eMaxPasswdLen);
			}
		}

		OmnString user_cid;
		AosStUser *local_userdoc;
		bool passwd_correct = false;
		if (user_valid)
		{
			local_userdoc = mThread->getUser(user_docid);
			aos_assert_r(local_userdoc, false);

			// Determine whether to use username or cid to login
			if ((rand() % 100) < mUseCidLoginWeight)
			{
				cid = local_userdoc->cid;
			}
			else
			{
				username = local_userdoc->username;
			}
			aos_assert_r(username != "" || cid != "", false);

			// Determine whether to use right password
			OmnString correct_passwd = local_userdoc->passwd;
			if ((rand() % 100) < mUserRightPasswdWeight)
			{
				passwd = correct_passwd;
				passwd_correct = true;
			}
			else
			{
				int len = rand() % 20;
				passwd = AosGeneratePasswd(len);
			}
		}
		else
		{
			// User invalid
			username = mThread->createInvalidUsername();
			passwd = OmnRandom::letterStr(eMinPasswdLen, eMaxPasswdLen);
		}
		AosXmlTagPtr userdoc1;
		if (local_userdoc && user_valid && passwd_correct)
		{
			userdoc1 = mThread->retrieveDoc(local_userdoc->svrdid);
		}

		OmnString ssid;
		AosXmlTagPtr server_userdoc;
		u64 server_userid;
		if (username == mMgr->getRootUsername() || cid == mMgr->getRootCid())
		{
			continue;
		}
		u64 urldocid;
		bool success = AosSengAdmin::getSelf()->login(
							username, 
							passwd,
							ctnr_objid, 
							ssid, 
							urldocid, 
							server_userid, 
							server_userdoc, 
							mThread->getSiteid(), 
							cid);

		if (user_valid && passwd_correct)
		{
			aos_assert_r(success, false);
			aos_assert_r(ssid != "", false);
			aos_assert_r(server_userdoc, false);
			mThread->setSession(ssid, server_userid, server_userdoc, urldocid);
			return true;
		}
	}

	logoutUser();
	return true;
*/
	OmnNotImplementedYet;
	return false;
}


bool
AosLoginTester::logoutUser()
{
	mThread->resetSession();
	return true;
}

