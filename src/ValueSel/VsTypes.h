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
// 09/28/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_ValueSel_VsTypes_h
#define Aos_ValueSel_VsTypes_h

#include "Util/String.h" 

#define AOSACTOPRID_ATTR				"attr"
#define AOSACTOPRID_CONST				"const"
#define AOSACTOPRID_OPR		  	 		"opr"
#define AOSACTOPRID_DAY					"day"
#define AOSACTOPRID_ENUM				"enum"
#define AOSACTOPRID_EPOCH				"epoch"
#define AOSACTOPRID_FULL				"full"
#define AOSACTOPRID_GETTIME				"gettime"
#define AOSACTOPRID_HOUR				"hour"
#define AOSACTOPRID_MINUTE				"minute"
#define AOSACTOPRID_MONTH				"month"
#define AOSACTOPRID_NUM2STRMAP			"num2str"
#define AOSACTOPRID_PATTERN				"pattern"	
#define AOSACTOPRID_RANGE				"range"	
#define AOSACTOPRID_MULTIRANGES			"mulranges"	
#define AOSACTOPRID_RAND				"rand"	
#define AOSACTOPRID_REQUESTER			"requester"
#define AOSACTOPRID_SEQNO				"seqno"
#define AOSACTOPRID_SECOND				"second"
#define AOSACTOPRID_TEXT				"text"
#define AOSACTOPRID_YEAR				"year"
#define AOSACTOPRID_NUMENTRIES			"numentries"
#define AOSACTOPRID_MDDIFYRDATA			"modifyrdata"
#define AOSACTOPRID_QUERY				"query"
#define AOSACTOPRID_RUNDATA				"rundata"
#define AOSACTOPRID_BYUSERARGS			"byuserargs"
#define AOSACTOPRID_COMPOSE				"compose"
#define AOSACTOPRID_ACCESSED			"accessed"
#define AOSACTOPRID_MATH				"math"
#define AOSACTOPRID_BYCOND				"bycond"
#define AOSACTOPRID_SUBSTR				"substr"

struct AosValueSelType
{
	enum E
	{
		eInvalid, 

		eByUserArgs,
		eConst,
		eEnum,
		eMonth,
		eDay,
		eHour,
		eMinute,
		eSecond,
		eAttr,
		eEpoch,
		eFull,
		eMultiRanges,
		eNumEntries,
		eNum2StrMap,
		ePattern,
		eQuery,
		eRandom,
		eRundataAttr,
		eRange,
		eRundata,
		eSeqno,
		eText,
		eYear,
		eValueSel,
		eRequester,
		eCompose,
		eAccessed,
		eMath,
		eByCond,
		eSubstr,

		eMax
	};

	static E toEnum(const OmnString &name);
	static OmnString  toString(const E type);
	static bool isValid(const E code) 
	{
		return code > eInvalid && code < eMax;
	}
	static bool addName(const OmnString &name, const E value, OmnString &errmsg);
};

#endif
#endif

