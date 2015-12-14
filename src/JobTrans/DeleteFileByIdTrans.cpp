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
#if 0
#include "JobTrans/DeleteFileByIdTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"

AosDeleteFileByIdTrans::AosDeleteFileByIdTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eDeleteFileById, regflag)
{
}


AosDeleteFileByIdTrans::AosDeleteFileByIdTrans(
		const u64 &fileId,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eDeleteFileById, svr_id, need_save, need_resp),
mFileId(fileId)
{
}


AosDeleteFileByIdTrans::~AosDeleteFileByIdTrans()
{
}


bool
AosDeleteFileByIdTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileId = buff->getU64(0);
	return true;
}


bool
AosDeleteFileByIdTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mFileId);
	return true;
}


AosTransPtr
AosDeleteFileByIdTrans::clone()
{
	return OmnNew AosDeleteFileByIdTrans(false);
}


bool
AosDeleteFileByIdTrans::proc()
{
	bool rslt = AosNetFileMgrObj::deleteFileByIdLocalStatic(mFileId, mRdata.getPtr());
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}

#endif
