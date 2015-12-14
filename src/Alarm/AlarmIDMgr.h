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
#ifndef Omn_Alarm_AlarmIDMgr_h
#define Omn_Alarm_AlarmIDMgr_h

#include "aosUtil/Types.h"
#include "Util/HashUtil.h"
#include "Util/OmnNew.h"


class AosAlarmIDMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, u32, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, u32, Omn_Str_hash, compare_str>::iterator mapitr_t;

private:
	static OmnMutexType			smLock;
	static map_t				smMap;
	static vector<OmnString>	smFiles;
	static vector<int>			smLines;

public:
	static bool init();
	static u32 getAlarmID(const char *fname, const int line);
	static bool getAlarmInfo(const u64 alarm_id, 
					OmnString &fname, 
					int &line);
};

#endif

