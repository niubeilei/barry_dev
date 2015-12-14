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
#include "SeReqProc/RemoveMicroBlogMsg.h"

#include "Microblog/Microblog.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"


AosRemoveMicroBlogMsg::AosRemoveMicroBlogMsg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REMOVE_MBLOGMSG, 
		AosSeReqid::eRemoveMBlogMsg, rflag)
{
}


bool 
AosRemoveMicroBlogMsg::proc(const AosRundataPtr &rdata)
{
	/*
	//send msg to all friend
	//<request reqid="removemblogmsg">
	//	<objdef>
	//		<Contents>
	//			<msg_docid>xxxx</msg_docid>
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

	OmnString  msg_docid = contents->getNodeText("msg_docid");
	if (msg_docid == "")
	{
		rdata->setError() << "Missing microblog message docid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;

	}

	bool rslt = AosMicroblog::getSelf()->removeMBlogMsg(msg_docid, rdata);
	if (!rslt)
	{
		rdata->setError() << "Faild to remove microblog message";
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

