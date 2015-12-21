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
//    Leader class implements leader's methods in raft protocol
//
// Modification History:
// 2015/05/01 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/RaftServer.h"

#include "Porting/Sleep.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/Ptrs.h"


void
AosRaftServer::leaderInit()
{
	init();

	RaftPeerMap::iterator itr;
	AosRaftPeer *peer;

	//set peers' logId to be synced
	itr = mPeerMap.begin();
	while (itr != mPeerMap.end())
    {
		if (itr->first == mServerId)
			continue;

		//reset peer's log to be synced
		peer = &(itr->second);
        peer->setLastAgreed(0);
		peer->setNextToReplicate(getLastLogId() + 1);

        itr++;
    }

	mLeaderId = mServerId;
	mHeartBeatSent = 0;

}

bool
AosRaftServer::leaderMain()
{
	bool timeout;
	OmnString str;

	//init the thread
	leaderInit();

	while(mRole == eLeader)
	{
		if (RAFT_DEBUG)
		{
			RAFT_OmnScreen << "Start a new heartbeat round. " << endl;
		}

		//send heartbeat message
		broadcastHeartbeat(mRundataRaw);

		//conditional wait on an event for some time
		timeout = false;
		//the interval unit for timedWait is micro-sec
		mSem->timedWait(eHeartbeatInterval, timeout);
	}

	str = timeout ? " Timed out." : "Not timed out.";
	RAFT_OmnScreen << "leader run completed. " << str << endl;
	return true;
}

//////////////////////////////////////////
//      networking methods
//////////////////////////////////////////

//
//Send log entries to followers based on 
//the follower's requirement
//
bool
AosRaftServer::sendAppendEntryReq(
		AosRundata*  rdata,
		u32 recvId,
		u64 logId)
{
	aos_assert_r(getLastLogId() >= logId, false);

	//Get prevlog term firstly
	u32 prevLogTermId = 0;
	AosRaftLogEntryPtr prevLog;
	if (logId > 1)
	{
		prevLog = mLogMgr.getLog(rdata, logId - 1);
		prevLogTermId = prevLog->getTermId();
	}

	//build the message with a list of log entry
	//AosRaftMsgAppendEntryReq msg(this, logId, &logList, prevLogTermId);
	AosRaftMsgAppendEntryReq msg(AosRaftMsg::eMsgAppendEntryReq, this, logId, prevLogTermId);
	AosRaftLogEntryPtr log;
	bool rslt;

	//Generate log list. Firstly decide number of logs	
	u64 maxLogId = logId + RAFT_MAX_LOG_IN_MSG - 1;
	if (maxLogId > getLastLogId())
		maxLogId = getLastLogId();

	AosBuffPtr buff;
	for (u64 i = logId; i <= maxLogId; i++)
	{
		log = mLogMgr.getLog(rdata, i);

		//make sure data is ready
		buff = log->getData(rdata, getStatMach());
		aos_assert_r(buff, false);

		msg.addLog(log);
	}

	//send the message
	//if (RAFT_DEBUG)
	//{
		RAFT_OmnScreen << "Start to sync Log data with slow server " 
			<< recvId << " from " << logId << " to " << maxLogId << endl;
	//}

	rslt = sendMsg(rdata, &msg, recvId);
	if (rslt)
	{
		//update next to replicate number
		AosRaftPeer *peer = getPeer(recvId);
		aos_assert_r(peer, false);
		//RAFT_OmnScreen << "Set next to replicate to " 
		//	<< maxLogId + 1 << " on peer " << recvId << endl;
		//peer->setNextToReplicate(maxLogId + 1);
	}

	if (RAFT_DEBUG)
	{
		RAFT_OmnScreen << "Finished syncing Log data with server " 
			<< recvId << " from " << logId << " to " << maxLogId << endl;
	}

	return true;
}

bool
AosRaftServer::broadcastHeartbeat(AosRundata*  rdata)
{
	//send the message to all the peers
	map<u32, AosRaftPeer>::iterator itr = mPeerMap.begin();
	AosRaftPeer *peer = NULL;
	bool rslt;

	u64 logId = getLastLogId();
	u32 prevLogTermId = 0;
	AosRaftLogEntryPtr prevLog;

	if (logId > 1)
	{
		prevLog = mLogMgr.getLog(rdata, logId - 1);
		prevLogTermId = prevLog->getTermId();
	}

	mHeartBeatSent++;
	if (mHeartBeatSent % 200 == 0)
		RAFT_OmnScreen << "Broadcast heartbeat times=" << mHeartBeatSent << endl;

	//build the message without any log entry which means
	//a heartbeat message only
	AosRaftMsgAppendEntryReq msg(AosRaftMsg::eMsgHeartbeat, this, 0, prevLogTermId);
	while (itr != mPeerMap.end())
	{
		peer = &(itr->second);

		//send the message
		rslt = sendMsg(rdata, &msg, peer->getServerId());
		aos_assert_r(rslt, false);

		itr++;
	}

	return true;
}

bool
AosRaftServer::broadcastAppendEntryReq(
		AosRundata*  rdata,
		AosRaftLogEntryPtr &log)
{
	//send the message to all the peers
	map<u32, AosRaftPeer>::iterator itr = mPeerMap.begin();
	AosRaftPeer *peer = NULL;
	bool rslt;

	u32 prevLogTermId = 0;
	AosRaftLogEntryPtr prevLog;
	u64 logId = log->getLogId();
	if (logId > 1)
	{
		prevLog = mLogMgr.getLog(rdata, logId - 1);
		prevLogTermId = prevLog->getTermId();
	}

	//prepare the appendentry message
	//get the list of log entry from logId
	AosRaftMsgAppendEntryReq msg(AosRaftMsg::eMsgAppendEntryReq, this, logId, prevLogTermId);
	msg.addLog(log);

	AosBuffPtr buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
	buildMsg(rdata, &msg, buff);

	if (RAFT_DEBUG)
		RAFT_OmnScreen << "Start to broadcast new Log data " << logId << endl;

	while (itr != mPeerMap.end())
	{
		peer = &(itr->second);
		if (peer->getNextToReplicate() == logId)
		{
			//Not sending new data to a follower in piggy-back mode
			//send the heartbeat message only
			rslt = sendMsg(rdata, &msg, peer->getServerId(), buff);
			aos_assert_r(rslt, false);
			//peer->setNextToReplicate(logId + 1);
		}

		itr++;
	}

	if (RAFT_DEBUG)
		RAFT_OmnScreen << "Finished broadcasting new Log data " << logId << endl;
	return true;
}

bool
AosRaftServer::leaderHandleMsg(
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
			rslt = leaderHandleAppendEntryReq(rdata, (AosRaftMsgAppendEntryReq*)msg);
			break;

		case AosRaftMsg::eMsgAppendEntryRsp:
			rslt = leaderHandleAppendEntryRsp(rdata, (AosRaftMsgAppendEntryRsp*)msg);
			break;

		case AosRaftMsg::eMsgVoteReq:
			rslt = leaderHandleVoteReq(rdata, (AosRaftMsgVoteReq*)msg);
			break;

		default:
			RAFT_OmnScreen << "Got " << 
				AosRaftMsg::getMsgStr(msgType) << " message and don't care. " << endl;
			break;
	}

	if (!rslt)
	{
		RAFT_OmnAlarm << 
			"Failed to handle message: " << msg->toString() <<enderr;
	}

	return true;
}

//
// This means two leaders exist and get 
// a message from the other leader
//
bool
AosRaftServer::leaderHandleAppendEntryReq(
		AosRundata* rdata,
		AosRaftMsgAppendEntryReq *msg)
{
    //we've found a leader who is legitimate
	RAFT_OmnScreen << "Multiple leaders --- me:" << 
		getServerId() << " and peer:" << msg->getSenderId() << endl;

	//If I am more qualified due to term Id, send response
	//to the bad leader
	if (getCurTermId() > msg->getCurTermId())
	{
		return sendAppendEntryRsp(rdata, msg, false);
	}

	//If can't tell, for simplicity, just change to be candidate 
	//and let the election process to determin the right leader
	//
	//Actually, it is nearly impossible to happen, since the 
	//different leader must be elected in different terms therefore
	//have different termid. But in case that ....... 
	RAFT_OmnScreen << 
		"Multiple leaders with the same term id!" << endl;
	return RAFT_ChangeRole(eCandidate);
}

bool
AosRaftServer::leaderHandleAppendEntryRsp(
		AosRundata*  rdata,
		AosRaftMsgAppendEntryRsp *msg)
{
	//get the peer node replying message
	u32 peerId = msg->getSenderId();
	AosRaftPeer *peer = getPeer(peerId);
	if (RAFT_DEBUG)
		RAFT_OmnScreen << "dumping peer info:" << peer->toString() << endl;

	u64 logId = msg->getLastLogId();
	AosRaftLogEntryPtr log = NULL;

	//last agreed Id should not go back
	//But it is possible that the previous append 
	//entry's reply back later 
	//
	//if so, just ignore this message
	if (logId < peer->getLastAgreed())
	{
		//this should not be an alarm since when system is busy, out
		//of order packet is pretty frequent
		RAFT_OmnScreen << "Peer:" << peerId <<
			" sent back an out-of-order appendEntryRsp with lastLogId: " <<
			logId << endl;

		return true;
	}

	//get the peer node
	//if (msg->getResult() == true)
	//{
		//the receiver get an old logId successfully
		if (logId > peer->getLastAgreed())
		{
			//If has more accepted log entries, need to update:
			//1. log entry received status
			//2. update commit index accordingly
			int count;
			for (u64 i = getCommitIndex() + 1; i <= logId; i++)
			{
				log = mLogMgr.getLog(rdata, i);
				aos_assert_r(log, false);

				//1. log entry received status
				count = log->incReceivedCount();

				//2. update commit index accordingly
				// > 1/2 and in current term, can be committed
				//this log and previous log can be committed
				if (log->getTermId() == getCurTermId() &&
						count > getTotalServers() / 2)
					mLogMgr.setCommitIndex(i);
			}

			//set peer's last agreed logId
			peer->setLastAgreed(logId);

			//set nextIndex accordingly
			//commented out for now. Need more testing ???
			peer->setNextToReplicate(logId + 1);

			if (logId % RAFT_SAMPLE_PRINT == 0)
			{
				RAFT_OmnScreen << "Sync log " << logId 
					<< " to peer " << peerId 
					<< " successfully (every "  
					<< RAFT_SAMPLE_PRINT << ")" << endl;
			}
		}
	//}
	//else
	if (msg->getResult() == false)
	{
		//failed to replicate the log
		//Do nothing for now and just add a log message
		RAFT_OmnScreen << "Failed to replicate log: The follower's last agreed logId is " << logId
				<< " on peer " << peerId << endl;

		//reset peer's nextIndex
		//RAFT_OmnScreen << "Set next to replicate to " 
		//	<< logId + 1 << " on peer " << peerId << endl;
		//peer->setNextToReplicate(logId + 1);
	}

	//check if need to send subsequent logId
	bool rslt;
	u64 replicateLogId = peer->getNextToReplicate();
	if (replicateLogId <= getLastLogId())
	{
		//send a group of log entries from the next logId
		//the follower wanted
		rslt = sendAppendEntryReq(rdata, peerId, replicateLogId);
		aos_assert_r(rslt, false);
	}

	//apply non-committed logs to state machine if any
	applyCommittedLogs(rdata);
	return true;
}

bool
AosRaftServer::leaderHandleVoteReq(
		AosRundata*  rdata,
		AosRaftMsgVoteReq *msg)
{
	aos_assert_r(msg, false);

	//send deny voteRsp message
	sendVoteRsp(rdata, msg, false);
	RAFT_OmnScreen << "I am a more qualified leader" << endl;

	return true;
}

//
//recv client data and put in the queue
//
bool
AosRaftServer::handleClientData(
		AosRundata* rdata,
		AosJimoCall& jimocall,
		AosBuffPtr &buff)
{
#if 0
	//It is in a role change stage. Do not
	//process the client requests and contineu to
	//process in the new role
	if (mRole != eLeader)
	{
		return false;
	}
#endif

	//Clone the jimocall parameter
	AosJimoCallPtr call;
	call = OmnNew AosJimoCall(jimocall);

	//Lock the data queue
	mDataLock->lock();

	//Create a new log entry and set data
	AosRaftLogEntryPtr log = OmnNew AosRaftLogEntry(AosMemoryCheckerArgsBegin);
	RAFT_LOG_TAG(log);
	log->setData(buff);
	log->setJimoCall(call.getPtrNoLock());

	//Get the new log entry in the queue
	mDataQueue.push_back(log);

	//Release the data queue lock and notify data
	//handlig thread to handle the new data
	mDataLock->unlock();
	mDataSem->post();

	u64 logId = getLastLogId();
	if (logId % RAFT_SAMPLE_PRINT == 0)
	{
		RAFT_OmnScreen << "Append user data " 
			<< logId << " (every "  
			<< RAFT_SAMPLE_PRINT << ")" << endl;
	}

#if 0
	//wait until the data is committed
	//set a timeout to be 5 seconds
	u32 count = 5000000;
	while (count > 0 && mRole == eLeader)
	{
		if (log->isApplied()) return true;

		OmnUsSleep(10);
		count--;
	}

	RAFT_OmnAlarm << "wait time out but log is still not applied" << enderr;
#endif

	return true;
}

//
//handle client data
//
bool
AosRaftServer::dataThreadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	RAFT_OmnScreen << "Client data thread started." << endl;

	bool rslt;
	AosRaftLogEntryPtr log;

	while (state == OmnThrdStatus::eActive)
	{
		//only process data for leader
		while (mRole != eLeader) 
		{
			//remove client data not to be handled
			clearClientData();

			//wait for role change notification
			mDataSem->wait();
			//continue;
		}

		mDataLock->lock();
		if (!mDataQueue.empty())
		{
			log = mDataQueue.front();
			mDataQueue.pop_front();
			mDataLock->unlock();

			aos_assert_r(log, false);
			//handle the client data
			rslt = mLogMgr.appendLog(mRundataRaw, log);
			aos_assert_r(rslt, false);
			log->incReceivedCount();
			broadcastAppendEntryReq(mRundataRaw, log); 
		}
		else
		{
			mDataLock->unlock();
			//wait until a client data arrived
			mDataSem->wait();
		}
	}

	RAFT_OmnScreen << "Client data thread ended." << endl;
	return true;
}

//
//clear all the unhandled client data if any
//
void
AosRaftServer::clearClientData()
{
	RAFT_OmnScreen << "Start to clear unhandled client data:" 
		<< mDataQueue.size() << endl;

	int num = mDataQueue.size();
	mDataLock->lock();
	for (u32 i = 0; i < mDataQueue.size(); i++)
		mDataQueue.pop_back();
	mDataLock->unlock();

	RAFT_OmnScreen << "Cleared unhandled client data:" 
		<< num << endl;
}
