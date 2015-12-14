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
// 2013/05/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_MethodId_h
#define Aos_Jimo_MethodId_h

#include "Util/String.h"


#define AOSMETHODNAME_INVALID				"method_invalid"
#define AOSMETHODNAME_BASIC001				"method_basic_001"
#define AOSMETHODNAME_BASIC002				"method_basic_002"

struct AosMethodId 
{
	enum E 
	{
		eInvalidMethodId,

		eAosRundata_Jimo,
		eAosRundata_Jimo_WorkerDoc,
		eAosRundata_Worker,

		eMaxMethodId
	};

	static bool isValid(const E code)
	{
		return code > eInvalidMethodId && code < eMaxMethodId;
	}

	static E toEnum(const OmnString &name);
	static OmnString toName(const E code);

private:
	static bool init();
};

#endif

