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
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SEInterfaces_VirtualFileType_h
#define Aos_SEInterfaces_VirtualFileType_h

#include "Util/String.h"

#define AOSVIRTUALFILE_SIMPLE			"simple"
#define AOSVIRTUALFILE_INVALID			"invalid"


class AosVirtualFileType
{
public:
	enum E
	{
		eInvalid, 

		eSimple,

		eMax
	};


public:
	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &id);
	static OmnString toStr(const E code);
	static bool check();
};
#endif
#endif
