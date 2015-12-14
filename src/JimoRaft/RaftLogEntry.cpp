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
#include "JimoRaft/RaftLogEntry.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "JimoCall/JimoCall.h"  
#include "JimoAPI/JimoDocFunc.h"

static u32 sgTotal = 0;

AosRaftLogEntry::AosRaftLogEntry(AosMemoryCheckDeclBegin)
:
mLogId(0),
mTermId(0),
mServerRecved(0),
mApplied(false),
mDataBuff(NULL)
{
	sgTotal++;
	//if (sgTotal % 100 == 10)
	//	OmnScreen << "Total log entry: " << sgTotal << endl;

	//if (AosMemoryChecker::getCheckOpen())
	//{
	//	AosMemoryCheckerObjCreated(AosClassName::eAosRaftLogEntry);
	//}
}

AosRaftLogEntry::AosRaftLogEntry(
			u64 logId,
			u32 termId AosMemoryCheckDecl)
:
mLogId(logId),
mTermId(termId),
mServerRecved(0),
mApplied(false),
mDataBuff(NULL)
{
	sgTotal++;
	//if (sgTotal % 100 == 10)
	//	OmnScreen << "Total log entry: " << sgTotal << endl;

	//if (AosMemoryChecker::getCheckOpen())
	//{
    //		AosMemoryCheckerObjCreated(AosClassName::eAosRaftLogEntry);
	//}
}

AosRaftLogEntry::~AosRaftLogEntry()
{
	sgTotal--;
	//if (sgTotal % 100 == 10)
	//	OmnScreen << "Total log entry: " << sgTotal << endl;

	//OmnScreen << "Release log entry by tag: " << mTag << endl;

	//if (AosMemoryChecker::getCheckOpen())
	//{
	//	AosMemoryCheckerObjDeleted(AosClassName::eAosRaftLogEntry);
	//}
}

bool 
AosRaftLogEntry::serializeTo(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);

	buff->setU64(mLogId);
	buff->setU32(mTermId);
	//buff->setU32(mServerRecved);
	//buff->setBool(mApplied);

	//if mStatMachHint has value, just reuse it without
	//getting entry from blobSE
	aos_assert_r(mStatMachHint, false);	

	//add databuff to log entry buff
	buff->setU32(mStatMachHint->dataLen());
	buff->setBuff(mStatMachHint);

	//if mDataBuff has value, just reuse it without
	//getting entry from statMachine
	aos_assert_r(mDataBuff, false);	

	//add databuff to log entry buff
	buff->setU32(mDataBuff->dataLen());
	buff->setBuff(mDataBuff);
	return true; 
}

bool 
AosRaftLogEntry::serializeFrom(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);
	mLogId = buff->getU64(0);
	mTermId = buff->getU32(0);
	//mServerRecved = buff->getU32(0);
	//mApplied = buff->getBool(false);

	//bool rslt;
	//get the RaftStateMachine class name and
	//build an instance
	//OmnString className = buff->getOmnStr("");
	//rslt = loadRaftStateMachine(rdata, className, 1);	
	//aos_assert_r(rslt, false);
	u32 dataLen = buff->getU32(0);
	mStatMachHint = buff->getBuff(dataLen, true AosMemoryCheckerArgs);

	dataLen = buff->getU32(0);
	mDataBuff = buff->getBuff(dataLen, true AosMemoryCheckerArgs);
	return true;
}

///////////////////////////////////////////
//   Data methods
///////////////////////////////////////////
bool
AosRaftLogEntry::apply(
		AosRundata*  rdata,
		AosRaftStateMachine* statMach)
{
	aos_assert_r(rdata, false);
	aos_assert_r(statMach, false);

	//commit the data to blobSE
	//need to use raft machine later on
	bool rslt = statMach->apply(rdata, mLogId, mStatMachHint);
	if (!rslt)
	{
		OmnAlarm << "Failed to apply logid:" << mLogId << enderr;
		return false;
	}

	mApplied = true;
	//OmnScreen << "apply data success" << endl;
	return true;
}

AosBuffPtr &
AosRaftLogEntry::getData(
		AosRundata*  rdata,
		AosRaftStateMachine* statMach)
{
	bool rslt;
	if (!mDataBuff)
	{
		aos_assert_r(statMach, mDataBuff);

		mDataBuff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
		rslt = statMach->getEntry(rdata, mLogId, mTermId, mStatMachHint, mDataBuff);
		OmnScreen << "Get log data from state machine for: " << 
			"logId:" << mLogId << ", termId:" << mTermId << ", rslt:" << rslt << endl;

		if (!rslt) OmnAlarm << "Failed to get data from state machine!" << enderr;
	}

	return mDataBuff;
}

void 
AosRaftLogEntry::setData(AosBuffPtr &buff) 
{ 
	mDataBuff = buff; 
}

