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
// The torturer is in SengTorturer/TesterRunIndexMgrNew.cpp
//   
//
// Modification History:
// 01/20/2014	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RunIndexMgr.h"

#include "API/AosApi.h"
#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
//#include "SEInterfaces/IndexMgrObj.h"
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
using namespace std;


AosRunIndexMgr::AosRunIndexMgr(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RUN_INDEXMGR, 
		AosSeReqid::eRunIndexMgr, rflag)
{
}


bool 
AosRunIndexMgr::proc(const AosRundataPtr &rdata)
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
	aos_assert_r(objdef, false);

	objdef = objdef->getFirstChild(true);
	aos_assert_r(objdef, false);

	OmnString type = objdef->getAttrStr("zky_index_type", "");
	aos_assert_r(type != "", false);
	
	if(type == "create")
	{
		AosXmlTagPtr conf = objdef->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(conf, false);
		AosCreateIndexEntry(rdata.getPtr(), conf);
	}
	else if (type == "delete")
	{
		OmnString table_name = objdef->getAttrStr(AOSTAG_TABLENAME, "");
		OmnString field_name = objdef->getAttrStr(AOSTAG_FIELD_NAME, "");
		aos_assert_r(table_name != "" || field_name != "", false);
		AosRemoveIndexEntry(rdata.getPtr(), table_name, field_name);
	}
	else if(type == "modify")
	{
		AosXmlTagPtr conf = objdef->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(conf, false);
		AosModifyIndexEntry(rdata.getPtr(), conf);
	}
	else
	{
		OmnAlarm << "Invalid request type!" << enderr;
		rdata->setError() << "Invalid request type!" ;
		AOSLOG_LEAVE(rdata);
		return false;
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
