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
// 2014/07/21	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StatTrans/CreateStatMetaFileTrans.h"

#include "SEInterfaces/StatSvrObj.h"

AosCreateStatMetaFileTrans::AosCreateStatMetaFileTrans(const bool regflag)
:
AosStatTrans(AosTransType::eCreateStatMetaFile, regflag)
{
}


AosCreateStatMetaFileTrans::AosCreateStatMetaFileTrans(
		const u32 cube_id,
		const OmnString &file_prefix)
:
AosStatTrans(AosTransType::eCreateStatMetaFile, cube_id, false, true),
mCubeId(cube_id),
mFilePrefix(file_prefix)
{
}


AosCreateStatMetaFileTrans::~AosCreateStatMetaFileTrans()
{
}


bool
AosCreateStatMetaFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosStatTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeId = buff->getU32(0);
	mFilePrefix = buff->getOmnStr("");
		
	return true;
}


bool
AosCreateStatMetaFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosStatTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeId);
	buff->setOmnStr(mFilePrefix);
	return true;
}


AosTransPtr
AosCreateStatMetaFileTrans::clone()
{
	return OmnNew AosCreateStatMetaFileTrans(false);
}


bool
AosCreateStatMetaFileTrans::proc()
{
	AosStatSvrObjPtr stat_svr = AosStatSvrObj::getStatSvr();
	if(!stat_svr)	return setErrResp();

	u64 file_id = stat_svr->createStatMetaFile(mRdata, mCubeId, mFilePrefix);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU64(file_id);
	sendResp(resp_buff);
	return true;
}


bool
AosCreateStatMetaFileTrans::setErrResp()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	resp_buff->setU64(0);
	return true;
}


