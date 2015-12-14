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
// 05/10/2012 Created by Chen Ding
// 2012/11/12 Moved From IILUtil By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILScanner_IILMatchType_h
#define Aos_IILScanner_IILMatchType_h

#include "Util/String.h"


#define AOSIILMATCHTYPE_INVALID				"invalid"
#define AOSIILMATCHTYPE_KEY					"key"
#define AOSIILMATCHTYPE_VALUE				"value"
#define AOSIILMATCHTYPE_PREFIX				"prefix"
#define AOSIILMATCHTYPE_MAPPER				"mapper"
#define AOSIILMATCHTYPE_SUBSTRING			"substr"

class AosIILMatchType
{
public:
	enum E
	{
		eInvalid,

		eKey,		// Match the current value with the key column
		eValue,		// Match the current value with the value column
		ePrefix,	// Match a field in the key column. 
		eSubstr,	// Match a substring.
		eMapper,

		eMax
	};

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E toEnum(const OmnString &name)
	{
		if (name == AOSIILMATCHTYPE_KEY) return eKey;
		if (name == AOSIILMATCHTYPE_VALUE) return eValue;
		if (name == AOSIILMATCHTYPE_PREFIX) return ePrefix;
		if (name == AOSIILMATCHTYPE_MAPPER) return eMapper;
		if (name == AOSIILMATCHTYPE_SUBSTRING) return eSubstr;
		return eInvalid;
	}

	static OmnString toStr(const E code)
	{
		switch (code)
		{
		case eKey: return AOSIILMATCHTYPE_KEY;
		case eValue: return AOSIILMATCHTYPE_VALUE;
		case ePrefix: return AOSIILMATCHTYPE_PREFIX;
		case eMapper: return AOSIILMATCHTYPE_MAPPER;
		case eSubstr: return AOSIILMATCHTYPE_SUBSTRING;
		default:
		     break;
		}
		return AOSIILMATCHTYPE_INVALID;
	}
};
#endif
