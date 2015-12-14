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
// An ID is defined by a doc. 
//
// Modification History:
// 01/06/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AppIdGens/SysIdGenU64.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"



AosSysIdGenU64::AosSysIdGenU64(
		const AosRundataPtr &rdata,
		const OmnString &objid)
:
AosAppIdGenU64(rdata, objid)
{
	mIILName = getIILName();
}


AosSysIdGenU64::AosSysIdGenU64(
		const AosRundataPtr &rdata,
		const OmnString &objid, 
		const u64 &block_size,
		const u64 &init_value,
		const u64 &max_id,
		const u64 &prefix)
:
AosAppIdGenU64(rdata, objid, block_size, init_value, max_id, prefix)
{
	mIILName = getIILName();
}


AosSysIdGenU64::~AosSysIdGenU64()
{
}


OmnString
AosSysIdGenU64::getIILName() const
{
	return AosIILName::composeSysIdGenIILName();
}

