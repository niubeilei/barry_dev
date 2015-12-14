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
// Modification History:
// 2013/12/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Valueset/Valueset.h"



AosValueset::AosValueset(
		const OmnString &type, 
		const OmnString &version)
:
AosJimo(AOS_JIMOTYPE_VALUESET, version),
mValuesetType(type)
{
}
	

AosValueset::~AosValueset()
{
}

