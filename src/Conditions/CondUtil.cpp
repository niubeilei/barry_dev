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
// Friday, December 21, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondUtil.h"

static const char* sgConditionTypeNames[AosConditionType::eLastEntry] =
{
	"Unknown",
	"CondInField",
	"CondNotInField",
	"CondEqual",
	"CondNotEqual",
	"CondGreater",
	"CondGreaterEqual",
	"CondLess",
	"CondLessEqual"
};


std::string 
AosConditionType::enum2Str(const AosConditionType::E type)
{
	if (type < 0 || type >= AosConditionType::eLastEntry)
	{
		return "Unknown";
	}

	return sgConditionTypeNames[type];
}


AosConditionType::E 
AosConditionType::str2Enum(const std::string& name)
{
	int i=0;
	for (i=0; i<AosConditionType::eLastEntry; i++)
	{
		if (name == std::string(sgConditionTypeNames[i]))
		{
			return (AosConditionType::E)i;
		}
	}
	return AosConditionType::eUnknown;
}


static const char* sgCondDataTypeNames[AosCondDataType::eLastEntry] =
{
	"Unknown",
	"Constant",
	"Variable",
	"ParmValue",
	"TableCell"
};


std::string 
AosCondDataType::enum2Str(const AosCondDataType::E type)
{
	if (type < 0 || type >= AosCondDataType::eLastEntry)
	{
		return "Unknown";
	}

	return sgCondDataTypeNames[type];
}


AosCondDataType::E 
AosCondDataType::str2Enum(const std::string& name)
{
	int i=0;
	for (i=0; i<AosCondDataType::eLastEntry; i++)
	{
		if (name == std::string(sgCondDataTypeNames[i]))
		{
			return (AosCondDataType::E)i;
		}
	}
	return AosCondDataType::eUnknown;
}
