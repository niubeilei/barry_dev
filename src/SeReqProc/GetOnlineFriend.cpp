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
#include "SeReqProc/GetOnlineFriend.h"

#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "InstantMsg/IMManager.h"
#include "XmlUtil/XmlTag.h"

AosGetOnlineFriend::AosGetOnlineFriend(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GETONLINE_FRIEND, 
		AosSeReqid::eGetOnlineFriend, rflag)
{
}


bool 
AosGetOnlineFriend::proc(const AosRundataPtr &rdata)
{
	/*
	//<request reqid = "getonlinefriend" >
	//  <objdef>
	//  	<Contents>
	//  	</Contents>
	//  </objdef>
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
		rdata->setError() << "Missing the contents!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	bool rslt = AosIMManager::getSelf()->getOnlineFriends(rdata);
	aos_assert_rr(rslt, rdata, false);
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

