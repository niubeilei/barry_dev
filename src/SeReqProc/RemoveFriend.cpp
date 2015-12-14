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
// 06/22/2011	Created by Michael 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RemoveFriend.h"

#include "InstantMsg/IMManager.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"


AosRemoveFriend::AosRemoveFriend(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REMOVE_FRIEND, 
		AosSeReqid::eRemoveFriend, rflag)
{
}


bool 
AosRemoveFriend::proc(const AosRundataPtr &rdata)
{
	/*
	//<request reqid="removefriend">
	//	<objdef>
	//		<Contents>
	//			<friend_cid>xxxx</friend>
	//			<self_gid>xxxx</self_gid>
	//		</Contents>
	//	</objdef>
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
		rdata->setError() << "Missing contents";
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

	OmnString self_gid = contents->getNodeText("self_gid");
	if (self_gid == "")
	{
		rdata->setError() << "Missing the yourself groupid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString friend_gid = contents->getNodeText("friend_gid");
	if (friend_gid == "")
	{
		rdata->setError() << "Missing the friend groupid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}


	bool rslt = AosIMManager::getSelf()->removeFriend(friend_cid, self_gid, friend_gid, rdata);
	if (!rslt)
	{
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

