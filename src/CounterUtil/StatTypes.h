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
//   
//
// Modification History:
// 03/26/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterServer_StatTypes_h
#define Aos_CounterServer_StatTypes_h

#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>


#define AOSSTATTYPE_SUM				"sum"
#define AOSSTATTYPE_MAX				"max"
#define AOSSTATTYPE_MIN				"min"
#define AOSSTATTYPE_AVERAGE			"avg"
#define AOSSTATTYPE_MEAN			"mean"
#define AOSSTATTYPE_STDERROR		"stderr"
#define AOSSTATTYPE_COUNT			"count"
#define AOSSTATTYPE_VALUE			"value"
#define AOSSTATTYPE_VALUECOUNT		"valuect"

class AosStatType
{
public:
	enum E
	{
		eInvalid,

		eOrigStatStart = 4,
		eSum = 5,
		eCount,
		eValue,
		eValueCount,
		eTimeSum,
		eOrigStatEnd,

		eAverage,
		eMin,
		eMax,
		eMean,
		eStandardError,

		eMaxEntry
	};

	static char smMap[eMaxEntry];
	static bool smInited;

	static E toEnum(const OmnString &name)
	{
		if (name == AOSSTATTYPE_SUM) 		return eSum;
		if (name == AOSSTATTYPE_MAX) 		return eMax;
		if (name == AOSSTATTYPE_MIN) 		return eMin;
		if (name == AOSSTATTYPE_AVERAGE) 	return eAverage;
		if (name == AOSSTATTYPE_MEAN) 		return eMean;
		if (name == AOSSTATTYPE_STDERROR) 	return eStandardError;
		if (name == AOSSTATTYPE_COUNT) 		return eCount;
		if (name == AOSSTATTYPE_VALUE) 		return eValue;
		if (name == AOSSTATTYPE_VALUECOUNT)	return eValueCount;
		return eInvalid;
	}

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax && 
				code != eOrigStatStart && code != eOrigStatEnd;
	}

	inline static bool isDerivedStat(const E s)
	{
		return s < eOrigStatStart || s > eOrigStatEnd;
	}

	inline static bool isOrigStat(const E s)
	{
		return s > eOrigStatStart && s < eOrigStatEnd;
	}

	inline static char toChar(const E code)
	{
		if (!smInited) init();
		if (code <= eInvalid || code >= eMaxEntry) return smMap[eInvalid];
		return smMap[code];
	}
	
	inline static OmnString toString(const E code)
	{
		if (!smInited) init();
		if (code <= eInvalid || code >= eMaxEntry) return "";
		if (code == eSum)			return AOSSTATTYPE_SUM;
		if (code == eMax)			return AOSSTATTYPE_MAX;
		if (code == eMin)			return AOSSTATTYPE_MIN;
		if (code == eAverage)		return AOSSTATTYPE_AVERAGE;
		if (code == eMean)			return AOSSTATTYPE_MEAN;
		if (code == eStandardError)	return AOSSTATTYPE_STDERROR;
		if (code == eCount)			return AOSSTATTYPE_COUNT;
		if (code == eValue)			return AOSSTATTYPE_VALUE;
		if (code == eValueCount)	return AOSSTATTYPE_VALUECOUNT;
		return "";
	}

	static bool init();

	static bool retrieveStatTypes(vector<E> &stattypes, const AosXmlTagPtr &def);
};

#endif
