////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DateTime.h
// Description:
//   This class handles database DateTime field.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_DateTime_h
#define Omn_Util1_DateTime_h

#include "Util/BasicTypes.h"
#include "Util/String.h"



class OmnDateTime 
{
public:

	enum ETimeType
	{
		eError = -1,
		eNone = 0,
		eDate = 1,
		eDateTime,
		eTime
	};

	enum EDateFormat
	{
		eYYYYMMDDHHMMSS = 0,  // YYYYMMDDHHMMSS
		eYYYY_MM_DD_HH_MM_SS, // YYYY-MM-DD HH:MM:SS
		eMM_DD_YYYY_HH_MM_SS, // MM/DD/YYYY HH:MM:SS
		eMM_DD_YY_HH_MM_SS,   // MM/DD/YY HH:MM:SS
		eYYYY_MM_DD,          // YYYY-MM-DD
		eYYYYMMDD,			  // YYYYMMDD
		eMM_DD_YYYY,		  // MM/DD/YYYY
		eMM_DD_YY,			  // MM/DD/YY
		eHHMMSS,			  // HHMMSS
		eHH_MM_SS			  // HH:MM:SS
	};

	uint			year;		
	uint			month;
	uint			day;
	uint			hour;
	uint			minute;
	uint			second;
	unsigned long	secondPart;
	uint			timeType;

	OmnDateTime() {reset();}
	OmnDateTime(char * dateStr, EDateFormat format = eYYYYMMDDHHMMSS);
	~OmnDateTime();

	void reset() 
	{
		year = 0;
		month = 0;
		day = 0;
		hour = 0;
		minute = 0;
		second = 0;
		secondPart = 0;
		timeType = eNone;
	}
	void set(char * dateStr, EDateFormat format = eYYYYMMDDHHMMSS);

	OmnString toString(EDateFormat format = eYYYYMMDDHHMMSS) const;
	bool operator == (const OmnDateTime &rhs) const
	{
		return year == rhs.year &&
               month == rhs.month &&
			   day == rhs.day &&
               hour == rhs.hour &&
			   minute == rhs.minute &&
               second == rhs.second &&
               secondPart == rhs.secondPart &&
			   timeType == rhs.timeType;
    }

	void operator = (const OmnDateTime &rhs)
	{
		year = rhs.year;
        month = rhs.month;
		day = rhs.day;
        hour = rhs.hour;
		minute = rhs.minute;
        second = rhs.second;
        secondPart = rhs.secondPart;
		timeType = rhs.timeType;
	}
};

#endif
