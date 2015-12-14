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
// 07/27/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ApplyFriend.h"

#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "InstantMsg/IMManager.h"
#include "XmlUtil/XmlTag.h"


AosApplyFriend::AosApplyFriend(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_APPLY_FRIEND, 
		AosSeReqid::eApplyFriend, rflag)
{
}


bool 
AosApplyFriend::proc(const AosRundataPtr &rdata)
{
	/*
	///<request reqid = "applyfriend" >
	//	<objdef>
	//		<Contents>
	//			<friend_cid>xxxx</friend_cid>
	//			<self_gid>xxxx</self_gid>
	//			<msg>xxxx</msg>
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

	OmnString friend_cid = contents->getNodeText("friend_cid");
	aos_assert_r(friend_cid != "", false);
	if (friend_cid == "")
	{
		rdata->setError() << "Missing the friend cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString self_gid = contents->getNodeText("self_gid");
	aos_assert_r(self_gid != "", false);
	if (self_gid == "")
	{
		rdata->setError() << "Missing the friend groupid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}


	OmnString msg = contents->getNodeText("msg");
	aos_assert_r(msg != "", false);
	if (msg == "")
	{
		rdata->setError() << "Missing the friend cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt = AosIMManager::getSelf()->applyFriend(friend_cid, self_gid, msg, rdata);
	aos_assert_r(rslt, false);
	if (!rslt)
	{
		rdata->setError() << "Faild to apply friend";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	rdata->setOk();
	*/
	OmnNotImplementedYet;
	return true;
}
