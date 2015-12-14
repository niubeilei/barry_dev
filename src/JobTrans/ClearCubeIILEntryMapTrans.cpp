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
// 2015/12/04	Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/ClearCubeIILEntryMapTrans.h"

#include "API/AosApi.h"
#include "IILEntryMap/IILEntryMapMgr.h"


AosClearCubeIILEntryMapTrans::AosClearCubeIILEntryMapTrans(const bool regflag)
:
AosDocTrans(AosTransType::eClearCubeIILEntryMap, regflag)
{
}


AosClearCubeIILEntryMapTrans::AosClearCubeIILEntryMapTrans(const u32 cube_id)
:
AosDocTrans(AosTransType::eClearCubeIILEntryMap, cube_id + AosGetNumCubes(), false, true, 0)
{
}


AosClearCubeIILEntryMapTrans::~AosClearCubeIILEntryMapTrans()
{
}


bool
AosClearCubeIILEntryMapTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosClearCubeIILEntryMapTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosTransPtr
AosClearCubeIILEntryMapTrans::clone()
{
	return OmnNew AosClearCubeIILEntryMapTrans(false);
}


bool
AosClearCubeIILEntryMapTrans::proc()
{
	AosIILEntryMapMgr::clear();
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	sendResp(resp_buff);
	return true;
}

