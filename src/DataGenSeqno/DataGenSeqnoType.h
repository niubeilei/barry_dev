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
#ifndef Aos_DataGenSeqno_DataGenSeqnoType_h
#define Aos_DataGenSeqno_DataGenSeqnoType_h

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/String.h" 
#include "Util/HashUtil.h"

#define AOSDATGGENTYPE_ARABIC_SEQNO				"arabicseqno"
#define AOSDATGGENTYPE_ROMAN_SEQNO				"romanseqno"
#define AOSDATGGENTYPE_CHAR_SEQNO				"charseqno"
#define AOSDATGGENTYPE_CHINESE_SEQNO			"chineseseqno"
#define AOSDATGGENTYPE_EARTHLY_SEQNO			"earthlyseqno"
#define AOSDATGGENTYPE_HEAVENLY_SEQNO			"heavenlyseqno"

struct AosDataGenSeqnoType 
{
private:
	static OmnMutex         smLock;
	static AosStr2U32_t     smNameMap;

public:
	enum E
	{
		eInvalid, 
		
		eArabicSeqno,
		eRomanSeqno,
		eCharSeqno,
		eChineseSeqno,
		eEarthlyBranchesSeqno,
		eHeavenlyStemsSeqno,

		eMax
	};


	static bool isValid(const E type)
	{
		 return (type > eInvalid && type < eMax);
	}

	static bool addName(const OmnString &name, const E type)
	{
//		AosStr2U32Itr_t itr = smNameMap.find(name);
//		if (itr != smNameMap.end())
//		{
//			smLock.unlock();
//			OmnAlarm << "Doc Selector name reused: " << name << ":" << type << enderr;
//			return false;
//		}
		aos_assert_r(name != "", false);
		aos_assert_r(type != eInvalid, false);
		smLock.lock();
		smNameMap[name] = type;
		smLock.unlock();
		return true;
	}

	static E toEnum(const OmnString &name)
	{
		if (name.length() < 1) return eInvalid;
		AosStr2U32Itr_t itr = smNameMap.find(name);
		if (itr == smNameMap.end()) return eInvalid;
		return (E) itr->second;
	}
};

#endif

