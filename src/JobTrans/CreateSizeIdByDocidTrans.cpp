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
// 12/26/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#if 0
#include "JobTrans/CreateSizeIdByDocidTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosCreateSizeIdByDocidTrans::AosCreateSizeIdByDocidTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eCreateSizeIdByDocid, regflag)
{
}


AosCreateSizeIdByDocidTrans::AosCreateSizeIdByDocidTrans(
		const u64 &record_docid,
		const int svr_id)
:
AosTaskTrans(AosTransType::eCreateSizeIdByDocid, svr_id, false, true),
mRecordDocid(record_docid)
{
}

AosCreateSizeIdByDocidTrans::~AosCreateSizeIdByDocidTrans()
{
}


bool
AosCreateSizeIdByDocidTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mRecordDocid = buff->getU64(0);
	aos_assert_r(mRecordDocid != 0, false);
	return true;
}


bool
AosCreateSizeIdByDocidTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(mRecordDocid != 0, false);

	buff->setU64(mRecordDocid);
	return true;
}


AosTransPtr
AosCreateSizeIdByDocidTrans::clone()
{
	return OmnNew AosCreateSizeIdByDocidTrans(false);
}


bool
AosCreateSizeIdByDocidTrans::proc()
{
	u64 sizeId = AosCreateSizeId(mRecordDocid, mRdata);
	aos_assert_r(sizeId != 0, false);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU64(sizeId);
	sendResp(resp_buff);
	return true;
}

#endif
