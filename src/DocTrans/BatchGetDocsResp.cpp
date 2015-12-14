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
// 2013/04/08 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocTrans/BatchGetDocsResp.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"

AosBatchGetDocsResp::AosBatchGetDocsResp(const bool regflag)
:
AosTaskTrans(AosTransType::eBatchGetDocsResp, regflag)
{
}


AosBatchGetDocsResp::AosBatchGetDocsResp(
		const OmnString &scanner_id,
		const int sender_server_id, 
		const int server_id,
		const AosBuffPtr &contents,
		const bool finished,
		const int num_call_back_data_msgs) 
:
AosTaskTrans(AosTransType::eBatchGetDocsResp, sender_server_id, false, false),
//mSenderTransId(sender_trans_id),
mScannerId(scanner_id),
mContents(contents),
mServerId(server_id),
mFinished(finished),
mNumCallBackNumDatas(num_call_back_data_msgs)
{
}


AosBatchGetDocsResp::~AosBatchGetDocsResp()
{
}


bool
AosBatchGetDocsResp::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mScannerId = buff->getOmnStr("");
	aos_assert_r(mScannerId != "", false);

	u32 data_len = buff->getU32(0);
	mContents = buff->getBuff(data_len, true AosMemoryCheckerArgs);

	mServerId = buff->getInt(0);
	mFinished = buff->getU8(0);
	mNumCallBackNumDatas = buff->getInt(0);
	return true;
}


bool
AosBatchGetDocsResp::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mScannerId);
	buff->setU32(mContents->dataLen());
	buff->setBuff(mContents->data(), mContents->dataLen());
	buff->setInt(mServerId);
	buff->setU8(mFinished);
	buff->setInt(mNumCallBackNumDatas);
	return true;
}


AosTransPtr
AosBatchGetDocsResp::clone()
{
	return OmnNew AosBatchGetDocsResp(false);
}


bool
AosBatchGetDocsResp::proc()
{
	// 1. Find the caller
	// 2. Call back
	AosDocClientObj::getDocClient()->batchDataReceived(
			mScannerId, mContents, mServerId, mFinished, mNumCallBackNumDatas, mRdata);
	return true;
}

#endif
