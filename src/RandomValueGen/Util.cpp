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
// 03/14/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Parms/Util.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

static OmnString sgRVGKeyTypeNames[eAosRVGKeyType_LastEntry+1] = 
{
	"unknown",
	"exist",
	"new", 
	"nokey", 
	"invalid"
};


AosRVGKeyType
AosGetRVGKeyType(const OmnString &str)
{
	OmnString name = str;
	name.toLower();
	for (int i=0; i<eAosRVGKeyType_LastEntry; i++)
	{
		OmnString tt = sgRVGKeyTypeNames[i];
		tt.toLower();
		if (tt == name) return (AosRVGKeyType)i;
	}

	OmnAlarm << "RVG Key not recognized: " << str << enderr;
	return eAosRVGKeyType_Unknown;
}


OmnString
AosGetRVGKeyTypeStr(const AosRVGKeyType type)
{
	if (type < 0 || type >= eAosRVGKeyType_LastEntry)
	{
		OmnAlarm << "Unrecognized key type: " << type << enderr;
		return "Unknown";
	}

	return sgRVGKeyTypeNames[type];
}


static OmnString sgRVGUsageTypeNames[eAosRVGUsageType_LastEntry+1] = 
{
	"Unknown", 
	"InputOnly",
	"InputOutputThrRef",
	"InputOutputThrAddr",
	"OutputThrRef", 
	"OutputThrAddr"
};

OmnString AosGetRVGUsageTypeStr(const AosRVGUsageType type)
{
	if (type < 0 || type >= eAosRVGUsageType_LastEntry)
	{
		OmnAlarm << "Invalid type: " << type << enderr;
		return "Unknown";
	}

	return sgRVGUsageTypeNames[type];
}


AosRVGUsageType
AosGetRVGUsageType(const OmnString &name)
{
	for (int i=0; i<eAosRVGKeyType_LastEntry; i++)
	{
		if (sgRVGUsageTypeNames[i] == name)
		{
			return (AosRVGUsageType)i;
		}
	}

	return eAosRVGUsageType_Unknown;
}

