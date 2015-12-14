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
// 01/27/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_Siteid_h
#define Omn_SEUtil_Siteid_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/String.h"


class AosSiteid
{
public:
	inline static u32 getDftSiteid()
	{
		return 100;
	}
	
	inline static u64 combineSiteid(const u64 &id, const u32 &siteid)
	{
		return ((id & 0xFF000000FFFFFFFFLL) | (((u64)(siteid & 0x00FFFFFFLL)) << 32));
	}

	inline static u64 separateSiteid(const u64 &id, u32 &siteid)
	{
		siteid = (id >> 32) & 0x00FFFFFFLL;
		return (id & 0xFF000000FFFFFFFFLL);
	}
};
#endif

