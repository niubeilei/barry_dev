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
#include "SeReqProc/ProcOnlineStatus.h"

#include "SeReqProc/ReqidNames.h"
#include "SearchEngine/DocServer.h"
#include "SEServer/SeReqProc.h"
#include "Security/SessionMgr.h"
#include "MsgService/MsgService.h"


AosProcOnlineStatus::AosProcOnlineStatus(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_PROCONLINESTATUS, 
		AosSeReqid::eProcOnlineStatus, rflag)
{
}


bool 
AosProcOnlineStatus::proc(const AosRundataPtr &rdata)
{
	/*
	// MsgServer . This function checklogin
	//<request reqid = "proconlinestatus" >
	//	<objdef>
	//		<Contents>
	//			<status>1</status>
	//		</Contents>
	//	</objdef>
	//</request>
	
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	aos_assert_r(root, false);
	if (!root)
	{
		rdata->setError() << "Missing request";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	aos_assert_r(objdef, false);
	if (!objdef)
	{
		rdata->setError() << "Missing objdef";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	aos_assert_r(contents, false);
	if (!contents)
	{
		rdata->setError() << "Missing the contents";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString status = contents ->getNodeText("status");
	aos_assert_r(status != "", false);
	if (status == "")
	{
		rdata->setError() << "Missing the status";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//bool setstatus = AosMsgSvrOnlineMgr::getSelf()->setUserOnlineStatus(status, rdata);
	//aos_assert_rr(setstatus, rdata, false);
	bool set = AosMsgService::getSelf()->setUserOnlineStatus(status, rdata);
	aos_assert_rr(set, rdata, false);
	OmnString cnts;
	cnts << "<Contents/>";
	rdata->setResults(cnts);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

