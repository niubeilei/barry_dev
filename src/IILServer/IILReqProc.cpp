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
// 11/21/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILServer/IILReqProc.h"
#include "TransUil/TransProc.h"
#include "TransServer/TransMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/XmlRc.h"
#include "SEUtil/SeXmlParser.h"

//-------------For Test---------------//
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

extern int shmid;
extern char *firstAddr;
//-------------Test End---------------//


extern int gAosLogLevel;

AosIILReqProc::AosIILReqProc()
:
mIsStopping(false)
{
	// mIILProc = OmnNew AosIILProc();
}


AosIILReqProc::~AosIILReqProc()
{
}


AosNetReqProcPtr	
AosIILReqProc::clone()
{
	return OmnNew AosIILReqProc();
}


bool
AosIILReqProc::config(const AosXmlTagPtr &config)
{
	//sgHealthChecker = OmnNew AosHealthChecker(0, eHealthCheckFreq, 
	//		eHealthCheckMaxAges, eHealthCheckMaxEntries);

	return true;
}


bool
AosIILReqProc::stop()
{
	return true;
}


bool
AosIILReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);

	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	if (gAosLogLevel >= 1)
	{
		OmnScreen << "Server: To process request: " << "(tid:"<< pthread_self()<<")\n" 
			<< buff->getDataLength() << ":" << conn->getRemotePort()
			<< ":" << req->getData() << endl;
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

	mOperation = AosTransReqId::toEnum(child->getAttrStr(AOSTAG_TRANS_OPERATION));
	switch (mOperation)
	{
	case AosTransReqId::eIILAddDoc:
		 return addDoc(req, conn, root);
		 
	case AosTransReqId::eRetrieveIIL:
	 	 return retrieveIIL(req, root);

	default:
		 errmsg = "Unrecognized request: ";
		 errmsg << mOperation;
		 sendResp(req, errcode, errmsg, "");
		 return false;
	}

	errmsg = "Internal error: ";
	errmsg << __FILE__ << ":" << __LINE__;
	OmnAlarm << errmsg << enderr;
	sendResp(req, errcode, errmsg, "");
	return false;
}


void
AosIILReqProc::sendResp(
		const AosWebRequestPtr &req, 
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents) 
{
	OmnString resp = "<response><status ";

	resp << " error=\"";
	if (errcode == eAosXmlInt_Ok || errcode == eAosXmlInt_Ack) resp << "false\" code=\"200\"/>";
	else resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";
	if (contents != "" ) resp << contents;

	resp << "</response>";

	if (gAosLogLevel >= 1)
	{
		OmnScreen << "Server: Send response:" << "(tid:"<< pthread_self() 
				  << ")\n" << resp << endl;
	}

	req->sendResponse(resp);
}


bool
AosIILReqProc::addDoc(
		const AosWebRequestPtr &req,
		const OmnTcpClientPtr  &conn,
		const AosXmlTagPtr &root)
{
	// This function does the following:
	// 1. Construct the transaction
	// 2. Check whether the transaction was finished. If yes, 
	//    send the response.
	// 3. Otherwise, create the transaction.
	// 
	// 'root' should be in the following format:
	// 	<request ...>
	// 		<AOSTAG_TRANSDEF .../>
	// 	</request>
	// Create the transaction
	//AosTransPtr trans = OmnNew AosTrans(transdef, mIILProc);
	AosTransPtr trans = OmnNew AosTrans(root);
	
	if (!trans)
	{
		OmnString errmsg = "Failed to create transaction: ";
		errmsg << root->toString();
		OmnAlarm << errmsg << enderr;
		sendResp(req, eAosXmlInt_General, errmsg, "");
		return false;
	}
	
	// Add the transaction
	AosXmlRc errcode;
	OmnString errmsg, contents;
	AosTransMgr::getSelf()->addTrans(trans, conn, errcode, errmsg, contents);

	// Send the acknowledgement(new or Retransmit) or finished
	sendResp(req, errcode, errmsg, contents);
	return true;
}


bool
AosIILReqProc::retrieveIIL(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root)
{
	// This function does the following:
	// 1. Retrieve the information
	// 2. Ask AosIILServer to retrieve the IIL
	// 3. Construct the response and return
	OmnNotImplementedYet;
	sendResp(req, eAosXmlInt_General, "Not implemented yet!", "");
	return false;
}


bool
AosIILReqProc::queryIIL(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root)
{
	OmnNotImplementedYet;
	sendResp(req, eAosXmlInt_General, "Not implemented yet!", "");
	return false;
}

