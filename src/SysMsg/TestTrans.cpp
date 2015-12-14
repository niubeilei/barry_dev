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
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/TestTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransSvrObj.h"

AosTestTrans::AosTestTrans(const bool regflag)
:
AosCubicTrans(AosTransType::eRlbTesterCreateFile, regflag)
{
}


AosTestTrans::AosTestTrans(
		const u32 cube_grp_id,
		const AosBuffPtr &fmt_buff)
:
AosCubicTrans(AosTransType::eRlbTesterCreateFile, cube_grp_id, true, true, false),
mCubeGrpId(cube_grp_id),
mFmtBuff(fmt_buff)
{
}


AosTestTrans::~AosTestTrans()
{
}


bool
AosTestTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeGrpId = buff->getU32(0);
	u32 log_len = buff->getU32(0);
	mFmtBuff = buff->getBuff(log_len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosTestTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeGrpId );
	buff->setU32(mFmtBuff->dataLen());
	buff->setBuff(mFmtBuff);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosTestTrans::clone()
{
	return OmnNew AosTestTrans(false);
}

bool	
AosTestTrans::directProc()
{
	AosTransSvrObj::getTransSvr()->resetCrtCacheSize(getSize());
	return true;
}


bool
AosTestTrans::proc()
{
	return true;
}


