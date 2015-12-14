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
// 2015/01/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/AlarmIDMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"

static bool sgInited = false;
OmnMutexType    		AosAlarmIDMgr::smLock;
AosAlarmIDMgr::map_t  	AosAlarmIDMgr::smMap;
vector<OmnString> 		AosAlarmIDMgr::smFiles;
vector<int>		 		AosAlarmIDMgr::smLines;



u32
AosAlarmIDMgr::getAlarmID(const char *fname, const int line)
{
	if (!sgInited) OmnInitMutex(smLock);

	OmnMutexLock(smLock);
	OmnString key;
	key << fname << "_" << line;
	mapitr_t itr = smMap.find(key);
	if (itr != smMap.end())
	{
		u32 id = itr->second;
		OmnMutexUnlock(smLock);
		return id;
	}

	u32 id = smFiles.size();
	smMap[key] = id;
	smFiles.push_back(fname);
	smLines.push_back(line);
	OmnMutexUnlock(smLock);
	return id;
}


bool 
AosAlarmIDMgr::getAlarmInfo(
		const u64 alarm_id, 
		OmnString &fname, 
		int &line)
{
	if (!sgInited) OmnInitMutex(smLock);

	aos_assert_r(alarm_id < smFiles.size(), false);
	OmnMutexLock(smLock);
	fname = smFiles[alarm_id];
	line = smLines[alarm_id];
	OmnMutexUnlock(smLock);
	return true;
}


