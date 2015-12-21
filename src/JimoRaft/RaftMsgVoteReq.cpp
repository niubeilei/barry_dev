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
#include "JimoRaft/RaftMsgVoteReq.h"

#include "JimoRaft/RaftServer.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

AosRaftMsgVoteReq::AosRaftMsgVoteReq(AosRaftServer *server)
:
AosRaftMsg(eMsgVoteReq, server AosMemoryCheckerArgs)
{
	aos_assert(server);
	mLastLogTerm = server->getLastLogTerm(server->getRundata());
}

//all the members will be set later on
AosRaftMsgVoteReq::AosRaftMsgVoteReq()
:
AosRaftMsg(AosMemoryCheckerArgsBegin)
{
}

AosRaftMsgVoteReq::~AosRaftMsgVoteReq()
{
}

bool
AosRaftMsgVoteReq::serializeTo(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);
	aos_assert_r(AosRaftMsg::serializeTo(rdata, buff), false);

	buff->setU32(mLastLogTerm);
	return true;
}

bool
AosRaftMsgVoteReq::serializeFrom(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);
	aos_assert_r(AosRaftMsg::serializeFrom(rdata, buff), false);

	mLastLogTerm = buff->getU32(0);
	return true;
}

