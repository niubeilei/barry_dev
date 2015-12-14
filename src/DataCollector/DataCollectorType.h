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
#ifndef Aos_DataCollector_DataCollectorType_h
#define Aos_DataCollector_DataCollectorType_h

#include "Util/String.h"

#define AOSDATACOLLECTOR_INVALID			"invalid"
#define AOSDATACOLLECTOR_SORT				"sort"
#define AOSDATACOLLECTOR_GROUP				"group"
#define AOSDATACOLLECTOR_GROUP				"group"
#define AOSDATACOLLECTOR_MAX				"max"


class AosDataCollectorType
{
public:
	enum E
	{
		eInvalid, 
		
		eSort,
		eGroup,

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
};
#endif

