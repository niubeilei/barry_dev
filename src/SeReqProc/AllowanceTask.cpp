////////////////////////////////////////////////////////////////////////
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
#include "SeReqProc/AllowanceTask.h"

#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "SEInterfaces/SmartDocObj.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosAllowanceTask::AosAllowanceTask(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_ALLOWANCE, 
		AosSeReqid::eAllowanceTask, rflag)
{
}


bool 
AosAllowanceTask::proc(const AosRundataPtr &rdata)
{
	// This function is writed by james 2010/11/08

	//<request reqid = "allowancetask" >
	//      <objdef>
	//          <contents>
	//              <zky_allowance> xxxx </zky_allowance>
	//              <taskCount> xxx </taskCount>
	//          </contents>
	//      </objdef>
	//   xxxxxxxxx
	//</request>
	
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;
	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	objdef = objdef->getFirstChild("Contents");
	if (!objdef)
	{
		errmsg = "Missing the allowance object!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString sdoc_objids = objdef->getNodeText(AOSTAG_ALLOWANCE);
	if (sdoc_objids != "")
	{
		AosSmartDocObj::procSmartdocsStatic(sdoc_objids, rdata);
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

