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
// 03/31/2012 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/TimeGran.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

bool AosTimeGran::smInited = false;
char AosTimeGran::smCharCode[AosTimeGran::eMax];

void
AosTimeGran::init()
{
	smCharCode[eInvalid] 	= 'a';
	smCharCode[eYearly]		= 'b';
	smCharCode[eMonthly]	= 'c';
	smCharCode[eWeekly]		= 'd';
	smCharCode[eDaily]		= 'e';
	smCharCode[eHourly]		= 'f';
	smCharCode[eMinutely]	= 'g';
	smCharCode[eSecondly]	= 'h';
	smCharCode[eNoTime]		= 'i';
	smCharCode[eMax]		= 'j';
	smInited = true;
}


AosTimeGran::E
AosTimeGran::toEnum(const OmnString &name)
{
	if (!smInited) init();
	int len = name.length();
	if (len <= 0) return eInvalid;
	const char *data = name.data();
	switch (data[0])
	{
	case 'd':
		 if (len == 3 && data[1] == 'a' && data[2] == 'y') return eDaily;
		 break;

	case 'h':
		 if (len == 3 && data[1] == 'u' && data[2] == 'r') return eHourly;
		 break;

	case 'm':
		 if (len == 3 && data[1] == 'u' && data[2] == 't') return eMinutely;
		 if (len == 3 && data[1] == 'n' && data[2] == 't') return eMonthly;
		 break;

	case 'n':
		 if (len == 3 && data[1] == 't' && data[2] == 'm') return eNoTime;
		 break;

	case 's':
		 if (len == 3 && data[1] == 'e' && data[2] == 'c') return eSecondly;
		 break;

	case 'w':
		 if (len == 3 && data[1] == 'e' && data[2] == 'k') return eWeekly;
		 break;

	case 'y':
		 if (len == 3 && data[1] == 'e' && data[2] == 'r') return eYearly;
		 break;

	default:
		 break;
	}

	return eInvalid;
}

