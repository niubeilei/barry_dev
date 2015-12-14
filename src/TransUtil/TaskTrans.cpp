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
#include "TransUtil/TaskTrans.h"

#include "TransUtil/TransProcThrd.h"
#include "API/AosApi.h"

AosTransProcThrdPtr 	AosTaskTrans::smTaskProcThrd = OmnNew AosTransProcThrd("taskTrans", 16);

AosTaskTrans::AosTaskTrans(
		const AosTransType::E type,
		const bool reg_flag)
:
AosTrans(type, reg_flag)
{
}


AosTaskTrans::AosTaskTrans(
		const AosTransType::E type,
		const int svr_id,
		const u32 to_proc_id,
		const bool need_save,
		const bool need_resp)
:
AosTrans(type, svr_id, to_proc_id, need_save, need_resp)
{
}

AosTaskTrans::AosTaskTrans(
		const AosTransType::E type,
		const int svr_id,
		const AosProcessType::E to_proc_tp,
		const int cube_grp_id,
		const bool need_save,
		const bool need_resp)
:
AosTrans(type, svr_id, to_proc_tp, cube_grp_id, need_save, need_resp)
{
}


AosTaskTrans::AosTaskTrans(
		const AosTransType::E type,
		const int svr_id,
		const AosProcessType::E to_proc_tp,
		const bool need_save,
		const bool need_resp)
:
AosTrans(type, svr_id, to_proc_tp, -1, need_save, need_resp)
{
	if(to_proc_tp == AosProcessType::eCube)	OmnAlarm << "ShouldNeverComeHere" << enderr;
}


AosTaskTrans::AosTaskTrans(
		const AosTransType::E type,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTrans(type, svr_id, AosProcessType::eFrontEnd, -1, need_save, need_resp)
{
}


AosTaskTrans::~AosTaskTrans()
{
}


bool 
AosTaskTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTrans::serializeFrom(buff); 
	aos_assert_r(rslt, false);

	return true;
}


bool
AosTaskTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTrans::serializeTo(buff); 
	aos_assert_r(rslt, false);
	
	return true;
}

u64
AosTaskTrans::getSendKey()
{
	int to_svr_id = getToSvrId();
	aos_assert_r(to_svr_id >=0, 0);

	u64 key = to_svr_id;
	key = (key << 32) + getToProcId();
	return key;
}

bool	
AosTaskTrans::directProc()
{
	u32 idx = rand() % smTaskProcThrd->getNumThrds();
	AosTransPtr thisptr(this, false);
	return smTaskProcThrd->addRequest(idx, thisptr);	
}

