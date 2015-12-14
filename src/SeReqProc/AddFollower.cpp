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
#include "SeReqProc/AddFollower.h"

#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "Microblog/Microblog.h"


AosAddFollower::AosAddFollower(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_ADD_FOLLOWER, 
		AosSeReqid::eAddFollower, rflag)
{
}


bool 
AosAddFollower::proc(const AosRundataPtr &rdata)
{
	/*
	// microblogging. This function adds the requester to 
	// the follower's List. 
	//<request reqid = "addFollower" >
	//	<objdef>
	//		<Contents>
	//			<byfollower_cid>xxxx</byfollower_cid>
	//		</Contents>
	//	</objdef>
	//</request>
	
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	aos_assert_r(root, false);
	if (!root)
	{
		AosSetErrorUser(rdata, "missing_request") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	aos_assert_r(objdef, false);
	if (!objdef)
	{
		AosSetErrorUser(rdata, "missing_objdef") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	aos_assert_r(contents, false);
	if (!contents)
	{
		AosSetErrorUser(rdata, "missing_contents") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString byfollower_cid = contents->getNodeText("byfollower_cid");
	aos_assert_r(byfollower_cid != "", false);
	if (byfollower_cid == "")
	{
		AosSetErrorUser(rdata, "missing_follower_cloudid") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString type = contents->getNodeText("type");
	if (type == "circulate")
	{
		OmnString gicid = contents->getNodeText("gicid");
		aos_assert_rr(gicid != "", rdata, false);
		OmnString userdata = contents->getNodeText("udata");
		bool rslt = AosMicroblog::getSelf()->registFollower(byfollower_cid, gicid, userdata, rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	else
	{
		bool rslt = AosMicroblog::getSelf()->addFollower(byfollower_cid, rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

