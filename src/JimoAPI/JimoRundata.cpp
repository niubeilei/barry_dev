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
// 2014/12/30 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoRundata.h"

#include "Util/String.h"


AosJimoRundata::AosJimoRundata(
		const u32 siteid, 
		const u64 userid,
		const string &app_name, 
		const string &ssid,
		const string &cid)
:
mSiteid(siteid),
mUserid(userid),
mAppname(app_name),
mSsid(ssid),
mCid(cid)
{
}


AosJimoRundata::~AosJimoRundata()
{
}


AosRundataPtr
AosJimoRundata::convert()
{
	AosRundataPtr rdata = OmnNew AosRundata(
		mSiteid, OmnString(mAppname), OmnString(mSsid),
		OmnString(mCid), mUserid AosMemoryCheckerArgs);
	return rdata;
}

