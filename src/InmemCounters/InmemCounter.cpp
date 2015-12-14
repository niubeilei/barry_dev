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
// 09/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "InmemCounters/InmemCounter.h"

#include "InmemCounters/InmemCounterCreator.h"

AosInmemCounter::AosInmemCounter(
		const OmnString &name, 
		const AosInmemCounterType::E type, 
		const bool flag)
:
AosInmemCounterObj(name, type, flag)
{
}


AosInmemCounter::~AosInmemCounter()
{
}

