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
#include "RlbTester/TesterTrans/RlbTesterCreateFileTrans.h"

#include "API/AosApi.h"
#include "RlbTester/Server/Ptrs.h"
#include "SEInterfaces/RlbTesterSvrObj.h"

AosRlbTesterCreateFileTrans::AosRlbTesterCreateFileTrans(const bool regflag)
:
AosCubicTrans(AosTransType::eRlbTesterCreateFile, regflag)
{
}


AosRlbTesterCreateFileTrans::AosRlbTesterCreateFileTrans(
		const u32 cube_id,
		const u32 site_id,
		const u64 requested_size)
:
AosCubicTrans(AosTransType::eRlbTesterCreateFile, 
		cube_id, true, true, true),
mSiteId(site_id),
mRequestedSize(requested_size)
{
}


AosRlbTesterCreateFileTrans::~AosRlbTesterCreateFileTrans()
{
}


bool
AosRlbTesterCreateFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mSiteId = buff->getU32(0);
	mRequestedSize = buff->getU64(0);
	return true;
}


bool
AosRlbTesterCreateFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mSiteId);
	buff->setU64(mRequestedSize);
	return true;
}


AosTransPtr
AosRlbTesterCreateFileTrans::clone()
{
	return OmnNew AosRlbTesterCreateFileTrans(false);
}


bool
AosRlbTesterCreateFileTrans::proc()
{
	AosRlbTesterSvrObjPtr svr = AosRlbTesterSvrObj::getSvr(); 
	aos_assert_r(svr, false);

	u64 file_id = 0;
	OmnString fname;
	bool rslt = svr->createFile(mRdata, getTransId(),
			getCubeId(), mSiteId, mRequestedSize, file_id, fname);
	aos_assert_r(rslt, false);
	
	AosBuffPtr resp_buff = OmnNew AosBuff(20, 0 AosMemoryCheckerArgs);
	resp_buff->setU64(file_id);
	resp_buff->setOmnStr(fname);

	sendResp(resp_buff);
	return true;
}

	
bool
AosRlbTesterCreateFileTrans::timeoutProc()
{
	return proc();
}


