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
// 05/29/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Groupby_GroupRcdPicke_h
#define Aos_Groupby_GroupRcdPicke_h

#include "Rundata/Ptrs.h"
#include "Groupby/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

#define AOSGROUPBYOPR_INVALID				"invalid"
#define AOSGROUPBYOPR_NONE					"none"
#define AOSGROUPBYOPR_FIRST					"first"
#define AOSGROUPBYOPR_LAST					"last"
#define AOSGROUPBYOPR_FIRSTCOND				"firstcond"
#define AOSGROUPBYOPR_LASTCOND				"lastcond"

class AosGroupRcdPicker
{
public:
	enum E
	{
		eInvalid,

		eNone,
		eFirst,
		eLast,
		eFirstCond,
		eLastCond,

		eMax
	};

	static inline bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static OmnString toStr(const E code);
	static E toEnum(const OmnString &name);
	static bool addName(const OmnString &name, const E code);
	static bool init();
};
#endif

