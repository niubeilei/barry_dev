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
#include "SeReqProc/CreateTalkGroup.h"

#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "InstantMsg/IMManager.h"
#include "XmlUtil/XmlTag.h"



AosCreateTalkGroup::AosCreateTalkGroup(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATETALKGROUP, 
		AosSeReqid::eCreateTalkGroup, rflag)
{
}


bool 
AosCreateTalkGroup::proc(const AosRundataPtr &rdata)
{
	/*
	// MsgServer . This function checklogin
	//<request reqid = "createtalkgroup" >
	//	<objdef>
	//		<Contents>
	//			<talkgroup zky_talk_groupname="xxxx">
	//				<friend ..../>
	//				<friend ..../>
	//			</talkgroup>
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
	
	AosXmlTagPtr talkgroup = contents->getFirstChild("talkgroup");
	aos_assert_rr(talkgroup , rdata, false);

	bool rslt = AosIMManager::getSelf()->createTalkGroup(talkgroup, rdata);
	aos_assert_rr(rslt, rdata, false);
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

