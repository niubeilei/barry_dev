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
#ifndef Aos_Groupby_GroupbyOpr_h
#define Aos_Groupby_GroupbyOpr_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

#define AOSGROUPBYOPR_INVALID				"invalid"
#define AOSGROUPBYOPR_FIRST					"first"
#define AOSGROUPBYOPR_LAST					"last"
#define AOSGROUPBYOPR_FIRSTCOND				"firstcond"
#define AOSGROUPBYOPR_LASTCOND				"lastcond"
#define AOSGROUPBYOPR_NONE					"none"

class AosGroupbyOpr
{
public:
	enum E
	{
		eInvalid,

		eFirst,
		eLast,
		eFirstCond,
		eLastCond,
		eNone,

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

