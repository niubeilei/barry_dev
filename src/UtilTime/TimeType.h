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
#ifndef Omn_UtilTime_TimeType_h
#define Omn_UtilTime_TimeType_h

#define AOSTIMETYPE_RECENT				"recent"
#define AOSTIMETYPE_TimeRange			"timerange"
#define AOSTIMETYPE_SpecificTime		"specifictm"

class AosTimeType
{
public:
	enum E
	{
		eInvalid,

		eRecent,
		eTimeRange,
		eSpecificTime,

		eMax
	};

	inline static E toEnum(const OmnString &name)
	{
		if (name == AOSTIMETYPE_RECENT) return eRecent;
		if (name == AOSTIMETYPE_TimeRange) return eTimeRange;
		if (name == AOSTIMETYPE_SpecificTime) return eSpecificTime;
		return eInvalid;
	}
};
#endif

