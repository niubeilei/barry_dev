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
// 11/03/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/SmartReply.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "SmartReply/SmartReplyMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosSmartReply::AosSmartReply(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SMART_REPLY, 
		AosSeReqid::eSmartReply, rflag)
{
}


bool 
AosSmartReply::proc(const AosRundataPtr &rdata)
{
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;
	OmnString question = root->getChildTextByAttr("name", "args");

	if (question == "")
	{
		errmsg = "Missing the answer!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString rslt = AosSmartReplyMgr::getSelf()->getReply(question);	

	OmnString contents = "<Contents>";
	contents << rslt << "</Contents>";

	rdata->setResults(contents);
	rdata->setOk();

	AOSLOG_LEAVE(rdata);
	return true;
}	
