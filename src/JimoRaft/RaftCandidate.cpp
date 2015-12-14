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
//    Candidate class implements Candidate's methods in raft protocol
//
// Modification History:
// 2015/05/01 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/RaftServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Random/RandomUtil.h"

void
AosRaftServer::candidateInit()
{
	init();
	mNumVotedForMe = 0;
	mLeaderId = -1;
}

bool
AosRaftServer::candidateMain()
{
	u32 nextElectionAt = 0;

	//init the thread
	candidateInit();

	if (mPeerMap.size() == 0)
	{
		//only one node, change to leader
		return RAFT_ChangeRole(eLeader);
	}

	bool timeout;
	OmnString str;
	while (mRole == eCandidate)
	{
		//initialize a new term round
		mLogMgr.incCurTermId();

		//vote for myself 
		mVotedFor = mServerId;	
		mVotedForTermId = getCurTermId();
		mNumVotedForMe = 1;

		//send vote request message
		broadcastVoteReq(mRundataRaw);

		//OmnMsSleep(nextElectionAt);
		timeout = false;
		OmnRandom::init();
		nextElectionAt = OmnRandom::nextU32(
				eMinElectionTimeout, eMaxElectionTimeout);
		OmnScreen << toString() << " election timeout is: "
			<< nextElectionAt << endl;
		//mSem->timedWait(nextElectionAt * 1000, timeout);
		mSem->timedWait(nextElectionAt, timeout);

		str = timeout ? " Timed out." : "Not timed out.";
		OmnScreen << toString() << "sleep completed, mVotedFor is: " 
			<< mVotedFor << ", mNumVotedForMe is: " 
			<< mNumVotedForMe << ", " << str << endl;
	}

	OmnScreen << toString() << "candidate run completed." << endl;
	return true;
}

//////////////////////////////////////////
//      message relevant methods
//////////////////////////////////////////
bool
AosRaftServer::broadcastVoteReq(
				AosRundata*  rdata)
{
	AosRaftMsgVoteReq msg(this);
	bool rslt;

	map<u32, AosRaftPeer>::iterator itr = mPeerMap.begin();
	while (itr != mPeerMap.end())
	{
		rslt = sendMsg(rdata, &msg, &(itr->second));
		aos_assert_r(rslt, false);
		itr++;
	}
	return true;
}

bool
AosRaftServer::candidateHandleMsg(
		AosRundata* rdata,
		AosRaftMsg *msg)
{
	bool rslt;
	AosRaftMsg::RaftMsgType msgType = msg->getMsgType();

	rslt = true;
	switch (msgType)
	{
		case AosRaftMsg::eMsgAppendEntryReq:
		case AosRaftMsg::eMsgHeartbeat:
			rslt = candidateHandleAppendEntryReq(rdata, (AosRaftMsgAppendEntryReq*)msg);
			break;

		case AosRaftMsg::eMsgVoteReq:
			rslt = handleVoteReq(rdata, (AosRaftMsgVoteReq*)msg);
			break;

		case AosRaftMsg::eMsgVoteRsp:
			rslt = candidateHandleVoteRsp(rdata, (AosRaftMsgVoteRsp*)msg);
			break;

		default:
			OmnScreen << toString() << "Got " << 
				AosRaftMsg::getMsgStr(msgType) << " message and don't care. " << endl;
			break;
	}

	return rslt;
}

bool
AosRaftServer::candidateHandleAppendEntryReq(
		AosRundata*  rdata,
		AosRaftMsgAppendEntryReq *msg)
{
	aos_assert_r(msg, false);

	//check the msg's termid
	if (getCurTermId() > msg->getCurTermId())
	{
		OmnScreen << "Get appendEntry request with term Id ("
				<< msg->getCurTermId() << ") less than mine: " << getCurTermId() << endl;

		//send a deny message
		sendAppendEntryRsp(rdata, msg, false);

		//even not accept the new log entry, still return true
		return true;
	}

	//recvMsg() already handle this case
	//mLogMgr.setCurTermId(msg->getCurTermId());
	return RAFT_ChangeRole(eFollower);
}

bool
AosRaftServer::candidateHandleVoteRsp(
		AosRundata*  rdata,
		AosRaftMsgVoteRsp *msg)
{
    if (msg->getVotedFor() == mServerId &&
		msg->getVotedForTermId() == getCurTermId())
    {
        mNumVotedForMe++;
        if (mNumVotedForMe > (mPeerMap.size() + 1) / 2)
		{
			//get most votes, change to leader
            RAFT_ChangeRole(eLeader);
		}
    }

    return true;
}

