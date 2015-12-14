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
// The super class for all semantics data. 
//
// Modification History:
// 01/29/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SemanticData/Var.h"

#include "RVG/CommonTypes.h"


AosVar::AosVar(const std::string &name)
:
mName(name)
{
}


AosVar::~AosVar()
{
}


AosValuePtr		
AosVar::getValue() const
{
	return 0;
}


