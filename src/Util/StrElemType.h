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
// 05/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_StrElemType_h
#define Aos_Util_StrElemType_h

#include "Rundata/Ptrs.h"
#include "Util/String.h"


#define AOSSTRELEMTYPE_CONST		"const"
#define AOSSTRELEMTYPE_ELEM			"elem"
#define AOSSTRELEMTYPE_KEY			"key"
#define AOSSTRELEMTYPE_DOCID		"docid"
#define AOSSTRELEMTYPE_SUBSTR		"substr"
#define AOSSTRELEMTYPE_DOCID_HIGH	"docidhg"
#define AOSSTRELEMTYPE_DOCID_LOW	"docidlw"
#define AOSSTRELEMTYPE_INVALID		"invalid"

class AosStrElemType
{
public:
	enum E
	{
		eInvalid,

		eElem,
		eConst,
		eDocid,
		eKey,
		eSubstr,
		eDocidHigh,
		eDocidLow,

		eMax
	};

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E toEnum(const OmnString &name)
	{
		if (name == AOSSTRELEMTYPE_CONST) 		return eConst;
		if (name == AOSSTRELEMTYPE_ELEM) 		return eElem;
		if (name == AOSSTRELEMTYPE_DOCID) 		return eDocid;
		if (name == AOSSTRELEMTYPE_KEY) 		return eKey;
		if (name == AOSSTRELEMTYPE_SUBSTR) 		return eSubstr;
		if (name == AOSSTRELEMTYPE_DOCID_HIGH) 	return eDocidHigh;
		if (name == AOSSTRELEMTYPE_DOCID_LOW) 	return eDocidLow;
		return eInvalid;
	}

	static OmnString toString(const E code)
	{
		switch (code)
		{
		case eConst:		return AOSSTRELEMTYPE_CONST;
		case eElem:			return AOSSTRELEMTYPE_ELEM;
		case eDocid:		return AOSSTRELEMTYPE_DOCID;
		case eKey:			return AOSSTRELEMTYPE_KEY;
		case eSubstr:		return AOSSTRELEMTYPE_SUBSTR;
		case eDocidHigh:	return AOSSTRELEMTYPE_DOCID_HIGH;
		case eDocidLow:		return AOSSTRELEMTYPE_DOCID_LOW;
		default:
			 break;
		}
		return AOSSTRELEMTYPE_INVALID;
	}
};
#endif
