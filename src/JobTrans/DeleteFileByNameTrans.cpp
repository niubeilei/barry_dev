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
#include "JobTrans/DeleteFileByNameTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"

AosDeleteFileByNameTrans::AosDeleteFileByNameTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eDeleteFileByName, regflag)
{
}


AosDeleteFileByNameTrans::AosDeleteFileByNameTrans(
		const OmnString &fname,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eDeleteFileByName, svr_id, need_save, need_resp),
mFileName(fname)
{
}


AosDeleteFileByNameTrans::~AosDeleteFileByNameTrans()
{
}


bool
AosDeleteFileByNameTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileName = buff->getOmnStr("");
	aos_assert_r(mFileName != "", false);
	return true;
}


bool
AosDeleteFileByNameTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mFileName);
	return true;
}


AosTransPtr
AosDeleteFileByNameTrans::clone()
{
	return OmnNew AosDeleteFileByNameTrans(false);
}


bool
AosDeleteFileByNameTrans::proc()
{
	bool rslt = AosNetFileMgrObj::deleteFileByNameLocalStatic(mFileName, mRdata.getPtr());
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}

#endif
