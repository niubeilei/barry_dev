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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataField. Chen Ding, 07/08/2012
#ifndef Aos_DataFormat_DataElemType_h
#define Aos_DataFormat_DataElemType_h

#include "Util/String.h"

#define AOSDATAELEMTYPE_INVALID				"invalid"
#define AOSDATAELEMTYPE_STR					"str"
#define AOSDATAELEMTYPE_STRCOMPOSE			"str_compose"
#define AOSDATAELEMTYPE_STRRANDOM			"str_rand"
#define AOSDATAELEMTYPE_TIME				"time"
#define AOSDATAELEMTYPE_U64					"u64"

class AosDataElemType 
{
public:
	enum E
	{
		eInvalid,

		eStr,
		eStrCompose,
		eStrRandom,
		eTime,
		eU64,

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
		if (code == AOSDATAELEMTYPE_STR) return eStr;
		if (code == AOSDATAELEMTYPE_STRCOMPOSE) return eStrCompose;
		if (code == AOSDATAELEMTYPE_STRRANDOM) return eStrRandom;
		if (code == AOSDATAELEMTYPE_TIME) return eTime;
		if (code == AOSDATAELEMTYPE_U64) return eU64;
		return eInvalid;
	}
	
	static OmnString toString(const E code)
	{
		switch(code)
		{
		case eStr : 		return AOSDATAELEMTYPE_STR;
		case eStrCompose: 	return AOSDATAELEMTYPE_STRCOMPOSE;
		case eStrRandom : 	return AOSDATAELEMTYPE_STRRANDOM;
		case eTime : 		return AOSDATAELEMTYPE_TIME;	
		case eU64 : 		return AOSDATAELEMTYPE_U64;
		default : return AOSDATAELEMTYPE_INVALID;
		}
		return AOSDATAELEMTYPE_INVALID;
	}
};

#endif
#endif
