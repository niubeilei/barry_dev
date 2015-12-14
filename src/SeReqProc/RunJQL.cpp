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
// The torturer is in SengTorturer/TesterRunJQLNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RunJQL.h"

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
#include "JQLParser/JQLParser.h"
using namespace std;

extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;


AosRunJQL::AosRunJQL(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RUN_JQL, 
		AosSeReqid::eRunJQL, rflag)
{
}


bool 
AosRunJQL::proc(const AosRundataPtr &rdata)
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

	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef")->getFirstChild(true);
	aos_assert_r(objdef, false);
	AosXmlTagPtr JQLnode  = objdef->getFirstChild("jqlnode");
	aos_assert_r(JQLnode, false);
	OmnString JQLstr = JQLnode->getNodeText();
	bool debug_flag = objdef->getAttrBool("zky_debug_flag", false);

	OmnScreen << "Second parsing" << endl;

	AosParseJQL((char *)JQLstr.data(), debug_flag);

	OmnScreen << "Parsing finished" << endl;
	gAosJQLParser.dump();
	OmnScreen << "------------ Run Statements" << endl;

	vector<AosJqlStatementPtr> statements = gAosJQLParser.getStatements();
	OmnString errmsg = gAosJQLParser.getErrmsg();
	if (errmsg != "") 
	{
		AosSetEntityError(rdata, "JQLParser_", errmsg, errmsg) << enderr;               
		return false;
	}

	for (u32 i=0; i<statements.size(); i++)
	{
		statements[i]->run(rdata, 0);
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

