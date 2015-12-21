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
#include "JimoRaft/RaftMsgAppendEntryReq.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "JimoRaft/RaftServer.h"

AosRaftMsgAppendEntryReq::AosRaftMsgAppendEntryReq(
		RaftMsgType msgType,
		AosRaftServer *server,
		u64 firstLogId,
		u32 prevLogTermId)
:
AosRaftMsg(msgType, server AosMemoryCheckerArgs),
mPrevLogTermId(prevLogTermId),
mFirstLogId(firstLogId)
{
	mLogList.clear();
	mBuff = NULL;
}

//all the members will be set later on
AosRaftMsgAppendEntryReq::AosRaftMsgAppendEntryReq()
:
AosRaftMsg(AosMemoryCheckerArgsBegin)
{
	mLogList.clear();
	mBuff = NULL;
}

AosRaftMsgAppendEntryReq::~AosRaftMsgAppendEntryReq()
{
	for (u32 i = 0; i < mLogList.size(); i++)
		mLogList[i] = NULL;

	mLogList.clear();
	mBuff = NULL;
}

void
AosRaftMsgAppendEntryReq::addLog(AosRaftLogEntryPtr &log)
{
	mLogList.push_back(log);
}

bool
AosRaftMsgAppendEntryReq::serializeTo(
		AosRundata*  rdata,
		AosBuff *buff)
{
	bool rslt;
	aos_assert_r(buff, false);
	rslt = AosRaftMsg::serializeTo(rdata, buff);
	aos_assert_r(rslt, false);

	int num = mLogList.size();

	buff->setU64(mFirstLogId);
	buff->setInt(num);
	buff->setU32(mPrevLogTermId);

	//add raft data
	AosRaftLogEntryPtr log;
	if (num > 0)
	{
		for (u32 i = mFirstLogId; i < mFirstLogId + num; i++)
		{
			log = mLogList[i - mFirstLogId];
			aos_assert_r(mServer, false);
			rslt = log->serializeTo(rdata, buff);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}

bool
AosRaftMsgAppendEntryReq::serializeFrom(
		AosRundata*  rdata,
		AosBuff *buff)
{
	bool rslt;
	aos_assert_r(buff, false);
	rslt = AosRaftMsg::serializeFrom(rdata, buff);
	aos_assert_r(rslt, false);

	mFirstLogId = buff->getU64(0);
	int num = buff->getInt(0);
	mPrevLogTermId = buff->getU32(0);

	//add raft data
	aos_assert_r(mLogList.empty(), false);
	AosRaftLogEntryPtr log;
	for (u32 i = mFirstLogId; i < mFirstLogId + num; i++)
	{
		log = OmnNew AosRaftLogEntry(AosMemoryCheckerArgsBegin);
		RAFT_LOG_TAG(log);
		aos_assert_r(mServer, false);
		rslt = log->serializeFrom(rdata, buff);
		aos_assert_r(rslt, false);
		mLogList.push_back(log);
	}

	return true;
}

OmnString
AosRaftMsgAppendEntryReq::toString()
{
	OmnString msg = AosRaftMsg::toString();
	msg << " PrevLogTermId=" << mPrevLogTermId
		<< " FirstLogId=" << mFirstLogId;
	if (!mLogList.empty())
	{
		msg << " NumLogs=" << mLogList.size();
	}
	else
	{
		msg << " Heartbeat";
	}
	return msg;
}

