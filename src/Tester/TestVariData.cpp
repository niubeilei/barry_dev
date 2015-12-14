////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestVariData.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestVariData.h"


OmnTestVariData::OmnTestVariData()
:
mType(eTester_DataType_Bool),
mIntValue(0),
mBoolValue(true),
mChanged(true)
{
}


OmnTestVariData::OmnTestVariData(const int &data)
:
mType(eTester_DataType_Int),
mIntValue(data),
mBoolValue(true),
mChanged(true)
{
}


OmnTestVariData::OmnTestVariData(const bool	&data)
:
mType(eTester_DataType_Bool),
mIntValue(0),
mBoolValue(data),
mChanged(true)
{
}

OmnTestVariData::OmnTestVariData(const OmnString	&data)
:
mType(eTester_DataType_String),
mIntValue(0),
mBoolValue(true),
mStrValue(data),
mChanged(true)
{
	
}
