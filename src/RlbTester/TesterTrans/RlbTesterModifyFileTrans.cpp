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
#include "RlbTester/TesterTrans/RlbTesterModifyFileTrans.h"

#include "API/AosApi.h"
#include "RlbTester/Server/Ptrs.h"
#include "SEInterfaces/RlbTesterSvrObj.h"

AosRlbTesterModifyFileTrans::AosRlbTesterModifyFileTrans(const bool regflag)
:
AosCubicTrans(AosTransType::eRlbTesterModifyFile, regflag)
{
}


AosRlbTesterModifyFileTrans::AosRlbTesterModifyFileTrans(
		const u32 cube_id,
		const u64 file_id,
		const u64 offset,
		const AosBuffPtr &data)
:
AosCubicTrans(AosTransType::eRlbTesterModifyFile, 
		cube_id, true, true, true),
mFileId(file_id),
mOffset(offset),
mData(data)
{
}


AosRlbTesterModifyFileTrans::~AosRlbTesterModifyFileTrans()
{
}


bool
AosRlbTesterModifyFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mFileId = buff->getU64(0);
	mOffset = buff->getU64(0);
	u32 data_len = buff->getU32(0);
	aos_assert_r(data_len, false);
	
	mData = buff->getBuff(data_len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosRlbTesterModifyFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mFileId);
	buff->setU64(mOffset);
	buff->setU32(mData->dataLen());
	buff->setBuff(mData);
	return true;
}


AosTransPtr
AosRlbTesterModifyFileTrans::clone()
{
	return OmnNew AosRlbTesterModifyFileTrans(false);
}


bool
AosRlbTesterModifyFileTrans::proc()
{
	AosRlbTesterSvrObjPtr svr = AosRlbTesterSvrObj::getSvr(); 
	aos_assert_r(svr, false);

	bool rslt = svr->modifyFile(
			mRdata, getTransId(), mFileId, mOffset, mData);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}

	AosBuffPtr resp_buff = OmnNew AosBuff(20, 0 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	sendResp(resp_buff);
	return true;
}


bool
AosRlbTesterModifyFileTrans::timeoutProc()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(20, 0 AosMemoryCheckerArgs);
	resp_buff->setU8(true);

	sendResp(resp_buff);
	return true;
}


