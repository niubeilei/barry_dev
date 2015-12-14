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
#include "SeReqProc/RemoveFollower.h"

#include "SEServer/SeReqProc.h"
#include "Microblog/Microblog.h"
#include "SeReqProc/ReqidNames.h"


AosRemoveFollower::AosRemoveFollower(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REMOVE_FOLLOWER, 
		AosSeReqid::eRemoveFollower, rflag)
{
}


bool 
AosRemoveFollower::proc(const AosRundataPtr &rdata)
{
	/*
	// This function remove user from the byfollower's list
	//<request reqid = "removeFollower" >
	//  <objdef>
	//  	<Contents>
	//      	<byfollower_cid>xxxx</byfollower_cid>
	//  	</Contents>
	//  </objdef>
	//</request>
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	aos_assert_r(root, false);
	if (!root)
	{
		rdata->setError() << "Missing request";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	aos_assert_r(objdef, false);
	if (!objdef)
	{
		rdata->setError() << "Missing objdef";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	aos_assert_r(contents, false);
	if (!contents)
	{
		rdata->setError() << "Missing the friend object!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString byfollower_cid = contents->getNodeText("byfollower_cid");
	aos_assert_r(byfollower_cid != "", false);
	if (byfollower_cid == "")
	{
		rdata->setError() << "Missing the by follower cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//2. remove follower by online Mgr.
	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		rdata->setError() << "Missing siteid";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr byfollower_doc = AosDocClientObj::getDocClient()->getDocByCloudid(byfollower_cid, rdata);
	aos_assert_r(byfollower_doc, false);
	if (!byfollower_doc)
	{
		rdata->setError() << "Missing the by follower doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString follower_cid;
	follower_cid << rdata->getCid();
	AosXmlTagPtr follower_doc = AosDocClientObj::getDocClient()->getDocByCloudid(follower_cid, rdata);
	aos_assert_r(follower_doc, false);
	if (!follower_doc)
	{
		rdata->setError() << "Missing the follower doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//bool rst = AosMicroblog::getSelf()->removeFollower(follower_doc, byfollower_doc, rdata);
	bool rst = AosMicroblog::getSelf()->removeFollower(byfollower_cid, rdata);
	aos_assert_r(rst, false);
	if (!rst)
	{
		rdata->setError() << "Faild to remove follower";
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

