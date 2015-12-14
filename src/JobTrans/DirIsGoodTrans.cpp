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
#include "JobTrans/DirIsGoodTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosDirIsGoodTrans::AosDirIsGoodTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eDirIsGood, regflag)
{
}


AosDirIsGoodTrans::AosDirIsGoodTrans(
		const OmnString &path,
		const int svr_id,
		const bool checkfile)
:
AosTaskTrans(AosTransType::eDirIsGood, svr_id, false, true),
mPath(path),
mCheckFile(checkfile)
{
}


AosDirIsGoodTrans::~AosDirIsGoodTrans()
{
}


bool
AosDirIsGoodTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mPath = buff->getOmnStr("");
	mCheckFile = buff->getInt(0);
	return true;
}


bool
AosDirIsGoodTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mPath);
	buff->setInt(mCheckFile);
	return true;
}


AosTransPtr
AosDirIsGoodTrans::clone()
{
	return OmnNew AosDirIsGoodTrans(false);
}


bool
AosDirIsGoodTrans::proc()
{
	bool rslt = AosNetFileMgrObj::dirIsGoodLocalStatic(
		mPath, mCheckFile, mRdata.getPtr());

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	
	sendResp(resp_buff);
	return true;
}

