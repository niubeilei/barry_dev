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
// 05/26/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SQLInterface/SQLInterface.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


AosSQLInterface::AosSQLInterface()
:
mLock(OmnNew OmnMutex())
{
	AosNetReqProcPtr thisptr(this, false);
	mRundata = OmnNew AosRundata(thisptr);
}


AosSQLInterface::~AosSQLInterface()
{
}


AosNetReqProcPtr	
AosSQLInterface::clone()
{
	return OmnNew AosSQLInterface();
}


bool
AosSQLInterface::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosSQLInterface::stop()
{
	return true;
}


bool
AosSQLInterface::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);
	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	u32 transid = req->getTransId();
	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Process request: (transid: " << req->getTransId()
			<< ")\n" << req->getData() << endl;
	}

	char *data = req->getData();
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(data, "");
	AosXmlTagPtr child;

	mRundata->resetForReuse(req);
	mRundata->setRequestRoot(root);

	if (!root || !(child = root->getFirstChild()))
	{
		errmsg = "Failed to parse the request!";
		OmnAlarm << errmsg << enderr;
		OmnScreen << data << endl;
		mRundata->setError(eAosXmlInt_General) << errmsg;
		sendResp(mRundata);
		return false;
	}

	mTransId = child->getAttrStr("transid");
	mRundata->setReceivedDoc(root);
	mSession = AosSessionMgr::getSelf()->getSession1(
				mSsid, loginvpd, mRundata->getErrcode(), mRundata->getErrmsg());
	if (mSession)
	{
		mUserid = mSession->getUserid();
		mRundata->setUserid(mUserid);
		mRundata->setSession(mSession);
	}

	mOperation = child->getAttrStr("operation");

	// Chen Ding, 01/27/2012
	// Not used anymore.
	// mRundata->setRecvedOperation(mOperation);

	return procReq(req, root, child);
}


bool
AosSQLInterface::procReq(
			const AosWebRequestPtr &req, 
			const AosXmlTagPtr &root,
			const AosXmlTagPtr &child)
{
	AosSqlReqProcPtr proc = AosSqlReqid::getProc(root->getAttrStr("reqid"));
	if (!proc)
	{
		mRundata->setError() << "Failed to retrieve the proc";
		OmnAlarm << mRundata->getErrmsg() << enderr;
		sendResp(mRundata);
		return false;
	}

	proc->proc(mRundata);
	sendResp(mRundata);
	return true;
}


void
AosSQLInterface::sendResp(
		const AosWebRequestPtr &req, 
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents) 
{
	OmnString resp = "<status ";

	resp << "session=\"" << mClientSsid<< "\" ";

	resp << " error=\"";
	if (errcode == eAosXmlInt_Ok) resp << "false\" code=\"200\"/>";
	else resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";

	if (contents != "" ) resp << contents;

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << req->getTransId()
			<< ")\n" << resp << endl;
	}

	req->sendResponse(resp);
	mSession = 0;
}


