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
// 06/22/2011	Created by Michael 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RemoveTalkGroup.h"

#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "InstantMsg/IMManager.h"
#include "XmlUtil/XmlTag.h"

AosRemoveTalkGroup::AosRemoveTalkGroup(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REMOVE_TALKGROUP, 
		AosSeReqid::eRemoveTalkGroup, rflag)
{
}


bool 
AosRemoveTalkGroup::proc(const AosRundataPtr &rdata)
{
	/*
	//<request reqid="removetalkgroup">
	//	<objdef>
	//		<Contents>
	//			<talkgroupid>xxxx</talkgroupid>
	//		</Contents>
	//	</objdef>
	//</request>
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		OmnAlarm << rdata->getErrmsg() << enderr;
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

	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	if (!contents)
	{
		rdata->setError() << "Missing contents";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString talkgroupid = contents->getNodeText("talkgroupid");
	if (talkgroupid == "")
	{
		rdata->setError() << "Missing the talkgroupid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt = AosIMManager::getSelf()->removeTalkGroup(talkgroupid, rdata);
	if (!rslt)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString cnts;
	cnts << "<Contents>" << talkgroupid << "</Contents>";
	rdata->setResults(cnts);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

