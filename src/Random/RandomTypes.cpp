////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Random.h
// Description:
//
// Modification History:
// 07/07/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomTypes.h"

#include "Util/String.h"

AosTimeStrFormat_e AosGetTimeStrEnum(const OmnString &name)
{
	if (name == "HH")
	{
		return eAosTimeStrFormat_HH;
	}

	if (name == "HH:MM")
	{
		return eAosTimeStrFormat_HHMM;
	}
	
	if (name == "HH:MM:SS")
	{
		return eAosTimeStrFormat_HHMMSS;
	}

	return eAosTimeStrFormat_Invalid;	
};


