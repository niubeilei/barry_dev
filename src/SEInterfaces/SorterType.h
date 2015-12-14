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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SorterType_h
#define Aos_SEInterfaces_SorterType_h

#include "Util/String.h" 

#define AOSSORTER_INVALID					"invalid"
#define AOSSORTER_MULTIFILE					"multifile"

class AosSorterType
{
private:
	static bool		smInited;

public:
	enum E
	{
		eInvalid, 

		eMultiFiles,

		eMax
	};

	static E toEnum(const OmnString &name);
	static OmnString  toString(const E type);
	static bool isValid(const E code) 
	{
		return code > eInvalid && code < eMax;
	}
};

#endif

