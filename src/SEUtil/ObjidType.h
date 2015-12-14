////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 01/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_ObjidType_h
#define AOS_SEUtil_ObjidType_h

#include "Util/HashUtil.h"
#include "SEUtil/ReservedObjids.h"

struct AosObjidType
{
	enum E
	{
		eInvalid,

		eNormal,

		eDomainOprArd,
		eUserOprArd,
		eOprDocCtnr,
		eDftUserOprDoc,
		eSystem,
		eStmcInstDoc,
		eDictCtnr,

		eMax
	};

	static AosStr2U32_t		smNameMap;
	static bool				smStatus[eMax];
	static bool				smInited;

	static bool isReservedObjid(const E code)
	{
		return code > eNormal && code < eMax;
	}
	static bool init();
	static E toEnum(const OmnString &name)
	{
		if (!smInited) init();
		AosStr2U32Itr_t itr = smNameMap.find(name);
		if (itr == smNameMap.end()) return eInvalid;
		return (E)itr->second;
	}
	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
};
#endif

