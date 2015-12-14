////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// Created: 05/08/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/SyncUnfinishTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/CubeTransSvrObj.h"

AosSyncUnfinishTrans::AosSyncUnfinishTrans(const bool reg_flag)
:
AosTaskTrans(AosTransType::eSyncUnfinishTrans, reg_flag)
{
	setIsSystemTrans(); 
}


AosSyncUnfinishTrans::AosSyncUnfinishTrans(
		const int to_svrid,
		const u32 cube_grp_id,
		const int read_id)
:
AosTaskTrans(AosTransType::eSyncUnfinishTrans, to_svrid, 
		AosProcessType::eCube, cube_grp_id, false, false),
mReadId(read_id)
{
	setIsSystemTrans(); 
}


AosSyncUnfinishTrans::~AosSyncUnfinishTrans()
{
}


bool
AosSyncUnfinishTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mReadId = buff->getInt(-1);
	return true;
}


bool
AosSyncUnfinishTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mReadId);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosSyncUnfinishTrans::clone()
{
	return OmnNew AosSyncUnfinishTrans(false);
}


bool
AosSyncUnfinishTrans::proc()
{
	AosCubeTransSvrObjPtr trans_svr = AosCubeTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);
	
	AosBuffPtr trans_buffs;
	bool finish = false;
	bool rslt = trans_svr->readTransBySync(trans_buffs, mReadId, finish);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}

	AosBuffPtr resp = OmnNew AosBuff(100, 0 AosMemoryCheckerArgs);
	resp->setU8(rslt);
	resp->setU8(finish);
	resp->setInt(mReadId);
	if(trans_buffs)
	{
		resp->setU32(trans_buffs->dataLen());
		resp->setBuff(trans_buffs);
	}
	
	sendResp(resp);
	return false;
}


