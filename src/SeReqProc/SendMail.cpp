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
// The torturer is in SengTorturer/TesterSendMailNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/SendMail.h"

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
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Python/Pyemail.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosSendMail::AosSendMail(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SENDMAIL, 
		AosSeReqid::eSendMail, rflag)
{
}


bool 
AosSendMail::proc(const AosRundataPtr &rdata)
{
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr child = rootchild->getFirstChild("command");
	if (child)
	{
		child = child->getFirstChild();
	}

	bool rslt = false;
	//bool rslt = AosPyemail::getSelf()->sendmail(child, rdata);
	if (!rslt)
	{
		rdata->setError() << "Faild send email";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

