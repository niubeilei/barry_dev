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
#include "TransUtil/AdminTrans.h"

#include "API/AosApi.h"

AosAdminTrans::AosAdminTrans(
		const AosTransType::E type,
		const bool reg_flag)
:
AosTaskTrans(type, reg_flag)
{
	setIsAdminTrans();
}


AosAdminTrans::AosAdminTrans(
		const AosTransType::E type,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(type, svr_id, need_save, need_resp)
{
	setIsAdminTrans();
}


AosAdminTrans::AosAdminTrans(
		const AosTransType::E type,
		const int svr_id,
		const u32 to_proc_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(type, svr_id, to_proc_id, need_save, need_resp)
{
	setIsAdminTrans();
}


AosAdminTrans::~AosAdminTrans()
{
}


bool 
AosAdminTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff); 
	aos_assert_r(rslt, false);
	return true;
}


bool
AosAdminTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff); 
	aos_assert_r(rslt, false);
	return true;
}


