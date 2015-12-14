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
// The torturer is in SengTorturer/TesterSendImMgrNew.cpp
//   
//
// Modification History:
// 06/21/2011	Created by Michael Yang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/SendInstantMsg.h"

#include "InstantMsg/IMManager.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"

AosSendInstantMsg::AosSendInstantMsg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SEND_INSTANTMSG,
		AosSeReqid::eSendInstantMsg, rflag)
{
}


bool 
AosSendInstantMsg::proc(const AosRundataPtr &rdata)
{
	/*
	// This function is called when a user wants to send an instant message
	// to another user. 
	// <request reqid = "sendimmsg">
	// 	<objdef>
	// 		<Contents>
	// 			<msgtype>chatmessage|chatmessage_group</msgtype>
	//  		<msg>xxxx</msg>
	//  		<toid>xxxx</toid>
	//  	</Contents>
	//  </objdef>
	//</request>
	//
	//<request reqid = "sendinstantmsg">
	//	<objdef>
	//		<Contents msgtype="xxxx" toid="xxxx" from="xxxx">
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

	OmnString from = contents->getNodeText("from");
	if (from == "")
	{
		rdata->setError() << "Missing from";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString sender_cid = rdata->getCid();
	if (sender_cid != from)
	{
		rdata->setError() << "login exception";
		//OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString msg = contents->getNodeText("msg");
	if (msg == "")
	{
		rdata->setError() << "Missing message";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString toid = contents->getNodeText("toid");
	if (toid == "")
	{
		rdata->setError() << "Missing friend cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString msgtype = contents->getNodeText("msgtype");
	aos_assert_rr(msgtype != "", rdata, false);

	bool rslt = false;
	if (msgtype == "chatmessage")
	{
		rslt = AosIMManager::getSelf()->sendInstantMsg(msgtype, "", toid, msg, rdata);
	}
	else if (msgtype == "chatmessage_group")
	{
		rslt = AosIMManager::getSelf()->sendTalkGroupMsg(msgtype, toid, msg, rdata);
	}
	aos_assert_r(rslt, false);
	if (!rslt)
	{
		rdata->setError() << "Faild to send Instant message";
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

