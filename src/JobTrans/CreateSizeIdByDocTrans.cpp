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
#include "JobTrans/CreateSizeIdByDocTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosCreateSizeIdByDocTrans::AosCreateSizeIdByDocTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eCreateSizeIdByDoc, regflag)
{
}


AosCreateSizeIdByDocTrans::AosCreateSizeIdByDocTrans(
		const int size,
		const AosXmlTagPtr &record,
		const int svr_id)
:
AosTaskTrans(AosTransType::eCreateSizeIdByDoc, svr_id, false, true),
mSize(size),
mRecordDoc(record)
{
}

AosCreateSizeIdByDocTrans::~AosCreateSizeIdByDocTrans()
{
}


bool
AosCreateSizeIdByDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mSize = buff->getInt(-1);
	OmnString str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	mRecordDoc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(mRecordDoc, false);
	return true;
}


bool
AosCreateSizeIdByDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setInt(mSize);
	aos_assert_r(mRecordDoc, false);
	buff->setOmnStr(mRecordDoc->toString());
	return true;
}


AosTransPtr
AosCreateSizeIdByDocTrans::clone()
{
	return OmnNew AosCreateSizeIdByDocTrans(false);
}


bool
AosCreateSizeIdByDocTrans::proc()
{
	u64 sizeId = AosCreateSizeId(mSize, mRecordDoc, mRdata);
	aos_assert_r(sizeId != 0, false);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU64(sizeId);
	sendResp(resp_buff);
	return true;
}

#endif
