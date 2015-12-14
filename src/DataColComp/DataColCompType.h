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
// 09/03/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataColComp_DataColCompType_h
#define Aos_DataColComp_DataColCompType_h

#include "Util/String.h"

#define AOSDATACOLCOMP_INVALID					"invalid"
#define AOSDATACOLCOMP_MAP						"map"

class AosDataColCompType 
{
public:
	enum E
	{
		eInvalid,
		eMap,

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
	
	static E toEnum(const OmnString &code)
	{
		if (code == AOSDATACOLCOMP_MAP) return eMap;
		return eInvalid;
	}
	
	static OmnString toString(const E code)
	{
		switch(code)
		{
		case eMap: 		return AOSDATACOLCOMP_MAP;
		default: break;
		}
		return AOSDATACOLCOMP_INVALID;
	}
	static bool check();
	static bool addName(const OmnString &name, const E code);
};

#endif
