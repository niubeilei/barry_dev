//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
////////////////////////////////////////////////////////////////////////////
#include "JimoAgentMonitor/RecveJimoAgentProc.h"
#include "JimoAgentMonitor/JimoAgentMonitor.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"

AosRecveJimoAgentProc::AosRecveJimoAgentProc()
{
}


AosRecveJimoAgentProc::~AosRecveJimoAgentProc()
{
}


AosNetReqProcPtr
AosRecveJimoAgentProc::clone()
{
	return OmnNew AosRecveJimoAgentProc();
}

bool
AosRecveJimoAgentProc::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr client = buff->getConn();
	AosWebRequestPtr req = OmnNew AosWebRequest(client, buff);
	aos_assert_r(req, false);
	char *data = req->getData();
	OmnString str;
	str << data;
	OmnScreen << "jozhi: recve" << str << endl;
	return procCommand(str, client);
}


bool
AosRecveJimoAgentProc::config(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosRecveJimoAgentProc::procCommand(const OmnString &str, const OmnTcpClientPtr &client)
{
	AosXmlTagPtr req = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(req, false);
	OmnString opr = req->getAttrStr("opr");
	if (opr == "report")
	{
		OmnIpAddr ip_addr = client->getRemoteIpAddr();
		OmnString addr = ip_addr.toString();
		OmnString report;
		AosJimoAgentMonitor::getSelf()->report(report, addr);
	}
	else if (opr == "heartbeat")
	{
		OmnIpAddr ip_addr = client->getRemoteIpAddr();
		OmnString addr = ip_addr.toString();
		AosJimoAgentMonitor::getSelf()->report(addr);
	}
	else
	{
		return true;
	}
	return true;
}
