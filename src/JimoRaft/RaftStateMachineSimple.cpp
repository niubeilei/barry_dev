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
#include "JimoRaft/RaftStateMachineSimple.h"
#include "JimoRaft/RaftServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosRaftStateMachineSimple_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRaftStateMachineSimple();
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosRaftStateMachineSimple::AosRaftStateMachineSimple()
{
}

AosRaftStateMachineSimple::~AosRaftStateMachineSimple()
{
}

AosJimoPtr
AosRaftStateMachineSimple::cloneJimo() const
{
    AosJimoPtr thisptr((AosRaftStateMachineSimple*)this, false);
    return thisptr;
}

AosBuffPtr
AosRaftStateMachineSimple::appendEntry(
		const AosRundataPtr &rdata,
		const u64 logId,
		const u32	termId,
		const AosBuffPtr &buff)
{
	AosBuffPtr hint = OmnNew AosBuff(
			RAFT_MAX_STATMACH_HINT_SIZE AosMemoryCheckerArgs);

	hint->reset();
	hint->setU32(0); //not applied
	hint->setU64(logId);
	hint->setU32(termId);
	hint->setU64(buff->dataLen());
	return hint;
}

bool  
AosRaftStateMachineSimple::apply(
		const AosRundataPtr &rdata,
		const u64 logId,
		AosBuffPtr &hint)
{
	u64 hintLogId;
	u64 applied;

	hint->reset();
	applied = hint->getU32(0);
	hintLogId = hint->getU64(0);
	aos_assert_r(hintLogId == logId && !applied, false);

	hint->reset();
	hint->setU32(1);  //applied
	return true;
}

bool  
AosRaftStateMachineSimple::getEntry(
		const AosRundataPtr &rdata,
		const u64 logId,
		u32	&termId,
		const AosBuffPtr &hint,
		AosBuffPtr &buff)
{
	RAFT_OmnScreen << "entering getEntry" << endl;

	u64 hintLogId;
	u32 hintTermId;
	u64 applied;

	hint->reset();
	applied = hint->getU32(0);
	hintLogId = hint->getU64(0);
	hintTermId = hint->getU32(0);

	//The entry must be applied already
	aos_assert_r(hintLogId == logId 
			&& hintTermId == termId && applied, false);
	return true;
}

//
//Remove obsolete data
//
bool  
AosRaftStateMachineSimple::removeEntry(
		const AosRundataPtr &rdata,
		const u64 logId,
		AosBuffPtr &hint)
{
	u64 hintLogId;
	u64 applied;

	hint->reset();
	applied = hint->getU32(0);
	hintLogId = hint->getU64(0);

	//The entry must NOT be applied
	aos_assert_r(hintLogId == logId && !applied, false);
	return true;
}

