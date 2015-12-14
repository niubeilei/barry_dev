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
// HtmlComplier  
//
// Modification History:
// 06/15/2011 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "MsgServer/MsgServer.h"
#include "MsgServer/MsgRequestProc.h"
#include "SingletonClass/SingletonImpl.cpp"

OmnSingletonImpl(AosMsgServerSingleton,
				 AosMsgServer,
				 AosMsgServerSelf,
				 "AosMsgServer");

extern int gAosLogLevel;

AosMsgServer::AosMsgServer()
{
	//Jozhi 2013/03/25
	//AosTransProcPtr thisptr(this, false);
	//mTransServer = OmnNew AosTransModuleSvr(AosTransModule::eInstantMessage, thisptr);
}


AosMsgServer::~AosMsgServer()
{
}

bool
AosMsgServer::start()
{
	return true;
}

bool
AosMsgServer::stop()
{
	return true;
}

bool
AosMsgServer::config(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosMsgServer::procRequest(
		const AosXmlTagPtr &root,
		const AosRundataPtr &rdata)
{
	if (!root)
	{
		rdata->setError() << "Failed to get the request!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

OmnScreen << root->toString() << endl;
	OmnString operation = root->getAttrStr("operation", "");
	aos_assert_r(operation != "", false);
	if (operation == "")
	{
		rdata->setError() << "Faild to get the operation";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosMsgRequestProcPtr proc = AosMsgRequestProc::getProc(operation);
	aos_assert_r(proc, false);
	if (!proc)
	{
		rdata->setError() << "Unrecognized operation: " << operation;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!proc->proc(root, rdata))
	{
		rdata->setError() << "Failed to proc request!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}



//Jozhi 2013/03/25
/*
bool
AosMsgServer::proc(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)

{
	//this function is handle the SEServer request
	//request format is: 
	//<request operation="xxx">
	//	<objdef>
	//		<Contents>
	//			...
	//			...
	//		</Contents>
	//	</objdef>
	//</request>
	AosXmlTagPtr root = trans_doc;
	AosXmlRc errcode = eAosXmlInt_General;
	if (!root)
	{
		rdata->setError() << "Failed to get the request!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResp(errcode, rdata->getErrmsg(), rdata);
		return false;
	}

OmnScreen << root->toString() << endl;
	OmnString operation = root->getAttrStr("operation", "");
	aos_assert_r(operation != "", false);
	if (operation == "")
	{
		rdata->setError() << "Faild to get the operation";
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResp(errcode, rdata->getErrmsg(), rdata);
		return false;
	}
	AosMsgRequestProcPtr proc = AosMsgRequestProc::getProc(operation);
	aos_assert_r(proc, false);
	if (!proc)
	{
		rdata->setError() << "Unrecognized operation: " << operation;
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResp(errcode, rdata->getErrmsg(), rdata);
		return false;
	}

	if (!proc->proc(root, rdata))
	{
		rdata->setError() << "Failed to proc request!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResp(errcode, rdata->getErrmsg(), rdata);
		return false;
	}
	errcode = eAosXmlInt_Ok;
	sendResp(errcode, "", rdata);
	return true;
}


void
AosMsgServer::sendResp(
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const AosRundataPtr &rdata)
{
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
	if (gAosLogLevel >= 2)
	{
		//OmnScreen << "Send response: (transid: " << rdata->getTrans()->getTransId()
		//	<< "): " << resp << endl;
	}
	rdata->setContents(resp);
}


bool
AosMsgServer::proc(
		const AosBuffTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosMsgServer::proc(
		const AosTinyTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
*/


