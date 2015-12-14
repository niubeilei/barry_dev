////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Combiner/Combiner.h"

#include "Combiner/CombinerSum.h"
#include "Combiner/CombinerAverage.h"
#include "Debug/ExitHandler.h"
#include "Debug/Debug.h"

AosCombiner::AosCombiner(
		const OmnString &name, 
		const AosCombinerType::E type,
		const bool regflag)
:
AosCombinerObj(name, type, regflag)
{
}


AosCombiner::~AosCombiner()
{
}


bool
AosCombiner::init()
{
	static AosCombinerSum				lsCombinerSum(true);
	static AosCombinerAverage			lsCombinerAverage(true);

	if (!AosCombinerType::check())
	{
		OmnExitApp("Some combiners were not initialized");
		return false;
	}

	OmnScreen << "All combiners are initialized" << endl;
	return true;
}
