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
// Generic Table serves as a database table. It contains a list
// of records, each of which is expressed by instances  of AosGenRecord. 
// Each record is identified through names. 
//
// Modification History:
// 05/13/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Util_GenTableUtil_h
#define Util_GenTableUtil_h

#include "Util/String.h"

enum AosGenFieldType
{
	eAosGenFieldType_Invalid,
	eAosGenFieldType_Int, 
	eAosGenFieldType_U32,
	eAosGenFieldType_Str,
	eAosGenFieldType_Table
};

extern OmnString AosGenFieldType_toStr(const AosGenFieldType type);

#endif

