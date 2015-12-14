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
// Modification History:
// 2015/01/28	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/JqlLogin.h"

#include "SeReqProc/ReqidNames.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "Security/SessionMgr.h"

AosJqlLogin::AosJqlLogin(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_JQLLOGIN, 
		AosSeReqid::eJqlLogin, rflag)
{
}


bool 
AosJqlLogin::proc(const AosRundataPtr &rdata)
{
	//<request>
	//	<username>root</username>
	//	<pwd>12345</pwd>
	//	<ctnr>sys_user</ctnr>
	//</request>
	OmnString contents;
	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "missing root request";
		return false;
	}
	AosXmlTagPtr request = root->getFirstChild("request");
	if (!request)
	{
		rdata->setError() << "missing request";
		return false;
	}
	OmnString errmsg;
	OmnString username = request->getNodeText("username");
	OmnString pwd = request->getNodeText("pwd");
	OmnString ctnr = request->getNodeText("ctnr");
	AosXmlTagPtr userdoc;
	bool rslt = AosCheckLogin(errmsg, userdoc, username, pwd, ctnr, rdata);
	if (!rslt || errmsg != "")
	{
		rdata->setError() << errmsg;
		//felicia, 2015/08/31 for bug JIMODB-626
		return false;
	}

	OmnString ssid;
	AosSessionObjPtr mSession;
	mSession = rdata->getSession();	
	ssid = AosSessionMgr::getSelf()->createSession(
			rdata, userdoc, "", "", mSession);
	if (!mSession)
	{
		errmsg = "create session fail";
		return false;
	}

	AosXmlTagPtr loginobj = userdoc->clone(AosMemoryCheckerArgsBegin);
	if (loginobj)
	{
		loginobj->setNodeName("zky_lgnobj");
		loginobj->setAttr(AOSTAG_USERID, userdoc->getAttrStr(AOSTAG_DOCID));
		loginobj->setAttr(AOSTAG_CTNR_HOME, userdoc->getAttrStr(AOSTAG_CTNR_HOME));
		loginobj->setAttr(AOSTAG_OBJID, userdoc->getAttrStr(AOSTAG_OBJID));
		loginobj->setAttr(AOSTAG_USERNAME, userdoc->getAttrStr(AOSTAG_USERNAME));

		loginobj->setAttr(AOSTAG_SESSIONID, ssid);
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
		loginobj->removeNode(AOSTAG_PASSWD, false, false);
		OmnString contents;
		contents << "<Contents>"
				 << loginobj->toString()
				 << "</Contents>";
		rdata->setOk();
		rdata->setResults(contents);
	}
	rdata->setOk();
	return true;
}

