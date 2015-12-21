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
#include "JimoRaft/RaftMsgAppendEntryRsp.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


AosRaftMsgAppendEntryRsp::AosRaftMsgAppendEntryRsp(
		AosRaftServer *server,
		bool result)
:
AosRaftMsg(eMsgAppendEntryRsp, server AosMemoryCheckerArgs),
mResult(result)
{
}

AosRaftMsgAppendEntryRsp::AosRaftMsgAppendEntryRsp()
:
AosRaftMsg(AosMemoryCheckerArgsBegin)
{
}

AosRaftMsgAppendEntryRsp::~AosRaftMsgAppendEntryRsp()
{
}

bool
AosRaftMsgAppendEntryRsp::serializeTo(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);
	aos_assert_r(AosRaftMsg::serializeTo(rdata, buff), false);

	buff->setBool(mResult);

	return true;
}

bool
AosRaftMsgAppendEntryRsp::serializeFrom(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);
	aos_assert_r(AosRaftMsg::serializeFrom(rdata, buff), false);

	mResult = buff->getBool(true);
	return true;
}

