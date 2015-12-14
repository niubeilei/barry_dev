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
// 10/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ModifyPassword.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/CodeComp.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosModifyPassword::AosModifyPassword(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_MODIFYPASSWORD, AosSeReqid::eModifyPassword, rflag)
{
}


bool 
AosModifyPassword::proc(const AosRundataPtr &rdata)
{
	// A user requests logging out. Logout means canceling the session.
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr child = root->getFirstChild("objdef");
	aos_assert_r(child,false);

	AosXmlTagPtr child1 = child->getFirstChild();
	aos_assert_r(child1,false);

	OmnString oldpasswd = child1->getAttrStr(AOSTAG_OLDPASSWD, "");
	if(oldpasswd == "")
	{
		rdata->setError() << "Missing the old password";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString newpasswd = child1->getAttrStr(AOSTAG_PASSWD, "");
	if(newpasswd == "")
	{
		rdata->setError() << "Missing the new password";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	u64 docid = rdata->getUserid();
	if(!docid)
	{
		rdata->setError() << "Missing Userid";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr thedoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	if(!thedoc)
	{
		rdata->setError() << "Missing User Account";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr doc =  thedoc->clone(AosMemoryCheckerArgsBegin);
	OmnString passwd = doc->getNodeText(AOSTAG_PASSWD);
	if(passwd == "")
	{
		rdata->setError() << "Missing The Password In User Account";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	if(passwd == oldpasswd)
	{
		doc->setNodeText(AOSTAG_PASSWD, newpasswd, true);
		bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, doc, "false", false);
		if(!rslt)
		{
			rdata->setError() << "there is the error to create new password !";
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}
	else
	{
		rdata->setError() << "old password is not correct";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

