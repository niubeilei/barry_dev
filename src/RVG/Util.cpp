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
#include "RVG/Util.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

static const char* sgSequenceOrderNames[AosSequenceOrder::eLastEntry] =
{
	"Random order",
	"ASC order",
	"DEC order",
};


OmnString 
AosSequenceOrder::enum2Str(const AosSequenceOrder::E type)
{
	if (type < 0 || type >= AosSequenceOrder::eLastEntry)
	{
		return "Random";
	}

	return sgSequenceOrderNames[type];
}


AosSequenceOrder::E 
AosSequenceOrder::str2Enum(const OmnString &name)
{
	int i=0;
	for (i=0; i<AosSequenceOrder::eLastEntry; i++)
	{
		if (name == OmnString(sgSequenceOrderNames[i]))
		{
			return (AosSequenceOrder::E)i;
		}
	}
	return AosSequenceOrder::eSeqOrder_Random;
}


static const char* sgValueTypeNames[AosValueType::eLastEntry] =
{
	"unknown",		// unknow type
	"bool",			// data type string: bool
	"u8",			// data type string: unsigned char/u8/uint8_t
	"u16",			// data type string: unsigned short/u16/uint16_t
	"u32",			// data type string: unsigned int/u32/uint32_t
	"u64",			// data type string: u64/uint64_t
	"int8_t",		// data type string: char/int8_t
	"int16_t",		// data type string: short/int16_t
	"int32_t",		// data type string: int/int32_t
	"int64_t",		// data type string: int64_t
	"float",		// data type string: float
	"double",		// data type string: double
	"string",		// data type string: char*/OmnString 
	"byteStream",	// data type string: unsigned char*
	"Userdefined",	// user-defined struct name
	"table",
	"record"
	
};


OmnString 
AosValueType::enum2Str(const AosValueType::E type)
{
	if (type < 0 || type >= AosValueType::eLastEntry)
	{
		return "Unknow";
	}

	return sgValueTypeNames[type];
}


AosValueType::E 
AosValueType::str2Enum(const OmnString &name)
{
	int i=0;
	for (i=0; i<AosValueType::eLastEntry; i++)
	{
		if (name == OmnString(sgValueTypeNames[i]))
		{
			return (AosValueType::E)i;
		}
	}
	return AosValueType::eUnknown;
}


static const char* sgRVGTypeNames[AosRVGType::eLastEntry] =
{
	"unknown",
	"CharsetRSG",
	"EnumRSG",
	"EnumSeqRSG",
	"U8RIG",
	"U16RIG",
	"U32RIG",
	"U64RIG",
	"Int8RIG",
	"Int16RIG",
	"Int32RIG",
	"Int64RIG",
	"Float",
	"Double",
	"AndRSG",
	"OrRSG"
};


OmnString 
AosRVGType::enum2Str(const AosRVGType::E type)
{
	if (type < 0 || type >= AosRVGType::eLastEntry)
	{
		return "Unknow";
	}

	return sgRVGTypeNames[type];
}


AosRVGType::E 
AosRVGType::str2Enum(const OmnString &name)
{
	int i=0;
	for (i=0; i<AosRVGType::eLastEntry; i++)
	{
		if (name == OmnString(sgRVGTypeNames[i]))
		{
			return (AosRVGType::E)i;
		}
	}
	OmnAlarm << "RVG Key not recognized: " << name << enderr;
	return AosRVGType::eUnknown;
}


static const char* sgCorCtrlNames[AosCorCtrl::eLastEntry] = 
{
	"Random",
	"Correct",
	"Incorrect"
};


AosCorCtrl::E
AosCorCtrl::str2Enum(const OmnString &name)
{
	for (int i=0; i<AosCorCtrl::eLastEntry; i++)
	{
		OmnString tt = sgCorCtrlNames[i];
		if (tt == name) 
			return (AosCorCtrl::E)i;
	}

	OmnAlarm << "corCtrl name not recognized: " << name << enderr;
	return AosCorCtrl::eRandom;
}


OmnString 
AosCorCtrl::enum2Str(const AosCorCtrl::E type)
{
	if (type < 0 || type >= AosCorCtrl::eLastEntry)
	{
		OmnAlarm << "Unrecognized CorCtrl type: " << type << enderr;
		return "Random";
	}

	return sgCorCtrlNames[type];
}


static const char* sgRVGUsageTypeNames[AosRVGUsageType::eLastEntry] = 
{
	"Unknown", 
	"InputOnly",
	"InputOutputThrRef",
	"InputOutputThrAddr",
	"OutputThrRef", 
	"OutputThrAddr"
};


AosRVGUsageType::E
AosRVGUsageType::str2Enum(const OmnString &name)
{
	for (int i=0; i<AosRVGUsageType::eLastEntry; i++)
	{
		OmnString tt = sgRVGUsageTypeNames[i];
		if (tt == name) 
			return (AosRVGUsageType::E)i;
	}

	OmnAlarm << "RVG Usage type name not recognized: " << name << enderr;
	return AosRVGUsageType::eUnknown;
}


OmnString 
AosRVGUsageType::enum2Str(const AosRVGUsageType::E type)
{
	if (type < 0 || type >= AosRVGUsageType::eLastEntry)
	{
		OmnAlarm << "Unrecognized RVG Usage type: " << type << enderr;
		return "Unknown";
	}

	return sgRVGUsageTypeNames[type];
}


