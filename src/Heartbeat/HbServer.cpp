////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HbServer.cpp
// Description:
//	A HbServer maintains a TCP Server connection. Anyone that wants
//	to monitor this unit can make a connection to this server.
//	
//	The server maintains a list of connections, one for each peer.
//	The peer can send a heartbeat message through its connection
//	to this server. When it receives a heartbeat request, it 
//	checks with the ThreadMgr to make sure all threads are running
//	ok. It bounces the status back to the sender. This server does not
//	monitor the remote peer status, but it does monitor the connection.
//	
//	If a broken connection is detected, it simply removes the connection
//	from its list. It does not make decision regarding whether the
// 	remote peer is ok or not, which should be done by OmnHbMonitor.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Heartbeat/HbServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEUtil/Ports.h"
#include "XmlUtil/XmlTag.h"


const OmnString sgLocalAddr = "0.0.0.0";
const OmnString sgServerName = "HeartbeatMgr";
const int sgMaxProcTime = 1000;
const int sgMaxConns = 1000;
const OmnString sgDeliminator = "first_four_high";

OmnHbServer::OmnHbServer()
{
	start();
}


OmnHbServer::~OmnHbServer()
{
}


bool				
OmnHbServer::procRequest(const OmnConnBuffPtr &req)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);
	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	if (gAosLogLevel >= 2)
	{
		cout << hex << "<0x" << pthread_self() << dec 
			<< ":" << __FILE__ << ":" << __LINE__
			<< "> Process request: (transid: " << req->getTransId()
			<< ")\n" << req->getData() << endl;
	}

	char *data = req->getData();
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(buff, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child;
	
	if (!root || !(child = root->getFirstChild()))
	{
		OmnAlarm << mRundata->getErrmsg() << ": " << data << enderr;
		sendResp();
		return false;
	}

	OmnString type = child->getAttrStr("type");
	if (type == AOSHBMSG_HEARTBEAT)
	{
		procHeartbeat(child);
	}

	OmnAlarm << "Message not recognized: " << root->toString() << enderr;
	sendResp(req);
	return true;
}


AosNetReqProcPtr	
OmnHbServer::clone()
{
	return OmnNew OmnHbServer();
}


bool
OmnHbServer::start()
{
	AosNetReqProcPtr thisptr(this, false);
	AosReqDistr reqDistr(thisptr);
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	AosXmlTagPtr hbconf;
	if (conf)
	{
		hbconf = conf->getFirstChild("heartbeat");
	}

	if (hbconf)
	{
		if (!reqDistr.config(hbconf))
		{
			OmnAlarm << "Failed the configuration: " 
				<< OmnApp::getAppConfig()->toString() << enderr;
			exit(-1);
			return false;
		}
	}
	else
	{
		reqDistr.config(sgLocalAddr, AOSPORT_HEARTBEAT, sgServerName, 
				sgDeliminator, sgMaxProcTime, sgMaxConns);
	}

	reqDistr.start();
	
	return true;
} 


void
OmnHbServer::sendResp(
		const AosWebRequestPtr &req, 
		const OmnString &status,
		const OmnString &contents) 
{
	OmnString resp = "<response status=\"";
	resp << status << "\">" << contents << "</response>";

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << req->getTransId()
			<< ")\n" << resp << endl;
	}

	req->sendResponse(resp);
}

