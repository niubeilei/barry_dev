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
// Modification History:
// 03/30/2012 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_TimeGran_h
#define Omn_UtilTime_TimeGran_h

#include "Util/String.h"

#define AOSTIMEGRAN_YEARLY			"yer";
#define AOSTIMEGRAN_MONTHLY			"mnt";
#define AOSTIMEGRAN_WEEKLY			"wek";
#define AOSTIMEGRAN_DAILY			"day";
#define AOSTIMEGRAN_HOURLY			"hur";
#define AOSTIMEGRAN_MINUTELY		"mut";
#define AOSTIMEGRAN_SECONDLY		"sec";
#define AOSTIMEGRAN_NOTIME			"ntm";

class AosTimeGran
{
public:
	enum
	{
		eNoTimeFlag = 'N',
		eTimeFlag = 'T'
	};

	enum E
	{
		eInvalid,

		eYearly			= 0x01,
		eMonthly		= 0x02,
		eWeekly			= 0x03,
		eDaily			= 0x04,
		eHourly			= 0x05,
		eMinutely		= 0x06,
		eSecondly		= 0x07,

		eNoTime,
		eMax
	};

private:
	static char		smCharCode[AosTimeGran::eMax];
	static bool		smInited;

public:
	static void init();

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E toEnum(const OmnString &name);

	inline static char toChar(const E code)
	{
		if (!smInited) init();
		if (code <= eInvalid || code >= eMax) return smCharCode[eInvalid];
		return smCharCode[code];
	}
};
#endif

