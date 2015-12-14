////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticAsserts/SemanticAssert.h"

#include "Conditions/Condition.h"
#include "Event/Event.h"


AosSemanticAssert::AosSemanticAssert(AosConditionList &conditions, 
						AosEventList &events)
:
mConditions(conditions),
mEvents(events)
{
}


AosSemanticAssert::~AosSemanticAssert()
{
}

