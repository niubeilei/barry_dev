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
#include "MsgServer/DisConnect.h"

#include "MsgServer/MsgReqProc.h"
#include "MsgServer/UserConnMgr.h"

AosDisConnect::AosDisConnect(const bool rflag)
:
AosMsgRequestProc(AOSMSPROC_DISCONNECT, AosMsgReqid::eDisConnect, rflag)
{
}


bool 
AosDisConnect::proc(
		const AosXmlTagPtr &request,
		const AosRundataPtr &rdata)
{
	//<request operation="disconnect" cid="xxxx"/>
	aos_assert_rr(request, rdata, false);
	OmnString cid = request->getAttrStr("cid", "");
	aos_assert_rr(cid != "", rdata, false);
	//OmnString sender_cid = rdata->getCid();
	//aos_assert_rr(sender_cid != "", rdata, false);
	//aos_assert_rr(sender_cid == cid, rdata, false);

	AosSysUserPtr user;
	bool wait = false;
	AosUserConnMgr::getSelf()->getSysUser(cid, user, wait);
	aos_assert_rr(user, rdata, false);
	if (user->getStatus() == AosSysUser::eOnline && wait)
	{
		OmnTcpClientPtr userconn = user->getConnection();
		OmnString usercallback = user->getCallBack();
		OmnString cnts;
		cnts << "<status type=\"disconnect\">"
			 << "You disconnect to the message server"
			 << "</status>";
		AosMsgReqProc::sendClientResp(userconn, usercallback, cnts);
		userconn->closeConn();
	}
	user->setStatus(AosSysUser::eOffline);
OmnScreen << "**********[" << cid << "] Disconnect **********" << endl;
	rdata->setOk();
	return true;
}
