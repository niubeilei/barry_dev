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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CreateSuperUser.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEServer/SeReqProc.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosCreateSuperUser::AosCreateSuperUser(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_SUPER_USER, 
		AosSeReqid::eCreateSuperUser, rflag)
{
}


bool 
AosCreateSuperUser::proc(const AosRundataPtr &rdata)
{
	// This function is created by Linda 
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	AosXmlTagPtr userobj;
	if (!objdef || !(userobj = objdef->getFirstChild()))
	{
		rdata->setError() << "Missing objdef!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//Linda 2010/12/06
	OmnString requester_passwd = userobj->getAttrStr(AOSTAG_PASSWD);
	if (requester_passwd == "")
	{
		rdata->setError() << "Missing password!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr userdoc = AosLoginMgr_CreateSuperUser(rdata);
	if (!userdoc)
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (!userdoc)
	{
		OmnAlarm << "Userdoc null!" << enderr;
		rdata->setError() << "Failed to create super user!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool exist;
	OmnString passwd_path = AOSTAG_PASSWD;
	passwd_path <<"/_#text";
	OmnString contents ="<Contents><record ";
	contents << AOSTAG_OBJID <<"=\"" << userdoc->getAttrStr(AOSTAG_OBJID) <<"\" "
		    << AOSTAG_DOCID << "=\"" << userdoc->getAttrStr(AOSTAG_DOCID) << "\" "
			<< AOSTAG_CTNR_HOME << "=\"" << userdoc->getAttrStr(AOSTAG_CTNR_HOME) << "\" "
			<< AOSTAG_PARENTC << "=\"" << userdoc->getAttrStr(AOSTAG_PARENTC) << "\" "
			<< AOSTAG_USERNAME << "=\"" << userdoc->getAttrStr(AOSTAG_USERNAME) << "\" "
			<< AOSTAG_CLOUDID << "=\"" << userdoc->getAttrStr(AOSTAG_CLOUDID) << "\">"
			<< "<passwd><![CDATA["
			<< userdoc->xpathQuery(passwd_path, exist, "") << "]]></passwd>"
			<< "</record></Contents>";
	rdata->setOk();
	rdata->setResults(contents);
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_CREATE_SUPER_USER, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}
