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
// 10/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataTypes/DataColOpr.h"

#include "Alarm/Alarm.h"


AosDataColOpr::E 
AosDataColOpr::toEnum(const OmnString &name)
{
	if (name == AOSDATACOLOPR_INDEX) 		return eIndex;
	if (name == AOSDATACOLOPR_NORMAL)		return eNormal;
	if (name == AOSDATACOLOPR_NOUPDATE) 	return eNoUpdate;
	if (name == AOSDATACOLOPR_SET_ON_DAY)	return eSetOnEpochDay;
	if (name == AOSDATACOLOPR_MAX_ON_DAY) 	return eMaxOnEpochDay;
	if (name == AOSDATACOLOPR_MIN_ON_DAY)	return eMinOnEpochDay;
	if (name == AOSDATACOLOPR_AVERAGE)		return eAverage;
	if (name == AOSDATACOLOPR_SUM_VALUE)	return eNormal;
	if (name == AOSDATACOLOPR_MAX_VALUE)	return eMaxValue;
	if (name == AOSDATACOLOPR_MIN_VALUE)	return eMinValue;
	if (name == AOSDATACOLOPR_COUNT_VALUE)	return eCountValue;
	if (name == AOSDATACOLOPR_SET_VALUE)	return eSetValue;
	if (name == AOSDATACOLOPR_INVALID)		return eInvalid;
	OmnAlarm << "error name:" << name << enderr;
	return eIndex;
}

OmnString 
AosDataColOpr::toStr(const E code)
{
	switch (code)
	{
	case eIndex: 			return AOSDATACOLOPR_INDEX;
	case eNormal:			return AOSDATACOLOPR_NORMAL;
	case eNoUpdate:         return AOSDATACOLOPR_NOUPDATE;
	case eSetOnEpochDay:	return AOSDATACOLOPR_SET_ON_DAY;
	case eMaxOnEpochDay:	return AOSDATACOLOPR_MAX_ON_DAY;
	case eMinOnEpochDay:	return AOSDATACOLOPR_MIN_ON_DAY;
	case eAverage:			return AOSDATACOLOPR_AVERAGE;
	case eMaxValue:			return AOSDATACOLOPR_MAX_VALUE;
	case eMinValue:			return AOSDATACOLOPR_MIN_VALUE;
	case eSetValue:			return AOSDATACOLOPR_SET_VALUE;
	case eInvalid:			return AOSDATACOLOPR_INVALID;
	default: 				break;
	}
	OmnAlarm << "error code:" << code << enderr;
	return AOSDATACOLOPR_INDEX;
}

