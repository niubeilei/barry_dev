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
// 06/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CreateAccessRecord.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosCreateAccessRecord::AosCreateAccessRecord(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_ACCESSRECORD, AosSeReqid::eCreateAccessRecord, rflag)
{
}


bool 
AosCreateAccessRecord::proc(const AosRundataPtr &rdata)
{
	// This function creates an access record. The input should be:
	// 	<request ...>
	// 		<objdef>
	// 			<accesses>
	// 				<access .../>
	// 				<access .../>
	// 				...
	// 			</accesses>
	// 		</objid>
	// 	</request>
	
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		rdata->setError() << "Missing arguments!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString objid;
	AosParseArgs(args, "objid", objid);
	if (objid == "")
	{
		rdata->setError() << "Missing objid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Missing objdef";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// AosXmlTagPtr accesses = objdef->getFirstChild();
	// AosArcdMgr::getSelf()->createAcrd(objid, accesses, rdata);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

