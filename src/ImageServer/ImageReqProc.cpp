////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// ImageComplier  
//
// Modification History:
// 05/18/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "ImageServer/ImageReqProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/XmlRc.h"


extern int gAosLogLevel;

AosImageReqProc::AosImageReqProc()
:
mLock(OmnNew OmnMutex()),
mIsStopping(false)
{
}


AosImageReqProc::~AosImageReqProc()
{
}


bool
AosImageReqProc::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosImageReqProc::stop()
{
	OmnScreen << "AosImageReqProc is stopping!" << endl;
	mIsStopping = false;
	return true;
}


bool			
AosImageReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;

	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);
	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	mConn = conn;

	// Requests are in the form:
	// 	<request operation="xxx" .../>
	AosImageReqOpr::E opr = AosImageReqOpr::toEnum(req->getAttrStr(AOSTAG_OPERATION));
	switch (opr)
	{
	case AosImageReqOpr::eHeartbeat:
		 procHeartbeat(req);
		 break;

	default:
		 OmnAlarm << "Unrecognized request: " << req->toString() << enderr;
		 sendReqponse(req, eAosXmlInt_General, "Unrecognized request", "");
		 return false;
	}
	return true;
}


bool
AosImageREqProc::procHeartbeat(const AosXmlTagPtr &req)
{
	// Just send a response
	
	sendResponse(req, eAosXmlInt_Ok, "", "");
}


void
AosImageReqProc::sendResponse(
		const AosWebRequestPtr &req,
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents)
{
	OmnString resp = "<status error=\"";
	if (errcode == eAosXmlInt_Ok) resp << "false\" code=\"200\"/>";
	else resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";

	if (contents != "" ) resp << contents;

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << mReq->getTransId()
			<< "): " << resp << endl;
	}
	req->sendResponse(resp);
}


AosNetReqProcPtr
AosImageReqProc::clone()
{

	return OmnNew AosImageReqProc();
}
