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
// The torturer is in SengTorturer/TesterRunSmartdocNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RunSmartdoc.h"

#include "API/AosApi.h"
#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SeReqProc/ResumeReq.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosRunSmartdoc::AosRunSmartdoc(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RUN_SMARTDOC, 
		AosSeReqid::eRunSmartdoc, rflag)
{
}


bool 
AosRunSmartdoc::proc(const AosRundataPtr &rdata)
{
	// root
	// 	<request>
	// 		<objdef>
	// 			<userdoc .../>
	// 		</objdef>
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();	//request
	if (!rootchild)
	{
		OmnAlarm << "Request incorrect!" << enderr;
		rdata->setError() << "Request incorrect!" ;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	// 2011/06/28 Tracy
	// Chen Ding, 01/21/2012
	// rdata->setRequestRoot(root);

	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	AosXmlTagPtr worker_doc;
	if (objdef)
	{
		worker_doc = objdef->getFirstChild();
	}

	// Chen Ding, 2013/05/31
	// The user (requester) wants to run a smart doc. It normally
	// requires two XML docs: (1) the smart doc, and (2) the 
	// worker doc. 
	//
	// There are several ways to identify the smart doc:
	// 1. Through 'args', i.e., args="objid:smartdoc_objid, ...",
	// 2. Through 'worker_doc', with the attribute AOSTAG_SMARTDOC_OBJID. 
	// 3. The 'worker_doc' itself is a smart doc. In this case, there 
	//    is no worker doc.
	//
	// Retrieve the smartdoc objid, which is stored in 'args'.
	// 'args' is in the form:
	//      "objid=xxx"
	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString sdoc_objid;
	if (args != "") AosParseArgs(args, "objid", sdoc_objid);
	if (worker_doc) rdata->setWorkingDoc(worker_doc, false);
	if (sdoc_objid != "")
	{
		// Chen Ding, 2013/05/30
		// AosSmartDocObj::procSmartdocsStatic(sdoc_objid, rdata);
		AosRunSmartDoc(rdata, sdoc_objid);
	}
	else
	{
		if (worker_doc)
		{
			OmnString jimo_objid = worker_doc->getAttrStr(AOSTAG_JIMO_OBJID);
			if (jimo_objid != "")
			{
				return AosRunJimo(rdata.getPtr(), worker_doc);
			}

			if (worker_doc->getAttrStr(AOSTAG_SMARTDOC_TYPE, "") != "")
			{
				// The 'worker_doc' itself is a smartdoc.
				// Chen Ding, 2013/05/30
				// AosSmartDocObj::runSmartdocStatic(worker_doc, rdata);
				AosRunSmartDoc(rdata, worker_doc);
			}
			else 
			{
				OmnString sdoc_objid = worker_doc->getAttrStr(AOSTAG_SMARTDOC_OBJID);
				if (sdoc_objid != "")
				{
					AosRunSmartDoc(rdata, sdoc_objid);
				}
				else
				{
					AosSetErrorUser(rdata, "missing_smartdoc") << enderr;
				}
			}
		}
		else
		{
			AosSetErrorUser(rdata, "missing_smartdoc") << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	if (!rdata->isOk())
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AOSSYSLOG_CREATE(true, AOSREQIDNAME_RUN_SMARTDOC, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}

