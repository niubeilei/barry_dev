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
// 09/28/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TimeSelector_TimeSelectorType_h
#define Aos_TimeSelector_TimeSelectorType_h

#include "Thread/Mutex.h"
#include "Util/String.h" 
#include "Util/HashUtil.h"

#define AOSTIMESELTYPE_ADD				"add"
#define AOSTIMESELTYPE_GETEPOCH			"getepoch"
#define AOSTIMESELTYPE_OFDAYHOUR2SEC	"ofdayh2s"
#define AOSTIMESELTYPE_SUB				"sub"
#define AOSTIMESELTYPE_STR2EPOCH		"str2epoch"
#define AOSTIMESELTYPE_EPOCH2STR		"epoch2str"
#define AOSTIMESELTYPE_MONTHWEEK2EPOCH	"monthweek2epoch"

struct AosTimeSelectorType 
{
private:
	static OmnMutex         smLock;
	static  AosStr2U32_t    smNameMap55;

public:
	enum E
	{
		eInvalid, 
		
		eAdd,
		eGetEpoch,
		eOfDayHour2Sec,
		eMonthWeek2Epoch,
		eSub,
		eStr2Epoch,
		eEpoch2Str,

		eMax
	};

	static bool isValid(const E type)
	{
		 return (type > eInvalid && type < eMax);
	}

	static bool addName(const OmnString &name, const E type)
	{
OmnScreen << "TimeSelector add: " << name << endl;
		smLock.lock();
		if (!smNameMap55.empty())
		{
			AosStr2U32Itr_t itr = smNameMap55.find(name);
			if (itr != smNameMap55.end())
			{
				smLock.unlock();
				OmnAlarm << "TimeSelector name reused: " << name << ":" << type << enderr;
				return false;
			}
		}
		smNameMap55[name] = type;
		smLock.unlock();
		return true;
	}

	static E toEnum(const OmnString &name)
	{
		if (name.length() < 1) return eInvalid;
		if (smNameMap55.empty()) return eInvalid;
		AosStr2U32Itr_t itr = smNameMap55.find(name);
		if (itr == smNameMap55.end()) return eInvalid;
		return (E) itr->second;
	}
};

#endif

