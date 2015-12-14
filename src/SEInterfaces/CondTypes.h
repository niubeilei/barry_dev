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
// 05/10/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ConditionType_h
#define Aos_SEInterfaces_ConditionType_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Conds/AllConds.h"
#include "Thread/Mutex.h"
#include "Util/String.h" 
#include "Util/HashUtil.h"
#include <vector>

using namespace std;

#define AOSCONDTYPE_INVALID					"invalid"
#define AOSCONDTYPE_AND						"and"
#define AOSCONDTYPE_OR						"or"
#define AOSCONDTYPE_ARITH					"arith"
#define AOSCONDTYPE_IN						"in"
#define AOSCONDTYPE_TRUE					"true"
#define AOSCONDTYPE_FALSE					"false"
#define AOSCONDTYPE_USERDOMAINS				"userdomains"
#define AOSCONDTYPE_USERGROUPS				"usergroups"
#define AOSCONDTYPE_USERROLES				"userroles"
#define AOSCONDTYPE_USERLISTING				"userlisting"
#define AOSCONDTYPE_DOCEXIST				"docexist"
#define AOSCONDTYPE_CHARSTRMAP				"charstrmap"

#define AOSCONDTYPE_CUSTOMIZE001			"customize001"

#define AOSCONDTYPE_IILMAP					"iilmap"		// Ken Lee, 2013/01/26		
#define AOSCONDTYPE_EMPTY					"empty"			// Ken Lee, 2013/01/26		
#define AOSCONDTYPE_REGEXP					"regexp"  		// Andy Zhang, 2013/07/26	
#define AOSCONDTYPE_EXPR					"expr"   		// Andy Zhang, 2013/12/17	

struct AosCondType
{
	enum E
	{
		eInvalid, 
	
		eAnd, 
		eOr,
		eIn,
		eArith,
		eTrue,
		eFalse,
		eUserDomains,
		eUserGroups,
		eUserRoles,
		eUserListing,
		eDocExist,
		eCharStrMap,
		eCustomize001,

		eIILMap,		// Ken Lee, 2013/01/26
		eEmpty,			// Ken Lee, 2013/01/26
		eRegExp,		// Andy Zhang, 2013/07/26
		eExpr,			// Andy Zhang, 2013/12/17

		eMax
	};

	static OmnMutex 	smLock;
	static AosStr2U32_t	smNameMap;
	static OmnString 	smEnumMap[eMax];
	static bool			smInited;

	static bool addName(const OmnString &name, const E type)
	{
		aos_assert_r(name != "", false);
		aos_assert_r(isValid(type), false);
		smLock.lock();
		AosStr2U32Itr_t itr = smNameMap.find(name);
		if (itr != smNameMap.end())
		{
			smLock.unlock();
			OmnAlarm << "Condition name reused: " << name << ":" << type << enderr;
			return false;
		}
		smNameMap[name] = type;
		smEnumMap[type] = name;
		smLock.unlock();
		return true;
	}

	static E toEnum(const OmnString &name)
	{
		//if (!smInited) 
		//{
		//	smInited = true;
		//	AosInitConditions();
		//}

		if (name.length() < 1) return eInvalid;
		AosStr2U32Itr_t itr = smNameMap.find(name);
		if (itr == smNameMap.end()) return eInvalid;
		return (E) itr->second;
	}

	static bool isValid(const E type)
	{
		//if (!smInited) 
		//{
		//	smInited = true;
		//	AosInitConditions();
		//}

		return (type > eInvalid && type < eMax);
	}

	static OmnString toStr(const E type)
	{
		//if (!smInited) 
		//{
		//	smInited = true;
		//	AosInitConditions();
		//}

		if (!isValid(type)) return "";
		return smEnumMap[type];
	}

	inline static int getNumValidEntries() {return eMax - eAnd;}
	inline static E getFirstEntry() {return eAnd;}
	static E getRandomType();
};

#endif

