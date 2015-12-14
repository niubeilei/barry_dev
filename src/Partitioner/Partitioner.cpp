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
#include "Partitioner/Partitioner.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/ExitHandler.h"
#include "Partitioner/PartitionDocid.h"


AosPartitioner::AosPartitioner(
		const OmnString &name, 
		const AosPartitionerType::E type,
		const bool regflag)
:
AosPartitionerObj(name, type, regflag)
{
}


AosPartitioner::~AosPartitioner()
{
}


bool
AosPartitioner::init()
{
	static AosPartitionDocid		lsPartitionDocid(true);

	if (!AosPartitionerType::check())
	{
		OmnAlarm << "Some partitioners not registered" << enderr;
		OmnExitApp("Some partitioners not registered");
		return false;
	}

	OmnScreen << "All partitioners are initialized" << endl;
	return true;
}

