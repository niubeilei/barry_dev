////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Time.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/String.h"
#include "Util1/Time.h"


i64			OmnTime::smCrtSec = 0;
u64			OmnTime::smCrtDay = 0;
u64			OmnTime::smAccumulatedMsec = 0;
u64			OmnTime::smStartSec = 0;
u64			OmnTime::smTimestamp = 0;		// Chen Ding, 2013/01/27
u64			OmnTime::smSecTick = 0;
uint		OmnTime::smLastSamplingMsec = 0;
OmnString	OmnTime::smTimeStr;

unsigned long volatile jiffies;
// OmnUint64 jiffies = 0;

bool
OmnTime::getRealtime(uint &sec, uint &usec)
{
	timeval time;
	OmnGetTimeOfDay(&time);
	sec = time.tv_sec;
	usec = time.tv_usec;
	return true;
}


u32
OmnTime::getCrtSecond()
{
	timeval time;
	OmnGetTimeOfDay(&time);
	return time.tv_sec;
}

