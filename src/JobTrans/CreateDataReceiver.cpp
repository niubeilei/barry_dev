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
#include "JobTrans/CreateDataReceiver.h"

#include "DataReceiver/DataReceiverMgr.h"


AosCreateDataReceiver::AosCreateDataReceiver(const bool regflag)
:
AosTaskTrans(AosTransType::eCreateDataReceiver, regflag)
{
}


AosCreateDataReceiver::AosCreateDataReceiver(
		const OmnString &datacol_id,
		const OmnString &conf,
		const int svr_id)
:
AosTaskTrans(AosTransType::eCreateDataReceiver, svr_id, false, true),
mDataColId(datacol_id),
mConfig(conf)
{
}


AosCreateDataReceiver::~AosCreateDataReceiver()
{
}


bool
AosCreateDataReceiver::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDataColId = buff->getOmnStr("");
	mConfig = buff->getOmnStr("");
	return true;
}


bool
AosCreateDataReceiver::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mDataColId);
	buff->setOmnStr(mConfig);
	return true;
}


AosTransPtr
AosCreateDataReceiver::clone()
{
	return OmnNew AosCreateDataReceiver(false);
}


bool
AosCreateDataReceiver::proc()
{
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(mConfig, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosDataReceiverMgr::getSelf()->createDataReceiver(mDataColId, xml, mRdata);
	aos_assert_r(rslt, false);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	sendResp(resp_buff);
	return true;
}
