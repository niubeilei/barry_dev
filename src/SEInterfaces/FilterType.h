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
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_FilterType_h
#define Aos_SEInterfaces_FilterType_h

#include "Util/String.h"

#define AOSFILTER_SIMPLE					"simple"
#define AOSFILTER_INVALID					"invalid"


class AosFilterType
{
public:
	enum E
	{
		eInvalid, 

		eSimple,

		eMax
	};


public:
	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &id);
	static OmnString toStr(const E code);
	static bool check();
};
#endif

