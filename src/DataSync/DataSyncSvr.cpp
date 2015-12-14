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
// 10/13/2010: Created by Chen Ding
// 2014/01/24: Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEServer/DataSyncSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "HealthCheck/HealthChecker.h"
#include "HealthCheck/HealthCheckObj.h"


AosDataSyncSvr::AosDataSyncSvr()
{
}


AosDataSyncSvr::~AosDataSyncSvr()
{
}


AosNetReqProcPtr	
AosDataSyncSvr::clone()
{
	return OmnNew AosDataSyncSvr();
}


bool
AosDataSyncSvr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosDataSyncSvr::stop()
{
	return true;
}


bool
AosDataSyncSvr::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);

	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	u32 transid = req->getTransId();
	if (gAosLogLevel >= 2)
	{
		OmnScreen << "\nTo process request: (transid: " << transid
			<< "):\n" << req->getData() << endl;
	}

	AosXmlRc errcode = eAosXmlInt_General;
	if (mIsStopping)
	{
		sendResp(req, errcode, "Server is stopping", "");
		return false;
	}

	OmnString errmsg;
	char *data = req->getData();

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(data, "");
	AosXmlTagPtr child;
	if (!root || !(child = root->getFirstChild()))
	{
		errmsg = "Failed to parse the request!";
		OmnAlarm << errmsg << enderr;
		OmnScreen << data << endl;
		sendResp(req, errcode, errmsg, "");
		return false;
	}

	mOperation = AosDataSyncOpr::toEnum(child->getAttrStr("operation"));
	sgHealthChecker->addEntry(transid);
	bool rslt = procReq(req, root, child);
	sgHealthChecker->removeEntry(transid);
	return rslt;
}


bool
AosDataSyncSvr::procReq(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root)
{
	switch (mOperation)
	{
	case 
	}

	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg = "Unrecognized operation: ";
	errmsg << mOperation;
	OmnAlarm << errmsg << enderr;
	sendResp(req, errcode, errmsg, "");
	return true;
}


void
AosDataSyncSvr::sendResp(
		const AosWebRequestPtr &req, 
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents) 
{
	OmnString resp = "<status ";
	resp << " error=\"";
	if (errcode == eAosXmlInt_Ok) resp << "false\" code=\"200\"/>";
	else resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";

	if (contents != "" ) resp << contents;

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << req->getTransId()
			<< "):\n" << resp << endl;
	}
	req->sendResponse(resp);
}


#endif
