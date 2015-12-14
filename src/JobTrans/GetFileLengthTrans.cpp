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
// 2014/08/06	Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetFileLengthTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosGetFileLengthTrans::AosGetFileLengthTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetFileLength, regflag)
{
}


AosGetFileLengthTrans::AosGetFileLengthTrans(
		const u64 &fileId,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetFileLength, svr_id, false, true),
mFileId(fileId)
{
}


AosGetFileLengthTrans::AosGetFileLengthTrans(
		const OmnString &filename,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetFileLength, svr_id, false, true),
mFileId(0),
mFileName(filename)
{
}


AosGetFileLengthTrans::~AosGetFileLengthTrans()
{
}


bool
AosGetFileLengthTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileId = buff->getU64(0);
	mFileName = buff->getOmnStr("");
	aos_assert_r(mFileId || mFileName != "", false);
	return true;
}


bool
AosGetFileLengthTrans::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(mFileId || mFileName != "", false);
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mFileId);
	buff->setOmnStr(mFileName);
	return true;
}


AosTransPtr
AosGetFileLengthTrans::clone()
{
	return OmnNew AosGetFileLengthTrans(false);
}


bool
AosGetFileLengthTrans::proc()
{
	int64_t fileLen = 0;
	bool rslt = true;
	if (mFileName != "")
	{
		rslt = AosNetFileMgrObj::getFileLengthLocalStatic(
			mFileName, fileLen, mRdata.getPtr());
	}
	else
	{
		rslt = AosNetFileMgrObj::getFileLengthLocalStatic(
			mFileId, fileLen, mRdata.getPtr());
	}

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setI64(fileLen);

	sendResp(resp_buff);
	return true;
}

