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
#include "JobTrans/AppendFileTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosAppendFileTrans::AosAppendFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAppendFile, regflag)
{
}


AosAppendFileTrans::AosAppendFileTrans(
		const OmnString &filename,
		const int svr_id,
		const AosBuffPtr &buff)
:
AosTaskTrans(AosTransType::eAppendFile, svr_id, false, false),
mFileName(filename),
mBuff(buff)
{
}


AosAppendFileTrans::~AosAppendFileTrans()
{
}


bool
AosAppendFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mFileName = buff->getOmnStr("");
	u32 buff_len = buff->getU32(0);
	int crtIdx = buff->getCrtIdx();
	// Chen Ding, 2015/05/25
	// mBuff = OmnNew AosBuff(&(buff->data())[crtIdx], buff_len, buff_len AosMemoryCheckerArgs);
	mBuff = OmnNew AosBuff(&(buff->data())[crtIdx], buff_len, buff_len, true AosMemoryCheckerArgs);
	aos_assert_r(mBuff, false);

	return true;
}


bool
AosAppendFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mFileName);
	buff->setU32(mBuff->dataLen());
	buff->setBuff(buff);

	return true;
}


AosTransPtr
AosAppendFileTrans::clone()
{
	return OmnNew AosAppendFileTrans(false);
}


bool
AosAppendFileTrans::proc()
{
	bool rslt = AosNetFileMgrObj::appendBuffToFileLocalStatic(
		mFileName, mBuff, mRdata.getPtr());
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}


