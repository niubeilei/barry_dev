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
#include "SysMsg/GetMaxRecvedFmtIdTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/FmtMgrObj.h"

AosGetMaxRecvedFmtIdTrans::AosGetMaxRecvedFmtIdTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetMaxRecvedFmtId, regflag)
{
	setIsSystemTrans();
}


AosGetMaxRecvedFmtIdTrans::AosGetMaxRecvedFmtIdTrans(
		const int svr_id,
		const u32 cube_grp_id)
:
AosTaskTrans(AosTransType::eGetMaxRecvedFmtId, svr_id,
		AosProcessType::eCube, cube_grp_id, false, false)
{
	setIsSystemTrans();
}


AosGetMaxRecvedFmtIdTrans::~AosGetMaxRecvedFmtIdTrans()
{
}


bool
AosGetMaxRecvedFmtIdTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosGetMaxRecvedFmtIdTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosGetMaxRecvedFmtIdTrans::clone()
{
	return OmnNew AosGetMaxRecvedFmtIdTrans(false);
}


bool
AosGetMaxRecvedFmtIdTrans::proc()
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);

	u64 max_recved_fmtid = fmt_mgr->getMaxRecvedFmtId(); 
	
	AosBuffPtr resp = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
	resp->setU64(max_recved_fmtid);

	sendResp(resp);
	return true;
}


