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
#include "JobTrans/WriteFileTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"

AosWriteFileTrans::AosWriteFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eWriteFile, regflag)
{
}


AosWriteFileTrans::AosWriteFileTrans(
		const OmnString &filename,
		const int svr_id,
		const int64_t &seekPos,
		const AosBuffPtr &buff)
:
AosTaskTrans(AosTransType::eWriteFile, svr_id, false, false),
mFileName(filename),
mSeekPos(seekPos),
mBuff(buff)
{
}


AosWriteFileTrans::~AosWriteFileTrans()
{
}


bool
AosWriteFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileName = buff->getOmnStr("");
	mSeekPos = buff->getI64(0);
	u32 buff_len = buff->getU32(0);
	mBuff = buff->getBuff(buff_len, true AosMemoryCheckerArgs);
	aos_assert_r(mBuff, false);
	return true;
}


bool
AosWriteFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mFileName);
	buff->setI64(mSeekPos);
	buff->setU32(mBuff->dataLen());
	buff->setBuff(buff);
	return true;
}


AosTransPtr
AosWriteFileTrans::clone()
{
	return OmnNew AosWriteFileTrans(false);
}


bool
AosWriteFileTrans::proc()
{
	bool rslt = AosNetFileMgrObj::writeBuffToFileLocalStatic(
		mFileName, mSeekPos, mBuff, mRdata.getPtr());
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}


