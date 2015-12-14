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
// 07/23/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSampler_DataSamplerType_h
#define Aos_DataSampler_DataSamplerType_h

#include "Util/String.h"

#define AOSDATASAMPLERTYPE_INVALID			"invalid"
#define AOSDATASAMPLERTYPE_FRONT			"front"
#define AOSDATASAMPLERTYPE_BACK				"back"
#define AOSDATASAMPLERTYPE_INTERVAL			"interval"
#define AOSDATASAMPLERTYPE_RANDOM			"random"

class AosDataSamplerType 
{
public:
	enum E
	{
		eInvalid,

		eFront,
		eBack,
		eInterval,
		eRandom,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	
	static bool isValid(const OmnString &code)
	{
		return isValid(toEnum(code));
	}
	
	static E toEnum(const OmnString &code);
	static OmnString toString(const E code);
	static bool check();
	static bool addName(const OmnString &name, const E code);
};

#endif
