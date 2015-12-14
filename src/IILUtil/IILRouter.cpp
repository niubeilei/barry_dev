////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILUtil/IILRouter.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


bool 
AosIILRouter::getPhysicalServerId(const u64 &iilid, const AosRundataPtr &rdata)
{
	int num_virtuals = AosGetNumCubes();
	int num_physicals = AosGetNumPhysicals();
	aos_assert_rr(num_virtuals, rdata, false);
	aos_assert_rr(num_physicals, rdata, false);
	return iilid % num_virtuals % num_physicals;
}

