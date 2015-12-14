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
// Modification History:
// 02/22/2012	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/SendMsgByAccount.h"

#include "MsgService/MsgService.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"

AosSendMsgByAccount::AosSendMsgByAccount(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SENDMSGBYACCOUNT,
		AosSeReqid::eSendMsgByAccount, rflag)
{
}


bool 
AosSendMsgByAccount::proc(const AosRundataPtr &rdata)
{
	/*
	// This function is called when a user wants to send an instant message
	// to another user. 
	// <request reqid = "sendmsgbyacct">
	// 	<objdef>
	// 		<Contents zky_account="xxxx" zky_port="xxxx" from="xxxx" to="xxxx">
	// 			<msg>xxxx</msg>
	//  	</Contents>
	//  </objdef>
	//</request>
	//
	//"zky_account": user define ,is not empty
	//"zky_port" user define
	//"from" the MsgService to set
	//"to" the MsgService to set
	//
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
		rdata->setError() << "Missing contents!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString account = contents->getAttrStr("zky_account", "");
	if (account == "")
	{
		rdata->setError() << "Missing account objid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString port = contents->getAttrStr("zky_port", "");
	if (port == "")
	{
		rdata->setError() << "Missing port";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString msg_type = contents->getAttrStr("zky_msg_type", "online");
	bool send = AosMsgService::getSelf()->sendMsgByAccount(msg_type, account, contents, rdata);
	aos_assert_rr(send, rdata, false);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

