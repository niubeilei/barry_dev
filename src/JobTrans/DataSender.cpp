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
// 11/17/2015	Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/DataSender.h"

#include "DataReceiver/DataReceiverMgr.h"


AosDataSender::AosDataSender(const bool regflag)
:
AosTaskTrans(AosTransType::eDataSender, regflag)
{
}


AosDataSender::AosDataSender(
		const OmnString &datacol_id,
		const AosBuffPtr &buff,
		const int svr_id)
:
AosTaskTrans(AosTransType::eDataSender, svr_id, false, true),
mDataColId(datacol_id),
mBuff(buff)
{
}


AosDataSender::~AosDataSender()
{
}


bool
AosDataSender::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDataColId = buff->getOmnStr("");
	u32 len = buff->getU32(0);
	mBuff= buff->getBuff(len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosDataSender::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mDataColId);
	buff->setU32(mBuff->dataLen());
	buff->setBuff(mBuff);
	return true;
}


AosTransPtr
AosDataSender::clone()
{
	return OmnNew AosDataSender(false);
}


bool
AosDataSender::proc()
{
	bool rslt = AosDataReceiverMgr::getSelf()->dataReceiver(mDataColId, mBuff);
	aos_assert_r(rslt, false);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	sendResp(resp_buff);
	return true;
}
