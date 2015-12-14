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
// 12/26/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetFileInfoTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosGetFileInfoTrans::AosGetFileInfoTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetFileInfo, regflag)
{
}


AosGetFileInfoTrans::AosGetFileInfoTrans(
		const OmnString &file_name,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetFileInfo, svr_id, false, true),
mFileName(file_name),
mServerId(svr_id)
{
}


AosGetFileInfoTrans::~AosGetFileInfoTrans()
{
}


bool
AosGetFileInfoTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileName= buff->getOmnStr("");
	mServerId = buff->getInt(-1);
	return true;
}


bool
AosGetFileInfoTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mFileName);
	buff->setInt(mServerId);
	return true;
}


AosTransPtr
AosGetFileInfoTrans::clone()
{
	return OmnNew AosGetFileInfoTrans(false);
}


bool
AosGetFileInfoTrans::proc()
{
	AosFileInfo file_info;
	bool rslt = AosNetFileMgrObj::getFileInfoLocalStatic(
		mFileName, mServerId, file_info, mRdata.getPtr());

	AosBuffPtr temp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	if (rslt) rslt = file_info.serializeTo(temp_buff);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	if (rslt) resp_buff->setBuff(temp_buff);

	sendResp(resp_buff);
	return true;
}

