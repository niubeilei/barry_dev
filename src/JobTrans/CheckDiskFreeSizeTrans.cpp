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
#include "JobTrans/CheckDiskFreeSizeTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobMgrObj.h"

AosCheckDiskFreeSizeTrans::AosCheckDiskFreeSizeTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eCheckDiskFreeSize, regflag)
{
}


AosCheckDiskFreeSizeTrans::AosCheckDiskFreeSizeTrans(
		const int svr_id)
:
AosTaskTrans(AosTransType::eCheckDiskFreeSize, svr_id, false, true)
{
}


AosCheckDiskFreeSizeTrans::~AosCheckDiskFreeSizeTrans()
{
}


bool
AosCheckDiskFreeSizeTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCheckDiskFreeSizeTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosTransPtr
AosCheckDiskFreeSizeTrans::clone()
{
	return OmnNew AosCheckDiskFreeSizeTrans(false);
}


bool
AosCheckDiskFreeSizeTrans::proc()
{
	bool rslt = AosJobMgrObj::getJobMgr()->checkDiskFreeSizeLocal(mRdata);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	sendResp(resp_buff);
	return true;
}

