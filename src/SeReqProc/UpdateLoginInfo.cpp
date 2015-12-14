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
#include "SeReqProc/UpdateLoginInfo.h"

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
#include "MsgClient/MsgClient.h"


AosUpdateLoginInfo::AosUpdateLoginInfo(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_UPDATE_LOGIN_INFO, 
		AosSeReqid::eUpdateLoginInfo, rflag)
{
}


bool 
AosUpdateLoginInfo::proc(const AosRundataPtr &rdata)
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

	OmnString cid = rdata->getCid();
	if (cid == "")
	{
		rdata->setError() << "Missing user's cloud id";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr user_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
	if (!user_doc)
	{
		rdata->setError() << "Missing user's doc";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr loginobj = user_doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(loginobj, false);

	OmnString hpvpd = user_doc->getAttrStr(AOSTAG_HPVPD);         
	OmnString workvpd = user_doc->getAttrStr(AOSTAG_WORK_VPD);
	OmnString publicvpd = user_doc->getAttrStr(AOSTAG_PUBLIC_VPD);
	OmnString familyvpd = user_doc->getAttrStr(AOSTAG_FAMILY_VPD);
	OmnString realname = user_doc->getAttrStr(AOSTAG_REALNAME); 
	OmnString personpic = user_doc->getAttrStr(AOSTAG_OBJIMAGE);


	OmnString ssid = rdata->getSsid();
	if (ssid == "") 
	{
		rdata->setError() << "Missing user's doc";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	loginobj->setAttr(AOSTAG_SESSIONID, ssid);
	
	OmnString url = AosMsgClientObj::getMsgSvrUrlStatic();
	loginobj->setAttr("zky_msgsvr_url", url);
	
	loginobj->setNodeName("zky_lgnobj");
	loginobj->setAttr(AOSTAG_USERID, user_doc->getAttrStr(AOSTAG_DOCID));
	loginobj->setAttr(AOSTAG_CTNR_HOME, user_doc->getAttrStr(AOSTAG_CTNR_HOME));
	loginobj->setAttr(AOSTAG_OBJID, user_doc->getAttrStr(AOSTAG_OBJID));
	loginobj->setAttr(AOSTAG_USERNAME, user_doc->getAttrStr(AOSTAG_USERNAME));

	loginobj->setAttr(AOSTAG_PERSONAL_VPD, hpvpd);
	loginobj->setAttr(AOSTAG_WORK_VPD, workvpd);
	loginobj->setAttr(AOSTAG_PUBLIC_VPD, publicvpd);
	loginobj->setAttr(AOSTAG_FAMILY_VPD, familyvpd);
	loginobj->setAttr(AOSTAG_REALNAME, realname);
	loginobj->setAttr(AOSTAG_OBJIMAGE, personpic);

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
	AosSessionObjPtr session = rdata->getSession();
	if (!session)
	{
		rdata->setError() << "Not login";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	session->modifyLoginObj(loginobj);

	OmnString contents = "<Contents>";
	contents << loginobj->toString() << "</Contents>";;
	rdata->setResults(contents);

	rdata->setOk();
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_LOGIN, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}

