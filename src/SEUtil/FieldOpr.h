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
// 05/23/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_FieldOpr_h
#define Omn_SEUtil_FieldOpr_h

#include "Util/Ptrs.h"
#include "Util/String.h"

#define AOSDBOPR_INVALID			"invalid"
#define AOSDBOPR_FIRST				"first"
#define AOSDBOPR_LAST				"last"
#define AOSDBOPR_SUM				"sum"
#define AOSDBOPR_MIN				"min"
#define AOSDBOPR_MAX				"max"
#define AOSDBOPR_AVERAGE			"average"
#define AOSDBOPR_COUNT				"count"
#define AOSDBOPR_FIELD				"field"
#define AOSDBOPR_NONE				"none"

class AosFieldOpr
{
public:
	enum E
	{
		eInvalid,

		eNone,
		eFirst,
		eLast,
		eSum,
		eMinimum,
		eMaximum,
		eAverage,
		eCount,
		eField,

		eMaxEntry
	};

	static inline bool isValid(const E code)
	{
		return code > eInvalid && code < eMaxEntry;
	}

	static OmnString toStr(const E code);
	static E toEnum(const OmnString &name);
	static bool addName(const OmnString &name, const E code);
	static bool init();
};
#endif

