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
#include "JimoRaft/RaftMsgVoteRsp.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"

AosRaftMsgVoteRsp::AosRaftMsgVoteRsp(
		AosRaftServer *server,
		bool result,
		u32 votedFor,
		u32 votedForTermId)
:
AosRaftMsg(eMsgVoteRsp, server),
mResult(result),
mVotedFor(votedFor),
mVotedForTermId(votedForTermId)
{
}

//all the member values will be set later on
AosRaftMsgVoteRsp::AosRaftMsgVoteRsp()
{
}

AosRaftMsgVoteRsp::~AosRaftMsgVoteRsp()
{
}

bool
AosRaftMsgVoteRsp::serializeTo(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);
	aos_assert_r(AosRaftMsg::serializeTo(rdata, buff), false);

	buff->setBool(mResult);
	buff->setU32(mVotedFor);
	buff->setU32(mVotedForTermId);
	return true;
}

bool
AosRaftMsgVoteRsp::serializeFrom(
		AosRundata*  rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);
	aos_assert_r(AosRaftMsg::serializeFrom(rdata, buff), false);

	mResult = buff->getBool(true);
	mVotedFor = buff->getU32(0);
	mVotedForTermId = buff->getU32(0);
	return true;
}

