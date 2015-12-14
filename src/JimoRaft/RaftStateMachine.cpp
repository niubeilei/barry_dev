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

// Modification History:
// 2015/04/30 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/RaftStateMachine.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"

static mapptr_t		sgDataPtrMap;
static map_t		sgDataMap;

AosRaftStateMachine::AosRaftStateMachine()
{
}

AosRaftStateMachine::~AosRaftStateMachine()
{
}

AosRaftStateMachine*
AosRaftStateMachine::createRaftStateMachineStatic(
		const AosRundataPtr &rdata,
		const OmnString &className,
		const int version)
{
	itr_t itr = sgDataMap.find(className);
	AosRaftStateMachine *data = NULL;
	if (itr != sgDataMap.end())
	{
		data = itr->second;
	}
	else
	{
		AosJimoPtr jimo = AosCreateJimoByClassname(rdata, className, version);
		aos_assert_r(jimo, NULL);

		AosRaftStateMachinePtr dataPtr = dynamic_cast<AosRaftStateMachine *>(jimo.getPtrNoLock());
		aos_assert_r(dataPtr, NULL);

		data = dataPtr.getPtrNoLock();
		sgDataPtrMap[className] = dataPtr;
		sgDataMap[className] = data;
	}

	//return data->clone();
	return data;
}

//
//Implemenet default APIs, just call
//old APIs without using hint
//
AosBuffPtr  
AosRaftStateMachine::appendEntry(
			const AosRundataPtr &rdata,
			const u64 logId,
			const u32 termId,
			const AosBuffPtr &buff)
{
	bool rslt = appendEntry(rdata, termId, logId, buff);
	if (rslt)
	{
		//create a hint buff and return
		int len = RAFT_MAX_STATMACH_HINT_SIZE;
		AosBuffPtr hintBuff = OmnNew AosBuff(len AosMemoryCheckerArgs);
		char data[RAFT_MAX_STATMACH_HINT_SIZE];

		hintBuff->setBuff(data, len);
		return hintBuff;
	}

	return NULL;
}

bool  
AosRaftStateMachine::apply(
		const AosRundataPtr &rdata,
		const u64 logId,
		AosBuffPtr &hint)
{
	return apply(rdata, logId);
}

bool  
AosRaftStateMachine::getEntry(
		const AosRundataPtr &rdata,
		const u64 logId,
		u32	&termId,
		const AosBuffPtr &hint,
		AosBuffPtr &buff)
{
	return getEntry(rdata, logId, termId, buff);
}

bool  
AosRaftStateMachine::removeEntry(
		const AosRundataPtr &rdata,
		const u64 logId,
		AosBuffPtr &hint)
{
	return removeEntry(rdata, logId);
}
