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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/SysDefValObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"


AosSysDefValObj::AosSysDefValObj(const int version)
:
AosJimo(AosJimoType::eSysDefinedValue, version)
{
}


AosSysDefValObj::~AosSysDefValObj()
{
}


bool 
AosSysDefValObj::rsolveStatic(
		const AosRundataPtr &rdata, 
		const OmnString &val_name, 
		const OmnString &dft, 
		OmnString &value)
{
	aos_assert_rr(smObject, rdata, 0);
	return smObject->resolve(rdata, val_name, dft, value);
}


