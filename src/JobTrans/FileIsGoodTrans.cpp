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
#include "JobTrans/FileIsGoodTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosFileIsGoodTrans::AosFileIsGoodTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eFileIsGood, regflag)
{
}


AosFileIsGoodTrans::AosFileIsGoodTrans(
		const OmnString &filename,
		const int svr_id)
:
AosTaskTrans(AosTransType::eFileIsGood, svr_id, false, true),
mFileName(filename)
{
}


AosFileIsGoodTrans::~AosFileIsGoodTrans()
{
}


bool
AosFileIsGoodTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileName = buff->getOmnStr("");
	return true;
}


bool
AosFileIsGoodTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mFileName);
	return true;
}


AosTransPtr
AosFileIsGoodTrans::clone()
{
	return OmnNew AosFileIsGoodTrans(false);
}


bool
AosFileIsGoodTrans::proc()
{
	bool rslt = AosNetFileMgrObj::fileIsGoodLocalStatic(mFileName, mRdata.getPtr());

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	return true;
}

