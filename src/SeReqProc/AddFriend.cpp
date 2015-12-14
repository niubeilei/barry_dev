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
// 08/30/2011	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/AddFriend.h"

#include "Security/SecurityMgr.h"
#include "InstantMsg/IMManager.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

AosAddFriend::AosAddFriend(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_ADD_FRIEND, AosSeReqid::eAddFriend, rflag)
{
}


bool 
AosAddFriend::proc(const AosRundataPtr &rdata)
{
	/*
	//<request reqid = "addfriend" >
	//  <objdef>
	//  	<Contents>
	//      	<friend_cid>xxxx</friend_cid>
	//      	<self_gid>xxxx</self_gid>
	//      	<friend_gid>xxxx</friend_gid>
	//  	</Contents>
	//  </objdef>
	//</request>
	
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
	OmnString friend_cid = contents->getNodeText("friend_cid");
	if (friend_cid == "")
	{
		AosSetErrorUser(rdata, "missing_friend_cid") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString friend_gid = contents->getNodeText("friend_gid");
	if (friend_gid == "")
	{
		AosSetErrorUser(rdata, "missing_groupid") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString self_gid = contents->getNodeText("self_gid");
	if (self_gid == "")
	{
		AosSetErrorUser(rdata, "missing_self_groupid") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt = AosIMManager::getSelf()->addFriend(friend_cid, self_gid, friend_gid, rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata, "failed_add_friend") << root->toString() << enderr;
		return false;
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

