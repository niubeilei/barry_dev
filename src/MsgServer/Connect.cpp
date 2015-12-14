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
#include "MsgServer/Connect.h"

#include "MsgServer/MsgReqProc.h"
#include "MsgServer/UserConnMgr.h"

AosConnect::AosConnect(const bool rflag)
:
AosMsgRequestProc(AOSMSPROC_CONNECT, AosMsgReqid::eConnect, rflag)
{
}


bool 
AosConnect::proc(
		const AosXmlTagPtr &request,
		const AosRundataPtr &rdata)
{
	// A request is received from a user to retrieve messages. 
	// <request operation="connect" cid="xxxx"/>
	AosXmlTagPtr root = rdata->getRequestRoot();
	aos_assert_rr(root, rdata, false);

	OmnString cid = root->getAttrStr("cid", "");
	aos_assert_rr(cid != "", rdata, false);

	OmnString callback = root->getAttrStr("callback", "");
	aos_assert_rr(callback != "", rdata, false);

	AosWebRequestPtr req = rdata->getReq();
	OmnTcpClientPtr conn = req->getClient();

	OmnString ssid = rdata->getSsid();
	bool login_check = true;
	//login_check = AosSengAdmin::getSelf()->checkLogin(100, cid, ssid);
	if (!login_check)
	{
	    OmnString cnts;
	    cnts << "<status type=\"exception\">login exception</status>";
	    AosMsgReqProc::sendClientResp(conn, callback, cnts);
	    return true;
	}

	AosSysUserPtr user;
	bool wait;
	AosUserConnMgr::getSelf()->getSysUser(cid, user, wait);
	aos_assert_rr(user, rdata, false);
	OmnTcpClientPtr userconn = user->getConnection();
	if (userconn)
	{
		int reqsock = conn->getSock();
		int usrsock = userconn->getSock();
		if (reqsock != usrsock && usrsock != -1)
		{
			OmnString usercallback = user->getCallBack();
			OmnString cnts;
			cnts << "<status type=\"exception\">"
				<< "Your account is logined in another place"
				<< "</status>";
			AosMsgReqProc::sendClientResp(userconn, usercallback, cnts);
			userconn->closeConn();
		}
OmnScreen << "usr socket: " << usrsock << endl;
OmnScreen << "req socket: " << reqsock << endl;
	}
	user->updateOnlineInfo(conn, callback);
	OmnString cnts;
	cnts << "<status type=\"connect\">"
		 << "Connect to msg server success"
		 << "</status>";
	AosMsgReqProc::sendClientResp(conn, callback, cnts);
	rdata->setOk();
	return true;
}
