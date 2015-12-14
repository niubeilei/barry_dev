////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DateTime.cpp
// Description:
//   This class handles the database DateTime field.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util1/DateTime.h"


OmnDateTime::OmnDateTime(char * dateStr, EDateFormat format)
{
	set(dateStr,format);
}


OmnDateTime::~OmnDateTime()
{
}

void
OmnDateTime::set(char * dateStr, EDateFormat format)
{
	if (format == eYYYYMMDDHHMMSS) 
	{
		sscanf (dateStr,"%4d%2d%2d%2d%2d%2d",&year,&month,&day,&hour,&minute,&second);
		timeType = eDateTime;
		secondPart = 0;
	}
	else if (format == eYYYY_MM_DD_HH_MM_SS)
	{
		sscanf (dateStr,"%4d-%2d-%2d %2d:%2d:%2d",&year,&month,&day,&hour,&minute,&second);
		timeType = eDateTime;
		secondPart = 0;
	}

	//
	// check whether it is the empty time
	//
	if (year==0 && month==0 && day==0 && hour==0 && minute==0 && second==0)
	{
		timeType = eNone;
	}
}


OmnString
OmnDateTime::toString(EDateFormat format) const
{
	char buffer[100];
	if (format == eYYYYMMDDHHMMSS) 
	{
		sprintf (buffer,"%4d%2d%2d%2d%2d%2d",year,month,day,hour,minute,second);
	}
	else if (format == eYYYY_MM_DD_HH_MM_SS)
	{
		sprintf (buffer,"%4d-%2d-%2d %2d:%2d:%2d",year,month,day,hour,minute,second);
	}
	return OmnString(buffer);
}

