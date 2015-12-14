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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiT_h
#define AOS_API_ApiT_h

#include "Porting/GetTime.h"
#include "Util/String.h"
#include <vector>
using namespace std;

#define AOS_DECLARE_TIMESTAMPS(size, check) \
	static vector<u64> lsTimestamps(size); \
	static u64 lsCurrentTimestamp = OmnGetTimestamp();\
	static u32 lsTimestampCheckFreq = check; \
	static u32 lsTimestampTick = 0;

#define AOS_PRINT_TIMESTAMP \
	if (lsTimestampTick++ >= lsTimestampCheckFreq) \
	{ \
		AosPrintTimestamp(lsTimestamps);\
		lsTimestampTick = 0;\
	}

#define AOS_TIMESTAMP(idx) \
	if (idx >= 0 && (u32)idx < lsTimestamps.size()) \
		AosTakeTimestamp(lsTimestamps[idx], lsCurrentTimestamp)

inline void AosTakeTimestamp(u64 &time, u64 &running_time)
{
	u64 tt = OmnGetTimestamp();
	time += tt - running_time;
	running_time = tt;
}

inline void AosPrintTimestamp(const vector<u64> &timestamps)
{
	OmnScreen << "=============TimeProfiling=======================\n";
	char buff[100];
	for (u32 i=0; i<timestamps.size(); i++)
	{
		sprintf(buff, "%'llu", (long long unsigned int)timestamps[i]);

		OmnString ss(buff);
		ss.convertToScientific();
		cout << "        Time" << i << ": " << ss << "\n";
	} 
}

#endif
