////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This is a utility to select docs.
//
// Modification History:
// 04/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CounterUtil_CounterOperations_h
#define AOS_CounterUtil_CounterOperations_h

#include "Util/String.h"

#define AOSCOUNTEROPR_ADDCOUNTER				"add"
#define AOSCOUNTEROPR_ADDCOUNTER2				"add2"
#define AOSCOUNTEROPR_SETCOUNTER				"set"

class AosCounterOperation
{
public:
	enum E
	{
		eInvalid,

		eAddCounter,
		eSetCounter,
		eAddCounter2,

		eMax
	};

	static E toEnum(const OmnString &name);
	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
};
#endif
