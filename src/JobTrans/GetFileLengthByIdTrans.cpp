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
#include "JobTrans/GetFileLengthByIdTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosGetFileLengthByIdTrans::AosGetFileLengthByIdTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetFileLengthById, regflag)
{
}


AosGetFileLengthByIdTrans::AosGetFileLengthByIdTrans(
		const u64 &fileId,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eGetFileLengthById, svr_id, need_save, need_resp),
mFileId(fileId)
{
}


AosGetFileLengthByIdTrans::~AosGetFileLengthByIdTrans()
{
}


bool
AosGetFileLengthByIdTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileId = buff->getU64(0);
	return true;
}


bool
AosGetFileLengthByIdTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mFileId);
	return true;
}


AosTransPtr
AosGetFileLengthByIdTrans::clone()
{
	return OmnNew AosGetFileLengthByIdTrans(false);
}


bool
AosGetFileLengthByIdTrans::proc()
{
	int64_t filelen;
	bool rslt = AosNetFileMgrObj::getFileLengthByIdLocalStatic(filelen, mFileId, mRdata.getPtr());

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setI64(filelen);
	
	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	return true;
}
#endif

