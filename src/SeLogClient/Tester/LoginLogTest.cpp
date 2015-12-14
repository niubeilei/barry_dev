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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeLogClient/Tester/LoginLogTest.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"
#include "SeLogClient/Tester/ReqidNames.h"
#include "SearchEngineAdmin/SengAdmin.h"


static u32 cloudid = 100034;

AosLoginLogTest::AosLoginLogTest(const bool flag)
:
AosLogReqProc(AOSREQIDNAME_LOGINLOGTEST, AosLogReqid::eLogin, flag)
{
}


AosLoginLogTest::AosLoginLogTest()
{
}


AosLoginLogTest::~AosLoginLogTest()
{
}


bool
AosLoginLogTest::proc(const AosRundataPtr &rdata)
{
	int nn = rand()%100;
	if (nn < 50) return loginOk(rdata);
	if ((51 < nn) && (nn < 60)) return loginNoContainer(rdata);
	if ((61 < nn) && (nn < 70)) return loginWithWrongPasswd(rdata);
	if ((71 < nn) && (nn < 80)) return loginNoPasswd(rdata);
	if ((81 < nn) && (nn < 99)) return loginNoUnameNoPasswd(rdata);
	if ((91 < nn) && (nn < 99)) return retrieveLog(rdata);

	return true;
}


bool
AosLoginLogTest::loginOk(const AosRundataPtr &rdata)
{
	OmnString cid;
	cid << cloudid;
	AosXmlTagPtr userdoc = AosSengAdmin::getSelf()->retrieveDocByCloudid(
							rdata->getSiteid(), cid);
	if (!userdoc) return true;
	
	OmnString uname = userdoc->getAttrStr(AOSTAG_USERNAME);
	aos_assert_rr(uname != "", rdata, false);

	AosXmlTagPtr pwstag = userdoc->getFirstChild(AOSTAG_PASSWD);
	aos_assert_rr(pwstag, rdata, false);
	OmnString passwd = pwstag->getNodeText();
	aos_assert_rr(passwd != "", rdata, false);

	OmnString ctnr_objid = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_rr(ctnr_objid != "", rdata, false);

	u64 userid;
	OmnString ssid;
	AosXmlTagPtr udoc;
	
	bool rslt = AosSengAdmin::getSelf()->login(uname, passwd, ctnr_objid,
								ssid, userid, udoc, rdata->getSiteid());
	if (!rslt)
	{
		rdata->setError() << "Login failed: " << uname << " : " << passwd;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}



bool
AosLoginLogTest::loginWithWrongPasswd(const AosRundataPtr &rdata)
{
	OmnString cid;
	cid << cloudid;
	AosXmlTagPtr userdoc = AosSengAdmin::getSelf()->retrieveDocByCloudid(
							rdata->getSiteid(), cid);
	if (!userdoc) return true;
	
	OmnString uname = userdoc->getAttrStr(AOSTAG_USERNAME);
	aos_assert_rr(uname != "", rdata, false);

	AosXmlTagPtr pwstag = userdoc->getFirstChild(AOSTAG_PASSWD);
	aos_assert_rr(pwstag, rdata, false);
	OmnString passwd = pwstag->getNodeText();
	aos_assert_rr(passwd != "", rdata, false);
	passwd << "1";
	
	OmnString ctnr_objid = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_rr(ctnr_objid != "", rdata, false);

	u64 userid;
	OmnString ssid;
	AosXmlTagPtr udoc;
	
	bool rslt = AosSengAdmin::getSelf()->login(uname, passwd, ctnr_objid,
								ssid, userid, udoc, rdata->getSiteid());
	if (rslt)
	{
		rdata->setError() << "Login success: " << uname << " : " << passwd;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool
AosLoginLogTest::loginNoPasswd(const AosRundataPtr &rdata)
{
	OmnString cid;
	cid << cloudid;
	AosXmlTagPtr userdoc = AosSengAdmin::getSelf()->retrieveDocByCloudid(
							rdata->getSiteid(), cid);
	if (!userdoc) return true;
	
	OmnString uname = userdoc->getAttrStr(AOSTAG_USERNAME);
	aos_assert_rr(uname != "", rdata, false);

	
	OmnString passwd = "";
	OmnString ctnr_objid = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_rr(ctnr_objid != "", rdata, false);

	u64 userid;
	OmnString ssid;
	AosXmlTagPtr udoc;
	
	bool rslt = AosSengAdmin::getSelf()->login(uname, passwd, ctnr_objid,
								ssid, userid, udoc, rdata->getSiteid());
	if (rslt)
	{
		rdata->setError() << "Login success: " << uname << " : " << passwd;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}

	
bool
AosLoginLogTest::loginNoUnameNoPasswd(const AosRundataPtr &rdata)
{
	OmnString cid;
	cid << cloudid;
	AosXmlTagPtr userdoc = AosSengAdmin::getSelf()->retrieveDocByCloudid(
							rdata->getSiteid(), cid);
	if (!userdoc) return true;
	
	OmnString uname = "";

	
	OmnString passwd = "";
	OmnString ctnr_objid = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_rr(ctnr_objid != "", rdata, false);

	u64 userid;
	OmnString ssid;
	AosXmlTagPtr udoc;
	
	bool rslt = AosSengAdmin::getSelf()->login(uname, passwd, ctnr_objid,
								ssid, userid, udoc, rdata->getSiteid());
	if (rslt)
	{
		rdata->setError() << "Login success: " << uname << " : " << passwd;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}

	
bool
AosLoginLogTest::loginNoContainer(const AosRundataPtr &rdata)
{
	OmnString cid;
	cid << cloudid;
	AosXmlTagPtr userdoc = AosSengAdmin::getSelf()->retrieveDocByCloudid(
							rdata->getSiteid(), cid);
	if (!userdoc) return true;

	OmnString uname = userdoc->getAttrStr(AOSTAG_USERNAME);
	aos_assert_rr(uname != "", rdata, false);

	AosXmlTagPtr pwstag = userdoc->getFirstChild(AOSTAG_PASSWD);
	aos_assert_rr(pwstag, rdata, false);
	OmnString passwd = pwstag->getNodeText();
	aos_assert_rr(passwd != "", rdata, false);

	OmnString ctnr_objid = ""; 

	u64 userid;
	OmnString ssid;
	AosXmlTagPtr udoc;
	
	bool rslt = AosSengAdmin::getSelf()->login(uname, passwd, ctnr_objid,
								ssid, userid, udoc, rdata->getSiteid());
	if (rslt)
	{
		rdata->setError() << "Login success: " << uname << " : " << passwd;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}

	
bool
AosLoginLogTest::retrieveLog(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}
