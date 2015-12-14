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
// 06/15/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "MsgServer/GetMessage.h"

#include "MsgServer/MsgReqProc.h"
#include "MsgServer/UserConnMgr.h"

AosGetMessage::AosGetMessage(const bool rflag)
:
AosMsgRequestProc(AOSMSPROC_GETMESSAGE, AosMsgReqid::eGetMessage, rflag)
{
}


bool 
AosGetMessage::proc(
		const AosXmlTagPtr &request,
		const AosRundataPtr &rdata)
{
	// A request is received from a user to retrieve messages. 
	// <request operation="getmessage" cid="xxxx"/>
	AosXmlTagPtr root = rdata->getRequestRoot();
	aos_assert_rr(root, rdata, false);
	OmnString cid = root->getAttrStr("cid", "");
	aos_assert_rr(cid != "", rdata, false);
	OmnString callback = root->getAttrStr("callback", "");
	aos_assert_rr(callback != "", rdata, false);

	AosWebRequestPtr req = rdata->getReq();
	OmnTcpClientPtr conn = req->getClient();
	AosSysUserPtr user;
	bool wait;
	AosUserConnMgr::getSelf()->getSysUser(cid, user, wait);
	aos_assert_rr(user, rdata, false);
	user->updateOnlineInfo(conn, callback);
	OmnString ctns= "";
	user->readMsg(ctns);
	if (ctns!= "")
	{
		//1. if readMsg success send it
		ctns <<"<status type=\"message\"/>"
			 << ctns;
		AosMsgReqProc::sendClientResp(conn, callback, ctns);
	}
	else
	{
		//2. if readMsg is empty return;
		user->setWait(true);
	}
	rdata->setOk();
	return true;
}
