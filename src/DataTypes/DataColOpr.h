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
// 10/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/String.h"

#ifndef Aos_DataTypes_DataColOpr_h
#define Aos_DataTypes_DataColOpr_h

#define AOSDATACOLOPR_INVALID				"invalid"
#define AOSDATACOLOPR_INDEX					"index"
#define AOSDATACOLOPR_NORMAL				"norm"
#define AOSDATACOLOPR_NOUPDATE				"noupdate"
#define AOSDATACOLOPR_SET_ON_DAY			"setonday"
#define AOSDATACOLOPR_MAX_ON_DAY			"maxonday"
#define AOSDATACOLOPR_MIN_ON_DAY			"minonday"
#define AOSDATACOLOPR_AVERAGE				"average"
#define AOSDATACOLOPR_SUM_VALUE				"sum"
#define AOSDATACOLOPR_MIN_VALUE				"min"
#define AOSDATACOLOPR_MAX_VALUE				"max"
#define AOSDATACOLOPR_SET_VALUE				"set"
#define AOSDATACOLOPR_COUNT_VALUE			"count"
class AosDataColOpr
{
public:
	enum E
	{
		eInvalid,

		eIndex,
		eNormal,
		eNoUpdate,
		eSetOnEpochDay,
		eMaxOnEpochDay,
		eMinOnEpochDay,
		eAverage,
		eMaxValue,
		eMinValue,
		eCountValue,
		eSetValue,

		eMax
	};

	static E toEnum(const OmnString &name);
	static OmnString toStr(const E code);
	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
};
#endif
