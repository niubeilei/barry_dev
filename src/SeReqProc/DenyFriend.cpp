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
// 06/28/2011	Created by Michael 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/DenyFriend.h"

#include "InstantMsg/IMManager.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"


AosDenyFriend::AosDenyFriend(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_DENY_FRIEND, 
		AosSeReqid::eDenyFriend, rflag)
{
}


bool 
AosDenyFriend::proc(const AosRundataPtr &rdata)
{
	/*
	//<request reqid = "denyfriend" >
	//  <objdef>
	//  	<Contents>
	//      	<friend_cid>xxxx</friend_cid>
	//      	<msg>xxxx</msg>
	//  	</Contents>
	//  </objdef>
	//</request>
	
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Missing objdef";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	if (!contents)
	{
		rdata->setError() << "Missing the friend object!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString friend_cid = contents->getNodeText("friend_cid");
	if (friend_cid == "")
	{
		rdata->setError() << "Missing the friend cid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString msg = contents->getNodeText("msg");
	if (msg == "")
	{
		rdata->setError() << "Missing the deny msg";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt = AosIMManager::getSelf()->denyFriend(friend_cid, msg, rdata);
	if (!rslt)
	{
		rdata->setError() << "Deny friend failed!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

