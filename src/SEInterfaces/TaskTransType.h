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
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_TaskTransType_h
#define AOS_SEInterfaces_TaskTransType_h

#include "Util/String.h"

#define AOSTASKTRANSTYPE_INVALID		"invalid"
#define AOSTASKTRANSTYPE_ONE_PHASE		"onephase"
#define AOSTASKTRANSTYPE_TWO_PHASE		"twophase"

class AosTaskTransType
{
public:
	enum E
	{
		eInvalid,

		eOnePhase,
		eTwoPhase, 

		eMax
	};

	static OmnString		smNames[eMax];

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static OmnString toStr(const E id)
	{
		if (id <= eInvalid || id >= eMax) return AOSTASKTRANSTYPE_INVALID;
		return smNames[id];
	}
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
};

#endif
