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
#include "JobTrans/ReadFileTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosReadFileTrans::AosReadFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eReadFile, regflag)
{
}


AosReadFileTrans::AosReadFileTrans(
		const u64 &fileId,
		const int svr_id,
		const int64_t &seekPos,
		const u32 bytes_to_read)
:
AosTaskTrans(AosTransType::eReadFile, svr_id, false, true),
mFileId(fileId),
mSeekPos(seekPos),
mBytesToRead(bytes_to_read)
{
}


AosReadFileTrans::AosReadFileTrans(
		const OmnString &filename,
		const int svr_id,
		const int64_t &seekPos,
		const u32 bytes_to_read)
:
AosTaskTrans(AosTransType::eReadFile, svr_id, false, true),
mFileId(0),
mFileName(filename),
mSeekPos(seekPos),
mBytesToRead(bytes_to_read)
{
}


AosReadFileTrans::~AosReadFileTrans()
{
}


bool
AosReadFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mFileId = buff->getU64(0);
	mFileName = buff->getOmnStr("");
	mSeekPos = buff->getI64(0);
	mBytesToRead = buff->getU32(0);
	return true;
}


bool
AosReadFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mFileId);
	buff->setOmnStr(mFileName);
	buff->setI64(mSeekPos);
	buff->setU32(mBytesToRead);
	return true;
}


AosTransPtr
AosReadFileTrans::clone()
{
	return OmnNew AosReadFileTrans(false);
}


bool
AosReadFileTrans::proc()
{
	AosBuffPtr filebuff;
	bool rslt = true;
	
	if (mFileName != "")
	{
		rslt = AosNetFileMgrObj::readFileToBuffLocalStatic(
			mFileName, mSeekPos, mBytesToRead, filebuff, mRdata.getPtr());
	}
	else
	{
		rslt = AosNetFileMgrObj::readFileToBuffLocalStatic(
			mFileId, mSeekPos, mBytesToRead, filebuff, mRdata.getPtr());
	}

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU32(filebuff->dataLen());
	resp_buff->setBuff(filebuff);

	sendResp(resp_buff);
	return true;
}

