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
#include "SeReqProc/RemoveSender.h"

#include "MsgService/MsgService.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"

AosRemoveSender::AosRemoveSender(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REMOVESENDER,
		AosSeReqid::eRemoveSender, rflag)
{
}


bool 
AosRemoveSender::proc(const AosRundataPtr &rdata)
{
	/*
	// This function is called when a user wants to send an instant message
	// to another user. 
	// <request reqid = "removesender">
	// 	<objdef>
	// 		<Contents>
	// 			<sender>xxxx</sender>
	// 			<account>xxxx</account>
	//  	</Contents>
	//  </objdef>
	//</request>
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
	OmnString account = contents->getNodeText("account");
	if (account == "")
	{
		rdata->setError() << "Missing account";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString sender = contents->getNodeText("sender");
	if (sender == "")
	{
		rdata->setError() << "Missing sender";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	//check sender is our system user
	bool send = AosMsgService::getSelf()->removeSender(account, rdata);
	aos_assert_rr(send, rdata, false);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

