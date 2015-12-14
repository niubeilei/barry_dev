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
// The torturer is in SengTorturer/TesterDataSyncNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/DataSync.h"

#include "Actions/ActSeqno.h"
#include "SearchEngine/DocServerCb.h"
#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SeSysLog/Ptrs.h"
#include "SeSysLog/SeSysLog.h"
#include "SEModules/ImgProc.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

//static AosSeSysLog sgSeSysLog;

AosDataSync::AosDataSync(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_DATASYNC, 
		AosSeReqid::eDataSync, rflag)
{
}


bool 
AosDataSync::proc(const AosRundataPtr &rdata)
{
	//<request ....>
	//	<item name=\"operation\">serverCmd</item>"
	//	<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
	//	<command><cmd opr='datasync' /></command>"
	//	<objdef> << log << "</objdef>
	//</request>
	
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr cmd = rootchild->getFirstChild("command");
	if (!cmd)
	{
		cmd = cmd->getFirstChild();
	}

	OmnString errmsg;
	OmnString contents;
	OmnString transid = root->getAttrStr("transid");
	OmnString requestid = root->getAttrStr("requestid");
	contents << "<Contents transid=\"" << transid
			 << "\" requestid=\"" << requestid << "\"/>";

	if (AosSeReqProc::getIsStopping())
	{
		rdata->setError() << "Server is stopping";
		AOSLOG_LEAVE(rdata);
		return true;
	}

	AosXmlTagPtr xmldoc = rootchild->getFirstChild("objdef");
	if (!xmldoc)
	{
		errmsg = "Missing the object to be dataSync";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr doc = xmldoc->getFirstChild();
	if (!doc)
	{
		errmsg = "Missing the object to be dataSync(1)";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	//bool rs = sgSeSysLog.recoverSystem(root, doc, rdata);
	
	bool rs = false;
	if (!rs)
	{
		rdata->setError();
		AOSLOG_LEAVE(rdata);
		return false;
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

