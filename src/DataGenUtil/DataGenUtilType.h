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
#ifndef Aos_DataGenUtil_DataGenUtilType_h
#define Aos_DataGenUtil_DataGenUtilType_h

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/String.h" 
#include "Util/HashUtil.h"

#define AOSDATGGENTYPE_ENUM_RAND				"enumrand"
#define AOSDATGGENTYPE_SECTION_RAND				"sectionrand"
#define AOSDATGGENTYPE_SIMPLE_RAND				"simplerand"
#define AOSDATGGENTYPE_STRING_RAND				"stringrand"
#define AOSDATGGENTYPE_ENUM_RAND				"enumrand"
#define AOSDATGGENTYPE_COMPOSE_RAND				"compserand"	
#define AOSDATGGENTYPE_SEQNO_RAND				"seqnorand"	


struct AosDataGenUtilType 
{
private:
	static OmnMutex         smLock;
	static AosStr2U32_t     smNameMap1;

public:
	enum E
	{
		eInvalid, 
	
		eEnumRand,
		eSectionRand,
		eSimpleRand,
		eStringRand,
		eComposeRand,
		eSeqnoRand,

		eMax
	};


	static bool isValid(const E type)
	{
		 return (type > eInvalid && type < eMax);
	}

	static bool addName(const OmnString &name, const E type)
	{
		aos_assert_r(name != "", false);
//		E ee = toEnum(name);
		aos_assert_r(type != eInvalid, false);
		smLock.lock();
//		AosStr2U32Itr_t itr = smNameMap.find(name);
//		if (itr != smNameMap.end())
//		{
//			smLock.unlock();
//			OmnAlarm << "Doc Selector name reused: " << name << ":" << type << enderr;
//			return false;
//		}
		smNameMap1[name] = type;
//		AosStr2U32Itr_t itr;
//		for (itr = smNameMap1.begin(); itr != smNameMap1.end(); itr++)
//		{
//			cout <<"name: " <<itr->first << " type:"<<itr->second << endl;
//		}
		smLock.unlock();
		return true;
	}

	static E toEnum(const OmnString &name)
	{
		if (name.length() < 1) return eInvalid;
//		AosStr2U32Itr_t itr1;
//		for (itr1 = smNameMap1.begin(); itr1 != smNameMap1.end(); itr1++)
//		{
//			cout <<"name: " <<itr1->first << " type:"<<itr1->second << endl;
//		}
		AosStr2U32Itr_t itr = smNameMap1.find(name);
		if (itr == smNameMap1.end()) return eInvalid;
		return (E) itr->second;
	}
};

#endif

