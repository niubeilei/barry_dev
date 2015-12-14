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
// 2014/08/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Patterns/Pattern.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"


AosPattern::AosPattern()
:
AosPatternObj(1)
{
}


AosPattern::AosPattern(const int version)
:
AosPatternObj(version)
{
}


AosPattern::~AosPattern()
{
}


