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
// Created: 08/04/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Book/DataType.h"

#include "Alarm/Alarm.h"


const OmnString sgNames[] = 
{
	"invalid",

	"image",
	"picture",
	"table",
	"record",

	"Invalid"
};


OmnString 
AosSystemDataType::getStr(const AosSystemDataType::E code)
{
	if (code < eInvalid || code > eMax) 
	{
		OmnAlarm << "Invalid data type: " << code << enderr;
		return "Invalid";
	}

	return sgNames[code];
}


AosSystemDataType::E
AosSystemDataType::getCode(const OmnString &name)
{
	for (int i=0; i<eMax; i++)
	{
		if (sgNames[i] == name) return (E)i;
	}
	return eInvalid;
}
