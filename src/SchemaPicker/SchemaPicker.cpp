////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SchemaPicker/SchemaPicker.h"

#include "API/AosApi.h"
AosSchemaPicker::AosSchemaPicker(
		const OmnString &type,
		const u32 version)
:
AosSchemaPickerObj(version),
mJimoSubType(type)
{
}


AosSchemaPicker::~AosSchemaPicker()
{
}


