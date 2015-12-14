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
// The super class for all Atomic type semantics data. 
//
// Modification History:
// 11/27/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticData/VarAtomic.h"

#include "aosUtil/ReturnCode.h"
#include "Debug/Except.h"
#include "Debug/ErrId.h"
#include "RVG/CommonTypes.h"


AosVarAtomic::AosVarAtomic(const std::string &name)
:
AosVar(name)
{
}


AosVarAtomic::~AosVarAtomic()
{
}


AosValuePtr
AosVarAtomic::getValue() const
{
	return mValue;
}

