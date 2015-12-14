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
#include "RlbTester/TesterTrans/RlbTesterDeleteFileTrans.h"

#include "API/AosApi.h"
#include "RlbTester/Server/Ptrs.h"
#include "RlbTester/Server/RlbTesterSvr.h"
#include "SEInterfaces/RlbTesterSvrObj.h"

AosRlbTesterDeleteFileTrans::AosRlbTesterDeleteFileTrans(const bool regflag)
:
AosCubicTrans(AosTransType::eRlbTesterDeleteFile, regflag)
{
}


AosRlbTesterDeleteFileTrans::AosRlbTesterDeleteFileTrans(
		const u32 cube_id,
		const u64 file_id)
:
AosCubicTrans(AosTransType::eRlbTesterDeleteFile, 
		cube_id, true, true, true),
mFileId(file_id)
{
}


AosRlbTesterDeleteFileTrans::~AosRlbTesterDeleteFileTrans()
{
}


bool
AosRlbTesterDeleteFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mFileId = buff->getU64(0);
	return true;
}


bool
AosRlbTesterDeleteFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mFileId);
	return true;
}


AosTransPtr
AosRlbTesterDeleteFileTrans::clone()
{
	return OmnNew AosRlbTesterDeleteFileTrans(false);
}


bool
AosRlbTesterDeleteFileTrans::proc()
{
	AosRlbTesterSvrObjPtr svr = AosRlbTesterSvrObj::getSvr(); 
	aos_assert_r(svr, false);

	u64 file_id = 0;
	OmnString fname;
	bool rslt = svr->deleteFile(
			mRdata, getTransId(), file_id);
	aos_assert_r(rslt, false);
	
	AosBuffPtr resp_buff = OmnNew AosBuff(20, 0 AosMemoryCheckerArgs);
	resp_buff->setU64(file_id);
	resp_buff->setOmnStr(fname);

	sendResp(resp_buff);
	return true;
}


