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
#include "Sorter/Sorter.h"

#include "Sorter/SorterRecord.h"
#include "Debug/ExitHandler.h"
#include "Debug/Debug.h"

AosSorter::AosSorter(
		const OmnString &name, 
		const AosSorterType::E type,
		const bool regflag)
:
AosSorterObj(name, type, regflag)
{
}


AosSorter::~AosSorter()
{
}


bool
AosSorter::init()
{
	static AosSorterRecord				lsSorterRecord(true);

	if (!AosSorterType::check())
	{
		OmnExitApp("Some sorters were not initialized");
		return false;
	}

	OmnScreen << "All sorters are initialized" << endl;
	return true;
}
