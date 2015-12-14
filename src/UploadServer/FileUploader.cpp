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
// 12/18/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UploadServer/FileUploader.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

AosFileUploader::AosFileUploader()
{
}


AosFileUploader::~AosFileUploader()
{
}


bool
AosFileUploader::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosFileUploader::stop()
{
	OmnScreen << "AosFileUploader is stopping!" << endl;
	mIsStopping = true;
	return true;
}


bool
AosFileUploader::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);
	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	u32 transid = req->getTransId();
	if (gAosLogLevel >= 2)
	{
		cout << hex << "<0x" << pthread_self() << dec 
			<< ":" << __FILE__ << ":" << __LINE__
			<< "> Process request: (transid: " << req->getTransId()
			<< ")\n" << req->getData() << endl;
	}

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(buff, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child;

	if (mIsStopping)
	{
		OmnAlarm << mRundata->getErrmsg() << enderr;
		sendResp(mRundata);
		return false;
	}

	if (!root || !(child = root->getFirstChild()))
	{
		OmnAlarm << mRundata->getErrmsg() << ": " << data << enderr;
		sendResp(mRundata);
		return false;
	}

	return true;
}


void
AosFileUploader::sendResp()
{

	for(u32 i=0; i<mRundata->getSendCookies().size(); i++)
	{
		resp << "<cookie>" << mRundata->getSendCookies()[i] << "</cookie>";	
	}
	resp << "</zky_cookies>";
	//------------------- Ketty End-------------

	req->sendResponse(resp);
	mSession = 0;
	mUrldocDocid = 0;
}


