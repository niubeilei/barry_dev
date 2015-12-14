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
// 2013/04/19 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/NotifyToClientTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"

AosNotifyToClient::AosNotifyToClient(const bool regflag)
:
AosTaskTrans(AosTransType::eNotifyToClient, regflag)
{
}


AosNotifyToClient::AosNotifyToClient(
		const int server_id,
		const u64 &docid,
		const u32 siteid) 
:
AosTaskTrans(AosTransType::eNotifyToClient, server_id, false, false),
mDocid(docid),
mSiteid(siteid)
{
}


AosNotifyToClient::~AosNotifyToClient()
{
}


bool
AosNotifyToClient::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	aos_assert_r(mDocid != 0, false);

	mSiteid = buff->getU32(0);
	return true;
}


bool
AosNotifyToClient::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	buff->setU32(mSiteid);
	return true;
}


AosTransPtr
AosNotifyToClient::clone()
{
	return OmnNew AosNotifyToClient(false);
}


bool
AosNotifyToClient::proc()
{
	AosDocClientObj::getDocClient()->procNotifyMsg(mDocid, mSiteid, mRdata);
	return true;
}

