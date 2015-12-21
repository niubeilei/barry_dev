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
// 08/11/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/LoginToEsvr.h"

#include "SEServer/SeReqProc.h"
#include "EmailClt/EmailClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/Ptrs.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "XmlUtil/XmlTag.h"


AosLoginToEsvr::AosLoginToEsvr(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_LOGINTOESVR, AosSeReqid::eLgEsvr, rflag)
{
}


bool 
AosLoginToEsvr::proc(const AosRundataPtr &rdata)
{
	// This function is created by Brian Zhang 11/08/2011
	// When we login to email server, we retrieve all email
	// and convert it into our doc to our system, then when
	// we use email, we just retrieve doc.
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
	if (!xmldoc)
	{
		rdata->setError() << "Missing object";
		OmnAlarm <<  rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr doc = xmldoc->getFirstChild();
	aos_assert_r(doc, false);

	// Get email attach director
	OmnNotImplementedYet;			// Chen Ding, 02/25/2012
	OmnString emldir;// = lgmgr->getEmailDir();
	aos_assert_rr(emldir != "", rdata, false);

	// Get user name 
	OmnString uname = doc->getAttrStr(AOSTAG_EMAIL_UNAME);
	if (uname == "")
	{
		rdata->setError() << "Missing user name";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Get password
	OmnString passwd = doc->getAttrStr(AOSTAG_EMAIL_PASSWD);
	if (passwd == "")
	{
		rdata->setError() << "Missing password";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
#if 0
	bool rslt = AosEmailClient::getSelf()->login(uname, passwd, emldir, rdata);
	if (!rslt)
	{
		rdata->setError() << "Login to email server failed";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
#endif
	
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}
