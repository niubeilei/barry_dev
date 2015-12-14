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
// 06/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocServer/DocSvrConnMgr.h"

#include "ValueSel/ValueRslt.h"
#include "DocServer/DocSvrProc.h"
#include "DocServer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"

// Ketty 2012/10/24
#if 0
extern int gAosLogLevel;
AosDocSvrConnMgr::AosDocSvrConnMgr()
{
}


AosDocSvrConnMgr::~AosDocSvrConnMgr()
{
}


bool
AosDocSvrConnMgr::proc(
		const AosBigTransPtr &trans,
		const AosBuffPtr &request,
		const AosRundataPtr &rdata) 
{
	if (!request || !trans)
	{
		rdata->setError() << "Failed to get the request!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResp(rdata->getErrcode(), rdata->getErrmsg(), rdata, trans);
		return false;
	}
	
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(request->data(), "" AosMemoryCheckerArgs);
	//AosXmlTagPtr xml = request;//->getFirstChild();
	if (!xml)
	{
		rdata->setError() << "Failed to get the request!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResp(rdata->getErrcode(), rdata->getErrmsg(), rdata, trans);
		return false;
	}

	if (gAosLogLevel >= 3)
	{
		OmnScreen << "Process request: " << xml->toString() << endl;
	}

	AosDocSvrProcPtr proc = AosDocSvrProc::getProc(xml->getAttrStr(AOSTAG_TYPE));
	if (!proc)
	{
		rdata->setError() << "Failed retrieving the proc: " 
			<< xml->getAttrStr(AOSTAG_TYPE);
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResp(rdata->getErrcode(), rdata->getErrmsg(), rdata, trans);
		return false;
	}

	bool rslt = proc->proc(rdata, xml, trans);
	if (!rslt)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResp(rdata->getErrcode(), rdata->getErrmsg(), rdata, trans);
		return false;
	}

	sendResp(rdata->getErrcode(), rdata->getErrmsg(), rdata, trans);
	return true;

}


void
AosDocSvrConnMgr::sendResp(
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const AosRundataPtr &rdata,
		const AosBigTransPtr &trans)
{
	// Ketty 2012/10/24
	/*
	OmnString resp = "<response><status error=\"";
	if (errcode == eAosXmlInt_Ok)
	{
		resp << "false\" code=\"200\" />";
	}
	else
	{
		resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";
	}

	if (rdata->getContents() != "")
	{
		resp << rdata->getContents();
	}
	resp << "</response>";
	if (gAosLogLevel >=3)
	{
		OmnScreen << "Send response: "<< resp << endl;
	}
	rdata->setContents(resp);
	*/
	
	aos_assert(mTransServer);
	
	OmnString resp_cont = rdata->getContents();
	AosBuffPtr resp_buff = OmnNew AosBuff(resp_cont.length() + 100, 0 AosMemoryCheckerArgs);
	
	if (errcode == eAosXmlInt_Ok)
	{
		resp_buff->setU32(eError);
		resp_buff->setOmnStr(errmsg);
	}
	else
	{
		resp_buff->setU32(eOk);
	}
	
	resp_buff->setOmnStr(resp_cont);
	mTransServer->sendResp(trans, resp_buff);
}

void
AosDocSvrConnMgr::setTransServer(const AosTransModuleSvrPtr &trans_server)
{
	mTransServer = trans_server;
}
#endif
