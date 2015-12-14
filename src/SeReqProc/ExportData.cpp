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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ExportData.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SEServer/SeReqProc.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosExportData::AosExportData(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_EXPORTDATA, 
		AosSeReqid::eExportData, rflag)
{
}


bool 
AosExportData::proc(const AosRundataPtr &rdata)
{
	// This function is created by james 2011/02/22
	//request format is:
	//<request>
	//  <command>
	//      <cmd .../>
	//  </command>
	//  <item name="zky_sdocid">xxx</item>
	//  <item name="zky_title">xxx</item>
	//</request>
	
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	//1. retrieve the sdoc id
	OmnString sdoc_id = root->getChildTextByAttr("name","zky_sdocid");
	if (sdoc_id != "")
	{
		OmnString cid;
		AosSessionObjPtr session = rdata->getSession();
		if (session) cid = session->getCid();
		rdata->setCid(cid);

		rdata->setRetrievedDoc(root, true);
		bool rslt = AosSmartDocObj::procSmartdocsStatic(sdoc_id, rdata);
		if (!rslt)
		{
			AOSLOG_LEAVE(rdata);
			return false;
		}
		else
		{
			rdata->setOk();
			AOSLOG_LEAVE(rdata);
			return true;
		}
	}

	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}	
