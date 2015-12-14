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
#include "SemanticAsserts/SA0001.h"


AosSA0001::AosSA0001(void *pointer, 
			   		 void *pointedTo,
			   		 const AosMemAllocType allocType, 
					 AosConditionList &conditions,
					 AosEventList &events)
:
AosSemanticAssert(conditions, events),
mPointer(pointer),
mPointedTo(pointedTo),
mMemAllocType(allocType)
{
}


AosSA0001::~AosSA0001()
{
}


