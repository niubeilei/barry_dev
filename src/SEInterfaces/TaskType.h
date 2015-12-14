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
// 04/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_TaskType_h
#define AOS_SEInterfaces_TaskType_h

#include "Util/String.h"

#define AOSTASKNAME_INVALID			"invalid"
#define AOSTASKNAME_REDUCETASK		"reduce_task"
#define AOSTASKNAME_MAPTASK			"map_task"
#define AOSTASKNAME_NORMTASK		"norm_task"

class AosTaskType
{
public:
	enum E
	{
		eInvalid,

		eMapTask,
		eReduceTask,
		eNormTask,

		eMax
	};

	static OmnString		smNames[eMax];

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static OmnString toStr(const E id)
	{
		if (id <= eInvalid || id >= eMax) return AOSTASKNAME_INVALID;
		return smNames[id];
	}
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
};

#endif
