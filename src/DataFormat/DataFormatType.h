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
// 2012/03/15 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataFormat_DataFormatType_h
#define Aos_DataFormat_DataFormatType_h

#include "Util/String.h"

enum AosDataFormatType
{
	eAosDataFormatType_Invalid,

	eAosDataFormatType_Xml,
	eAosDataFormatType_Raw,
	eAosDataFormatType_FixLen,

	eAosDataFormatType_Max
};

inline bool AosDataFormatType_isValid(const AosDataFormatType code)
{
	return code > eAosDataFormatType_Invalid && code < eAosDataFormatType_Max;
}

inline AosDataFormatType AosDataFormatType_toEnum(const OmnString &name)
{
	if (name == "xml") return eAosDataFormatType_Xml;
	if (name == "raw") return eAosDataFormatType_Raw;
	if (name == "fixlen") return eAosDataFormatType_FixLen;
	return eAosDataFormatType_Invalid;
}

#endif

