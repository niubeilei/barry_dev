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
// 2015/05/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoPod/JimoPod.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/ErrorMsg.h"

AosJimoPod::AosJimoPod()
:
mStoreSize(-1)
{
}


AosJimoPod::AosJimoPod(const int store_size)
:
mStoreSize(store_size)
{
	aos_assert_rh(store_size > 0,
			AosErrMsg("invalid_store_size")
			.field("store_size", store_size));
	OmnThrowException("invalid_store_size");
	return;
}


AosJimoPod::~AosJimoPod()
{
}


u32 
AosJimoPod::getJSID(const u64 docid) const
{
	return ((docid & 0x00ffffffffffffffULL) >> (56 - mStoreSize));
}
