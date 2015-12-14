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
// The torturer is in SengTorturer/TesterLoginNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/Login.h"

#include "Actions/ActSeqno.h"
#include "SEInterfaces/DocClientObj.h"
#include "SearchEngine/DocServerCb.h"
#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/ScrtUtil.h"
#include "Security/SessionMgr.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SeSysLog/Ptrs.h"
#include "SeSysLog/SeSysLog.h"
#include "SEModules/ImgProc.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/MsgClientObj.h"


AosLogin::AosLogin(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_LOGIN, 
		AosSeReqid::eLogin, rflag)
{
}


bool 
AosLogin::proc(const AosRundataPtr &rdata)
{
	// 	<cmd opr="login"
	// 		container="xxx"
	// 		AOSTAG_LOGIN_VPD="xxx"
	// 		zky_hpvpd="xxx"			// This is the vpd after the login
	// 		username="xxx">
	// 		<passwd>xxx</passwd>
	// 	</operation>
	//
	
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr cmd = rootchild->getFirstChild("command");
	if (cmd)
	{
		cmd = cmd->getFirstChild();
	}

	rdata->setNeedLoginObj(false);

	AosXmlTagPtr user_doc;
	OmnString personalVpd, workVpd, publicVpd, familyVpd;
	bool rslt = AosCheckLogin(rdata, user_doc, cmd, personalVpd, workVpd, publicVpd, familyVpd);
	if (!rslt)
	{
		AosSetError(rdata, "user_not_found");
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (!user_doc)
	{
		AosSetError(rdata, "Internal Error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString hpvpd = cmd->getAttrStr(AOSTAG_HPVPD);
	OmnString loginvpd = cmd->getAttrStr(AOSTAG_LOGIN_VPD);
	if (hpvpd == "$from_loginvpd")
	{
		if (loginvpd != "")
		{
			// No hpvpd. Check whether it can retrieve it from the loginvpd.
			AosXmlTagPtr lgvpd = AosDocClientObj::getDocClient()->getDocByObjid(loginvpd, rdata);
			if (!lgvpd)
			{
				AosSetError(rdata, AOSLT_FAILED_RETREIVE_LOGIN_VPD);
				OmnAlarm << rdata->getErrmsg() << enderr;
				AOSLOG_LEAVE(rdata);
				return false;
			}
			hpvpd = lgvpd->getAttrStr(AOSTAG_AFTLGI_VPD, "");
		}
		else
		{
			// No hpvpd. check whether it can retrieve it from the roles objid
			OmnString loginvpd1 = loginvpd1 = user_doc->getAttrStr(AOSTAG_ROLES);
			OmnString roles = user_doc->getAttrStr(AOSTAG_ROLES);
			if (roles != "")
			{
				bool finished;
				vector<OmnString> str;
				AosStrSplit::splitStrByChar(roles.data(), ",", str, 100, finished);
				aos_assert_r(str.size() != 0, false);
				OmnString role = str[0];
				AosXmlTagPtr lgvpd = AosDocClientObj::getDocClient()->getDocByObjid(role, rdata);
				if (!lgvpd)
				{
					AosSetError(rdata, "Missing roles objid ");
					OmnAlarm << rdata->getErrmsg() << enderr;
					AOSLOG_LEAVE(rdata);
					return false;
				}
				hpvpd = lgvpd->getAttrStr(AOSTAG_HPVPD, "");
			}
		}
	}

	OmnString mode = cmd->getAttrStr(AOSTAG_MODE);
	OmnString vpd_set_to_session;
	OmnString vpd_to_return;
	if (mode == "$none")
	{
		// The user wants to ignore hpvpd. 
		vpd_to_return = "";
	}
	else if (mode == "$replace")
	{
		// It returns the hpvpd but does not set to session
		vpd_to_return = hpvpd;
	}
	else if (mode == "$setsession")
	{
		// It returns the hpvpd and sets it to the session.
		vpd_to_return = hpvpd;
		vpd_set_to_session = hpvpd;
	} 

	int level = cmd->getAttrInt(AOSTAG_LEVEL, 0);
	// check whether set session; 
	OmnString sessionid;
	AosSessionObjPtr mSession;
	if (level == 0)
	{
		mSession = rdata->getSession();	
		// Login is successful. Need to generate a session id.
		sessionid = AosSessionMgr::getSelf()->createSession(
				rdata, user_doc, loginvpd, vpd_set_to_session, mSession);

		if (!mSession)
		{
			AosSetError(rdata, "Internal Error");
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	// Construct the response:
	// 	<Contents>
	// 		<record hpvpd="xxx">
	// 			<realname>xxx</realname>
	// 			<personpic>xxx</personpic>
	// 		</record>
	// 	</Contents>
	OmnString contents = "<Contents>";

	AosXmlTagPtr loginobj = user_doc->clone(AosMemoryCheckerArgsBegin);

	if (sessionid != "") 
	{
		//record << "zky_ssid=\"" << sessionid << "\" ";
		if (loginobj) loginobj->setAttr(AOSTAG_SESSIONID, sessionid);
	}

	// Chen Ding, 12/26/2011
	// OmnString cookie;
	// cookie << AOSTAG_SESSIONID << "=" << sessionid 
	// 	<< ";path=/;Max-Age=50000;";
	OmnString cookie;
	u64 urldoc_docid = rdata->getUrldocDocid();
	if (urldoc_docid != 0 && level == 0)
	{
		OmnString ssid_name = AosComposeSsidCookieName(urldoc_docid);
		cookie << ssid_name << "=" << sessionid 
			<< ";path=/;Max-Age=50000;";
		rdata->setSendCookie(cookie);
		OmnScreen << "===============: Cookie: " << cookie << endl;
	}

	OmnString realname = user_doc->getAttrStr(AOSTAG_REALNAME);
	OmnString personpic = user_doc->getAttrStr(AOSTAG_OBJIMAGE);

	//Ketty 2011/05/06 check whether pay yet
	OmnString pay_status = user_doc->getAttrStr(AOSTAG_REGIST_PAY_STATUS);	
	if(pay_status == "no_level")
	{
		// not pay yet
		// do something
		AosSetError(rdata, "pay first!");
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	//Ketty End

	if (loginobj)
	{
		//Jozhi 2014-08-06 set msg url
		OmnString url = AosMsgClientObj::getMsgSvrUrlStatic();
		loginobj->setAttr("zky_msgsvr_url", url);

		//ZKY-2776 Ice 2011/2/14 
		loginobj->setNodeName("zky_lgnobj");
		loginobj->setAttr(AOSTAG_USERID, user_doc->getAttrStr(AOSTAG_DOCID));
		loginobj->setAttr(AOSTAG_CTNR_HOME, user_doc->getAttrStr(AOSTAG_CTNR_HOME));
		loginobj->setAttr(AOSTAG_OBJID, user_doc->getAttrStr(AOSTAG_OBJID));
		loginobj->setAttr(AOSTAG_USERNAME, user_doc->getAttrStr(AOSTAG_USERNAME));

		if (vpd_to_return != "") loginobj->setAttr(AOSTAG_HPVPD, vpd_to_return);
		loginobj->setAttr(AOSTAG_PERSONAL_VPD, personalVpd);
		loginobj->setAttr(AOSTAG_WORK_VPD, workVpd);
		loginobj->setAttr(AOSTAG_PUBLIC_VPD, publicVpd);
		loginobj->setAttr(AOSTAG_FAMILY_VPD, familyVpd);
		loginobj->setAttr(AOSTAG_REALNAME, realname);
		loginobj->setAttr(AOSTAG_OBJIMAGE, personpic);
		if (cmd->getAttrStr("getsysurl") == "true")
		{
			//is used by torturer.
			loginobj->setAttr("zky_sysurldocid", AOSDOCID_SYSURL);
		}

		loginobj->removeAttr("zky_modby");
		loginobj->removeAttr(AOSTAG_DOCID);
		loginobj->removeAttr(AOSTAG_USERSTATUS);
		loginobj->removeAttr(AOSTAG_MTIME);
		loginobj->removeAttr(AOSTAG_MT_EPOCH);
		loginobj->removeAttr(AOSTAG_CTIME);
		loginobj->removeAttr(AOSTAG_CT_EPOCH);
		loginobj->removeAttr(AOSTAG_OTYPE);
		loginobj->removeAttr(AOSTAG_ACCT_TYPE);
		loginobj->removeAttr(AOSTAG_PASSWD);
		loginobj->removeAttr("zky_password__n1");
		//james 2011/02/19
		loginobj->removeNode(AOSTAG_PASSWD, false, false);
		if (mSession) mSession->modifyLoginObj(loginobj);
	
		contents << loginobj->toString() << "</Contents>";;
		rdata->setOk();
		rdata->setResults(contents);
	}
	rdata->setArg1("zky_login_status", "login");
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_LOGIN, rdata);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

