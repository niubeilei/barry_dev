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
//	2011/04/19	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_TimeUtil_h
#define Omn_Util_TimeUtil_h

#include "aosUtil/Types.h"
#include "Util/String.h"

#define AOSTIMEFORMAT_YYYY			"yyyy"
#define AOSTIMEFORMAT_YYYYMM		"yyyymm"

/*
struct AosTimeInfo
{
	int		year;
	int		month;
	int		day;
	int		hour;
	int		minute;
	int		second;
	int		usec;
};

class AosTimeUtil : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Format
	{
		eYYYY,
		eYYYYMM,
		
		eInvalid
	};

public:
	AosTimeUtil();
	~AosTimeUtil();

	static bool 	getTime(const OmnString &timestr, 
						const OmnString &format,
						AosTimeInfo &timeinfo);
	static bool		getTime(AosTimeInfo &timeinfo);

	static Format	toEnum(const OmnString &format);
	static bool 	toU32(const OmnString &timestr, OmnString &value, OmnString &errmsg);
};
*/
#endif




