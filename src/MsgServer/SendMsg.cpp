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
#include "MsgServer/SendMsg.h"

#include "MsgServer/MsgReqProc.h"
#include "MsgServer/UserConnMgr.h"

AosSendMsg::AosSendMsg(const bool rflag)
:
AosMsgRequestProc(AOSMSPROC_SENDMSG, AosMsgReqid::eSendMsg, rflag)
{
}

bool 
AosSendMsg::proc(
		const AosXmlTagPtr &request,
		const AosRundataPtr &rdata)
{
	//request format:
	//<request operation="sendmsg">
	//	<objdef>
	//  	<Contents from="xxxx" to="xxxx" zky_port="xxxx" zky_account="xxxx">
	//  		<msg>xxxx</msg>
	//      </Contents>
	//  </objdef>
	//</request>

	aos_assert_rr(request, rdata, false);
	AosXmlTagPtr objdef = request->getFirstChild("objdef");
	aos_assert_rr(objdef, rdata, false);
	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	aos_assert_rr(contents, rdata, false);
	OmnString recver_cid = contents->getAttrStr("to", "");
	aos_assert_rr(recver_cid != "", rdata, false);
	OmnString from = contents->getAttrStr("from", "");
	aos_assert_rr(from != "", rdata, false);
	//OmnString sender_cid = rdata->getCid();
	//aos_assert_rr(sender_cid != "", rdata, false);
	//aos_assert_rr(sender_cid == from, rdata, false);


	AosSysUserPtr user;
	bool wait = false;
	AosUserConnMgr::getSelf()->getSysUser(recver_cid, user, wait);
	aos_assert_rr(user, rdata, false);
	if (user->getStatus() == AosSysUser::eOnline && wait)
	{
		OmnString ctns;
		OmnString status = "<status type=\"message\"/>";
		ctns << status << contents->toString();
		OmnTcpClientPtr conn = user->getConnection();
		OmnString callback = user->getCallBack();
		AosMsgReqProc::sendClientResp(conn, callback, ctns);
	}
	else
	{
		user->sendMsg(contents->toString());
	}
	rdata->setOk();
	return true;
}

