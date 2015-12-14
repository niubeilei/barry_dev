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
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "RlbTester/TesterTrans/RlbTesterReadDataTrans.h"

#include "API/AosApi.h"
#include "RlbTester/Server/Ptrs.h"
#include "SEInterfaces/RlbTesterSvrObj.h"

AosRlbTesterReadDataTrans::AosRlbTesterReadDataTrans(const bool regflag)
:
AosCubicTrans(AosTransType::eRlbTesterReadData, regflag)
{
}


AosRlbTesterReadDataTrans::AosRlbTesterReadDataTrans(
		const u64 file_id,
		const u64 offset,
		const u32 data_len)
:
AosCubicTrans(AosTransType::eRlbTesterReadData, 
		(file_id >> 32), true, false, true),
mFileId(file_id),
mOffset(offset),
mDataLen(data_len)
{
}


AosRlbTesterReadDataTrans::~AosRlbTesterReadDataTrans()
{
}


bool
AosRlbTesterReadDataTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mFileId = buff->getU64(0);
	mOffset = buff->getU64(0);
	mDataLen = buff->getU32(0);
	return true;
}


bool
AosRlbTesterReadDataTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mFileId);
	buff->setU64(mOffset);
	buff->setU32(mDataLen);
	return true;
}


AosTransPtr
AosRlbTesterReadDataTrans::clone()
{
	return OmnNew AosRlbTesterReadDataTrans(false);
}


bool
AosRlbTesterReadDataTrans::proc()
{
	AosRlbTesterSvrObjPtr svr = AosRlbTesterSvrObj::getSvr(); 
	aos_assert_r(svr, false);

	AosBuffPtr data;
	bool rslt = svr->readData(
			mRdata, mFileId, mOffset, mDataLen, data);
	aos_assert_r(rslt && data, false);
	aos_assert_r((u32)data->dataLen() == mDataLen, false);

	AosBuffPtr resp_buff = OmnNew AosBuff(20, 0 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU32(mDataLen);
	resp_buff->setBuff(data);

	sendResp(resp_buff);
	return true;
}


