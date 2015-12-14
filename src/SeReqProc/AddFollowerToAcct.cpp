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
// 02/22/2012	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/AddFollowerToAcct.h"

#include "MsgService/MsgService.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"

AosAddFollowerToAcct::AosAddFollowerToAcct(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_ADDFOLLOWERTOACCT,
		AosSeReqid::eAddFollowerToAcct, rflag)
{
}


bool 
AosAddFollowerToAcct::proc(const AosRundataPtr &rdata)
{
	/*
	// This function is called when a user wants to send an instant message
	// to another user. 
	// <request reqid = "addfollowertoacct">
	// 	<objdef>
	// 		<Contents>
	// 			<follower_cid>xxxx</follower_cid>
	// 			<account>xxxx</account>
	//  	</Contents>
	//  </objdef>
	//</request>
	//
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		AosSetErrorUser(rdata, "missing_request") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	if (!objdef)
	{
		AosSetErrorUser(rdata, "missing_objdef") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	if (!contents)
	{
		AosSetErrorUser(rdata, "missing_contents") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString account = contents->getNodeText("account");
	if (account == "")
	{
		AosSetErrorUser(rdata, "missing_account") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString follower = contents->getNodeText("follower_cid");
	if (follower == "")
	{
		AosSetErrorUser(rdata, "missing_follower") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//check follower_cid is our system user
	bool send = AosMsgService::getSelf()->addFollowerToAcct(follower, account, rdata);
	aos_assert_rr(send, rdata, false);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

