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
// Modification History:
// 10/10/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/CubicTrans.h"

AosCubicTrans::AosCubicTrans(
		const AosTransType::E type,
		const bool reg_flag)
:
AosTrans(type, reg_flag)
{
	setIsCubeTrans();
}

AosCubicTrans::AosCubicTrans(
		const AosTransType::E type,
		const u64 dist_id,
		const bool need_save,
		const bool need_resp)
:
AosTrans(type, 0, AosProcessType::eCube,
		AosGetCubeGrpIdByCubeId(initCubeId(dist_id)), need_save, need_resp),
mCubeId(initCubeId(dist_id)),
mCubeGrpId(AosGetCubeGrpIdByCubeId(mCubeId))
{
	setIsCubeTrans();
}

AosCubicTrans::AosCubicTrans(
		const AosTransType::E type,
		const u32 cube_id,
		const bool is_cube,
		const bool need_save,
		const bool need_resp)
:
AosTrans(type, 0, AosProcessType::eCube,
		AosGetCubeGrpIdByCubeId(cube_id), need_save, need_resp),
mCubeId(cube_id),
mCubeGrpId(AosGetCubeGrpIdByCubeId(mCubeId))
{
	setIsCubeTrans();
}


AosCubicTrans::~AosCubicTrans()
{
}

int
AosCubicTrans::initCubeId(const u64 dist_id)
{
	u32 vir_num = AosGetNumCubes();
	if(vir_num == 0 || dist_id == 0)	return -1;

	return dist_id % vir_num;
}


bool 
AosCubicTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTrans::serializeFrom(buff); 
	aos_assert_r(rslt, false);

	mCubeId = buff->getU32(0);
	return true;
}


bool
AosCubicTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTrans::serializeTo(buff); 
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeId);
	return true;
}

	
u64
AosCubicTrans::getSendKey()
{
	u64 key = 1;
	key = (key << 63) + mCubeId;
	return key;
}
	
u32
AosCubicTrans::getNeedAckNum()
{
	if(isNeedSave())	return AosGetCubeBkpNum();
	return AosTrans::getNeedAckNum();
}

