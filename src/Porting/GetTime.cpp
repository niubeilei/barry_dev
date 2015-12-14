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
//	04/25/2010:	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Porting/GetTime.h"

#include "Alarm/Alarm.h"

bool OmnParseTimeStr(
		const AosLocale::E loc,
		const char *timestr,
		char *year,
		char *month,
		char *day)
{
	switch (loc)
	{
	case AosLocale::eChina:
		 //	YYYY-MM-DD HH:MM:SS
		 year[0] = timestr[0];
		 year[1] = timestr[1];
		 year[2] = timestr[2];
		 year[3] = timestr[3];
		 year[4] = 0;

		 month[0] = timestr[5];
		 month[1] = timestr[6];
		 month[2] = 0;

		 day[0] = timestr[8];
		 day[1] = timestr[9];
		 day[2] = 0;
		 break;

	case AosLocale::eUSA:
		 //	MM-DD-YYYY HH:MM:SS
		 month[0] = timestr[0];
		 month[1] = timestr[1];
		 month[2] = 0;

		 day[0] = timestr[3];
		 day[1] = timestr[4];
		 day[2] = 0;

		 year[0] = timestr[6];
		 year[1] = timestr[7];
		 year[2] = timestr[8];
		 year[3] = timestr[9];
		 year[4] = 0;
		 break;

	default:
		 OmnAlarm << "Unrecognized locale: " << loc << enderr;
		 return false;
	}

	// hour[0] = timestr[11];
	// hour[1] = timestr[12];
	// hour[2] = 0;

	// minute[0] = timestr[14];
	// minute[1] = timestr[15];
	// minute[2] = 0;

	// sec[0] = timestr[17];
	// sec[1] = timestr[18];
	// sec[2] = 0;
	return true;
}


