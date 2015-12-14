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
// 06/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Groupby_GroupbyTypeType_h
#define Aos_Groupby_GroupbyTypeType_h

#include "Util/String.h"

#define AOSGROUPBYTYPE_INVALID			"invalid"
#define AOSGROUPBYTYPE_NORM				"norm"


class AosGroupbyType
{
public:
	enum E
	{
		eInvalid, 

		eNorm,

		eMax
	};


public:
	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &id);
	static bool addName(const OmnString &name, const E e);
	static bool check();
	static bool init();
};
#endif

