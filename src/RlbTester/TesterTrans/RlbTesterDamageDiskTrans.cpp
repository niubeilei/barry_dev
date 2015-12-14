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
#include "RlbTester/TesterTrans/RlbTesterDamageDiskTrans.h"

#include "API/AosApi.h"

AosRlbTesterDamageDiskTrans::AosRlbTesterDamageDiskTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eRlbTesterDamageDisk, regflag)
{
}


AosRlbTesterDamageDiskTrans::AosRlbTesterDamageDiskTrans(
		const int svr_id,
		const u32 proc_id)
:
AosTaskTrans(AosTransType::eRlbTesterDamageDisk, 
		svr_id, proc_id, false, false)
{
}


AosRlbTesterDamageDiskTrans::~AosRlbTesterDamageDiskTrans()
{
}


bool
AosRlbTesterDamageDiskTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosRlbTesterDamageDiskTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	return true;
}


AosTransPtr
AosRlbTesterDamageDiskTrans::clone()
{
	return OmnNew AosRlbTesterDamageDiskTrans(false);
}


bool
AosRlbTesterDamageDiskTrans::proc()
{
	AosSetDiskErrTest();
	return true;
}


