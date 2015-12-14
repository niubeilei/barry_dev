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
//    Follower class implements follower's methods in raft protocol
//
// Modification History:
// 2015/05/01 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/RaftServer.h"

#include "Random/RandomUtil.h"
#include "Porting/Sleep.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util1/Time.h"


void
AosRaftServer::followerInit()
{
	init();

	mHeartBeatRecved = 0;
	mLeaderId = -1;
	mLastLogTime = OmnTime::getTimestamp();
}

bool
AosRaftServer::followerMain()
{
	u32 nextElectionAt = 0;
	bool timeout;
	OmnString str;

	//init the main thread
	followerInit();

	//sleep 1 second to make sure networking condition is 
	//fine to avoid unnecessary role change again
	//OmnSleep(1);
	while (mRole == eFollower)
	{
		//clear the counters
		mHeartBeatRecved = 0;
		mVotedFor = -1;

		//sleep sometime to decide a leader down
		timeout = false;
		OmnRandom::init();
		nextElectionAt = OmnRandom::nextU32(
				eMinElectionTimeout, eMaxElectionTimeout);

		if (RAFT_DEBUG)
		{
			OmnScreen << toString() << " election timeout is: "
				<< nextElectionAt << endl;
			OmnScreen << toString() << "follower start to sleep" << endl;
		}

		mSem->timedWait(nextElectionAt, timeout);

		if (RAFT_DEBUG)
		{
			OmnScreen << toString() << "follower end sleeping. timeout is: "
				<< timeout << endl;
		}

		if (mHeartBeatRecved <= 0 && mVotedFor == -1)
		{
			// start an election
			RAFT_ChangeRole(eCandidate);
			break;
		}
	}

	str = timeout ? " Timed out." : "Not timed out.";
	OmnScreen << toString() << "follower run completed."
		<< str << endl;
	return true;
}

//////////////////////////////////////////
//      networking methods
//////////////////////////////////////////
bool
AosRaftServer::followerHandleMsg(
		AosRundata* rdata,
		AosRaftMsg *msg)
{
	aos_assert_r(msg, false);
	bool rslt;
	AosRaftMsg::RaftMsgType msgType = msg->getMsgType();

	rslt = true;
	switch (msgType)
	{
		case AosRaftMsg::eMsgAppendEntryReq:
		case AosRaftMsg::eMsgHeartbeat:
			rslt = followerHandleAppendEntryReq(rdata, (AosRaftMsgAppendEntryReq*)msg);
			break;

		case AosRaftMsg::eMsgVoteReq:
			rslt = handleVoteReq(rdata, (AosRaftMsgVoteReq*)msg);
			break;

		default:
			OmnScreen << toString() << "Got " << 
				AosRaftMsg::getMsgStr(msgType) << " message and don't care. " << endl;
			break;
	}

	return rslt;
}

bool
AosRaftServer::followerHandleAppendEntryReq(
		AosRundata *rdata,
		AosRaftMsgAppendEntryReq *msg)
{
	bool rslt;

	//check the msg's termid
	if (getCurTermId() > msg->getCurTermId())
	{
		//this could happen when network is busy
		OmnScreen << toString() << "Got appendEntry request with term Id ("
				<< msg->getCurTermId() << ") less than mine: " << getCurTermId() << endl;

		//send deny response
		sendAppendEntryRsp(rdata, msg, false);
		return true;
	}

	//since append entry req is also heartbeat, add the
	//heartbeat count if a correct appendEntry message
	mHeartBeatRecved++;
	setLeaderId(msg->getSenderId());

#if 0
	//set the new term id. recvMsg already did it
	mLogMgr.setCurTermId(msg->getCurTermId());
#endif

	//if the firstLogId to be appended > last log id + 1, ask 
	//the leader to send logId from last log Id
	u64 firstLogId = msg->getFirstLogId();
	if (firstLogId > getLastLogId() + 1)
	{
		//this can't be an alarm since when system is busy, out
		//of order packet is pretty frequent
		OmnScreen << toString() << "Get log entries not in order: " << 
			firstLogId << ". Wanted log entry: " << getLastLogId() + 1 << endl;

		sendAppendEntryRsp(rdata, msg, false);
		//even a unwanted appendEntry, not a system erro
		//return true anyway
		return true;
	}

	if (firstLogId > 0 && firstLogId <= getCommitIndex())
	{
		//this can't be an alarm since when system is busy, out
		//of order packet is pretty frequent
		OmnScreen << toString() << "Get log entries already committed: " << 
			firstLogId << ". Wanted log entry: " << getLastLogId() + 1 << endl;

		sendAppendEntryRsp(rdata, msg, false);
		//even a unwanted appendEntry, not a system erro
		//return true anyway
		return true;
	}

	//check if previous log's termId conflict with my previous log's termId
	//If conflict, ask the leader to send log from previous log entry
	u32 prevLogTermId = msg->getPrevLogTermId();
	if (firstLogId > 1)
	{
		u64 prevLogId;
		prevLogId = firstLogId - 1;

		//num is previous log Id
		AosRaftLogEntryPtr prevLog;               
		prevLog = mLogMgr.getLog(rdata, prevLogId);
		aos_assert_r(prevLog, false);
		if (prevLogTermId != prevLog->getTermId())
		{
			OmnAlarm << "Previous log's termId not match:" << 
				" local=" << prevLog->getTermId() << " remote=" <<
				prevLog << enderr;

			//remove logs from previous log
			mLogMgr.removeLogs(rdata, prevLogId);

			//the rsp message will tell the leader to 
			//send the previous logs
			sendAppendEntryRsp(rdata, msg, false);

			//even a unwanted appendEntry, not a system erro
			//return true anyway
			return true;
		}
	}	

	AosRaftLogEntryPtr msgLog = NULL;
	AosRaftLogEntryPtr localLog = NULL;
	if (msg->getNumLogs() > 0)
	{
		OmnScreen << toString() << "Log data received: "
			<< msg->getNumLogs() << " entries. First logid is: " 
			<< firstLogId << endl;

		//append all entries to log
		//1. if the log exists in the logMap with the 
		//    same term Id, ignore this log
		//2. If exist, but with different term id, this is
		//   a conflict log, remove the log and subsequent
		//   logs
		//3. If not existing, append the log
		for (int i = 0; i < msg->getNumLogs(); i++)
		{
			msgLog = msg->getLog(i);
			aos_assert_r(msgLog, false);

			localLog = mLogMgr.getLog(rdata, firstLogId + i);
			if (localLog)
			{
				//ignore the log if existing and same
				if (localLog->getTermId() == msgLog->getTermId())
					continue;

				//a conflict log, need to remove the log to the end
				mLogMgr.removeLogs(rdata, firstLogId + i);
			}

			rslt = mLogMgr.appendLog(rdata, msgLog);
			aos_assert_r(rslt, false);
		}

		//Successfully get log messages, update timer
		mLastLogTime = OmnTime::getTimestamp();
	}

	//If leader's Commit > commitIndex,
	// set commitIndex = min(leaderCommit, last log index)
	u64 leaderCommit = msg->getCommitIndex();
	u64 myCommit = getCommitIndex();
	if (myCommit < leaderCommit)
	{
		u64 maxLogIdToCommit = 0;
		if (getLastLogId() < leaderCommit)
			maxLogIdToCommit = getLastLogId();
		else
			maxLogIdToCommit = leaderCommit;

		mLogMgr.setCommitIndex(maxLogIdToCommit);
	}

	//Reply to the sender
	u64 curTime = OmnTime::getTimestamp();
	i64 timeDelay = curTime - mLastLogTime;
	if (msg->getLastLogId() > getLastLogId() &&
		timeDelay > eMaxMsgDelay)
	{
		//This means the leader needs to send me
		//my missing logs
		sendAppendEntryRsp(rdata, msg, false);
	}
	else
	{
		sendAppendEntryRsp(rdata, msg, true);
	}

	//apply non-committed logs to state machine if any
	applyCommittedLogs(rdata);
	return true;
}

