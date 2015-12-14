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
#include "SeReqProc/SendMicroBlogMsg.h"

#include "Microblog/Microblog.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"


AosSendMicroBlogMsg::AosSendMicroBlogMsg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SEND_MBLOGMSG, 
		AosSeReqid::eSendMicroBlogMsg, rflag)
{
}


bool 
AosSendMicroBlogMsg::proc(const AosRundataPtr &rdata)
{
	/*
	//send msg to all friend
	//<request reqid="sendmblogmsg">
	//	<objdef>
	//		<Contents>
	//			<type>private</type>
	//			<follower_cid>xxx</follower_cid>
	//			<msg>xxxx</msg>
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
		rdata->setError() << "Missing the friend object!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString type = contents->getNodeText("type");
	aos_assert_rr(type != "", rdata, false);
	if (type ==  "private")
	{
		OmnString msg = contents->getNodeText("msg");
		aos_assert_rr(msg != "", rdata, false);
		OmnString follower_cid = contents->getNodeText("follower_cid");
		aos_assert_rr(follower_cid != "", rdata, false);
		bool rslt = AosMicroblog::getSelf()->sendPrivateMsg(msg, follower_cid, rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	else if (type == "circulate")
	{
		AosXmlTagPtr msg = contents->getFirstChild("msg");
		OmnString follower_cid = contents->getNodeText("follower_cid");
		aos_assert_rr(follower_cid != "", rdata, false);
		bool rslt = AosMicroblog::getSelf()->sendCirculationMsg(msg, follower_cid, rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	else
	{
		OmnString msg = contents->getNodeText("msg");
		aos_assert_rr(msg != "", rdata, false);
		bool rslt = AosMicroblog::getSelf()->sendMBlogMsg(msg, rdata);
		aos_assert_rr(rslt, rdata, false);
	}

	OmnString ctns;
	ctns << "<Contents/>";
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

