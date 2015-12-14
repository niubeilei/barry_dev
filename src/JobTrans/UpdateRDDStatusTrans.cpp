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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/UpdateRDDStatusTrans.h"

#include "XmlUtil/XmlTag.h"
#include "TaskUtil/Ptrs.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "StreamEngine/StreamData.h"

AosUpdateRDDStatusTrans::AosUpdateRDDStatusTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eUpdateRDDStatus, regflag)
{
}


AosUpdateRDDStatusTrans::AosUpdateRDDStatusTrans(
		const AosTaskStatus::E &status,
		const OmnString &serviceid,
		const OmnString &dataid,
		const OmnString &dp_name,
		const u64 &rddid,
		const int remain,
		const int server_id)
:
AosTaskTrans(AosTransType::eUpdateRDDStatus, server_id, false, true),
mStatus(status),
mServiceId(serviceid),
mDataId(dataid),
mDataProcName(dp_name),
mRDDId(rddid),
mRemain(remain)
{
}


AosUpdateRDDStatusTrans::~AosUpdateRDDStatusTrans()
{
}


bool
AosUpdateRDDStatusTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mServiceId = buff->getOmnStr("");
	mRDDId = buff->getU64(0);
	aos_assert_r(mRDDId != 0, false);
	mStatus = AosTaskStatus::toEnum(buff->getOmnStr(""));
	mDataId = buff->getOmnStr("");
	mDataProcName = buff->getOmnStr("");
	mRemain = buff->getInt(-1);
	aos_assert_r(mRemain != -1, false);
	return true;
}


bool
AosUpdateRDDStatusTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mServiceId);
	buff->setU64(mRDDId);
	buff->setOmnStr(AosTaskStatus::toStr(mStatus));
	buff->setOmnStr(mDataId);
	buff->setOmnStr(mDataProcName);
	aos_assert_r(mRemain != -1, false);
	buff->setInt(mRemain);
	return true;
}


AosTransPtr
AosUpdateRDDStatusTrans::clone()
{
	return OmnNew AosUpdateRDDStatusTrans(false);
}


bool
AosUpdateRDDStatusTrans::proc()
{
	//this trans proc in 0 machine's frontend
	AosStreamDataPtr stream_data = AosStreamData::getStreamData(mRdata.getPtr(), mDataId, mServiceId);
	bool rslt = stream_data->updateRDDStatus(mStatus, mDataProcName, mRDDId, mRemain, mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}


OmnString
AosUpdateRDDStatusTrans::toString()
{
	return "";
}

