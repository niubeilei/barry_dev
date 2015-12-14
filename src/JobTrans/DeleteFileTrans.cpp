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
#include "JobTrans/DeleteFileTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosDeleteFileTrans::AosDeleteFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eDeleteFile, regflag)
{
}


AosDeleteFileTrans::AosDeleteFileTrans(
		const u64 &fileId,
		const int svr_id)
:
AosTaskTrans(AosTransType::eDeleteFile, svr_id, true, false),
mFileId(fileId)
{
}


AosDeleteFileTrans::AosDeleteFileTrans(
		const OmnString &filename,
		const int svr_id)
:
AosTaskTrans(AosTransType::eDeleteFile, svr_id, true, false),
mFileId(0),
mFileName(filename)
{
}


AosDeleteFileTrans::~AosDeleteFileTrans()
{
}


bool
AosDeleteFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileId = buff->getU64(0);
	mFileName = buff->getOmnStr("");
	aos_assert_r(mFileId || mFileName != "", false);
	return true;
}


bool
AosDeleteFileTrans::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(mFileId || mFileName != "", false);
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mFileId);
	buff->setOmnStr(mFileName);
	return true;
}


AosTransPtr
AosDeleteFileTrans::clone()
{
	return OmnNew AosDeleteFileTrans(false);
}


bool
AosDeleteFileTrans::proc()
{
	bool rslt = true;
	if (mFileName != "")
	{
		AosNetFileMgrObj::deleteFileLocalStatic(mFileName, mRdata.getPtr());
	}
	else
	{
		AosNetFileMgrObj::deleteFileLocalStatic(mFileId, mRdata.getPtr());
	}

	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}

