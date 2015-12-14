////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Random.h
// Description:
//  This file contains a group of random functions and a random class. 
//  The OmnRandom class is obselete. You are encouraged to use the 
//  random functions.  
//
// Modification History:
//	05/01/2007 Extracted from Random.h, Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Util/StrType.h"

#include "Alarm/Alarm.h"


static OmnString sgNames[eAosStrType_LastEntry+1] = 
{
	"Invalid", 
	"AsciiStr", 
	"Printable",
	"NoSpaceStr", 
	"BinStr",
	"LowcaseStr", 
	"CapitalStr",
	"DigitStr",
	"LetterStr",
	"LetterDigitStr",
	"VarName",
	"Symbol",
	"WhiteSpace",
	"WhiteSpaceWithNewLines",
	"NonPrintable",
	"ControlChar",
	"HH:MM:SS",
	"InvalidHH:MM:SS",
	"Invalid_1"
};

AosStrType AosStrType_strToEnum(const OmnString &name)
{
	for (int i=0; i<eAosStrType_LastEntry; i++)
	{
		if (name == sgNames[i]) return (AosStrType)i;
	}

	OmnAlarm << "Invalid string type: " << name << enderr;
	return eAosStrType_Invalid;
}


OmnString AosStrType_enumToStr(const AosStrType type)
{
	if (type > eAosStrType_Invalid && type < eAosStrType_LastEntry)
	{
		return sgNames[type];
	}

	return "Invalid";
}


OmnString AosStrType_getStrTypeNames()
{
	OmnString str;
	for (int i=0; i<eAosStrType_LastEntry; i++)
	{
		str << sgNames[i] << "\n";
	}
	return str;
}


