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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataShuffler_DataShufflerType_h
#define Aos_DataShuffler_DataShufflerType_h

#include "Util/String.h"

#define AOSDATASHUFFLER_INVALID					"invalid"
#define AOSDATASHUFFLER_DOCID					"docid"
#define AOSDATASHUFFLER_IIL						"iil"
#define AOSDATASHUFFLER_DISTMAP					"distmap"

class AosDataShufflerType 
{
public:
	enum E
	{
		eInvalid,

		eDocid,
		eIIL,
		eDistMap,

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
		if (code == AOSDATASHUFFLER_DOCID) return eDocid;
		if (code == AOSDATASHUFFLER_DISTMAP) return eDistMap;
		return eInvalid;
	}
	
	static OmnString toString(const E code)
	{
		switch(code)
		{
		case eDocid: 		return AOSDATASHUFFLER_DOCID;
		case eDistMap: 		return AOSDATASHUFFLER_DISTMAP;
		default: break;
		}
		return AOSDATASHUFFLER_INVALID;
	}
	static bool check();
	static bool addName(const OmnString &name, const E code);
};

#endif
