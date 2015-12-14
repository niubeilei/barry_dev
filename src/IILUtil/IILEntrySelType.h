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
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILUtil_IILEntrySelType_h
#define Aos_IILUtil_IILEntrySelType_h

#include "Util/String.h"


#define AOSENTRYSELTYPE_ALL					"all"
#define AOSENTRYSELTYPE_AVERAGE				"average"
#define AOSENTRYSELTYPE_DIFF				"diff"
#define AOSENTRYSELTYPE_DISTINCT			"distinct"
#define AOSENTRYSELTYPE_FIRST				"first"
#define AOSENTRYSELTYPE_LAST				"last"
#define AOSENTRYSELTYPE_MIN					"min"
#define AOSENTRYSELTYPE_MAX					"max"
#define AOSENTRYSELTYPE_MEAN				"mean"
#define AOSENTRYSELTYPE_SUM					"sum"
#define AOSENTRYSELTYPE_STANDARD_ERROR		"stderr"
#define AOSENTRYSELTYPE_RANDOM				"random"
#define AOSENTRYSELTYPE_RANGE				"range"

class AosIILEntrySelType
{
public:
	enum E
	{
		eInvalid,

		eFirst, 				// Select first; 
		eLast,					// Select the last;
		eAverage,				// Select the average;
		eMax,					// Select the max
		eMin,					// Select the min
		eMean,					// Select the mean
		eSum,					// Select the sum
		eStandardError,			// Select the differences
		eRandom,				// Select randomly
		eDistinct,				// Select distinct values only
		eDiff,					// = eMax - eMin + 1
		eAll,					// Select all
		eRange,					// The range is specified separately

		eMaxNumEntries
	};

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMaxNumEntries;
	}

	static E toEnum(const OmnString &name)
	{
		if (name.length() < 1) return eInvalid;
		switch (name.data()[0])
		{
		case 'a':
			 if (name == AOSENTRYSELTYPE_ALL) return eAll;
			 if (name == AOSENTRYSELTYPE_AVERAGE) return eAverage;
			 break;

		case 'd':
			 if (name == AOSENTRYSELTYPE_DIFF) return eDiff;
			 if (name == AOSENTRYSELTYPE_DISTINCT) return eDistinct;
			 break;

		case 'f':
			 if (name == AOSENTRYSELTYPE_FIRST) return eFirst;
			 break;

		case 'l':
			 if (name == AOSENTRYSELTYPE_LAST) return eLast;
			 break;

		case 'm':
			 if (name == AOSENTRYSELTYPE_MIN) return eMin;
			 if (name == AOSENTRYSELTYPE_MAX) return eMax;
			 if (name == AOSENTRYSELTYPE_MEAN) return eMean;
			 break;

		case 's':
			 if (name == AOSENTRYSELTYPE_SUM) return eSum;
			 if (name == AOSENTRYSELTYPE_STANDARD_ERROR) return eStandardError;
			 break;

		case 'r':
			 if (name == AOSENTRYSELTYPE_RANDOM) return eRandom;
			 if (name == AOSENTRYSELTYPE_RANGE) return eRange;
			 break;

		default:
			 return eInvalid;
		}
		return eInvalid;
	}
};


#endif
