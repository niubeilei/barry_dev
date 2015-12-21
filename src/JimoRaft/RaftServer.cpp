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
//
// Modification History:
// 2015/05/01 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/RaftServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "JimoAPI/JimoRaftAPI.h"
#include "SEInterfaces/ClusterMgrObj.h"
#include "SEInterfaces/ClusterObj.h"
#include "API/AosApi.h"

/////////////////////////////////////////////////
//constructor & destructor
/////////////////////////////////////////////////
AosRaftServer::AosRaftServer()
{
}

AosRaftServer::AosRaftServer(
		AosRundata								*rdata,
		int										cubeId,
		const vector<AosCubeMapObj::CubeInfo>	&peer_cubes,
		AosRaftStateMachinePtr					stateMachine)
{
	mServerId = AosGetSelfServerId();
	if (mServerId < 0)
	{
		OmnThrowException("invalid_server_id");
		return;
	}

	//init the rundata
	mRundataRaw = rdata;
	mRundata = rdata;

	//init log manager
	mLogMgr.init(rdata, this, stateMachine);

	//init peer map based on the configuration and cluster
	aos_assert(peer_cubes.size() > 0);
	bool rslt = getPeers(rdata, peer_cubes);
	aos_assert(rslt);

	//Init threads
	OmnThreadedObjPtr thisPtr(this, false);

	//init message handling objects
	mLock = OmnNew OmnMutex();
	mSem = OmnNew OmnSem(0);
	mMainThread = OmnNew OmnThread(thisPtr,
			"MainThread", 1, true, true, __FILE__, __LINE__);

	//init message handling objects
	mMsgLock = OmnNew OmnMutex();
	mMsgSem = OmnNew OmnSem(0);
	mMsgThread = OmnNew OmnThread(thisPtr,
			"MsgThread", 2, true, true, __FILE__, __LINE__);

	//init message handling objects
	mDataLock = OmnNew OmnMutex();
	mDataSem = OmnNew OmnSem(0);
	mDataQueue.clear();
	mDataThread = OmnNew OmnThread(thisPtr,
			"DataThread", 3, true, true, __FILE__, __LINE__);

	//default role is follower
	mRole = eFollower;
}

AosRaftServer::~AosRaftServer()
{
}

void
AosRaftServer::init()
{
	mVotedFor = -1;
	mVotedForTermId = 0;

	//clear old messages
	mMsgQueue.clear();

	//Clear old client data
	mDataQueue.clear();

	//For non-replied log entry, need to 
	//send failure message
	mLogMgr.notifyJimoCall(mRundataRaw);

}

//////////////////////////////////////////////////
//            thread relevant methods
//////////////////////////////////////////////////
bool
AosRaftServer::start()
{
	mMainThread->start();
	mMsgThread->start();
	mDataThread->start();

	return true;
}

bool
AosRaftServer::stop()
{
	//notify the main thread to stop
	mSem->post();
	mMsgSem->post();
	mMsgThread->stop();

	if (mRole == eLeader)
	{
		mSem->post();
		mDataSem->post();
		mDataThread->stop();
	}

	return true;
}

bool
AosRaftServer::notify()
{
	//notify all the threads
	if (mRole != eCandidate)
		mSem->post();  //this is driven by the main thread already

	mMsgSem->post();
	mDataSem->post();

	return true;
}

bool	
AosRaftServer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	switch (thread->getLogicId())
	{
	case 1 :
		 return mainThreadFunc(state, thread);

	case 2 :
		 return msgThreadFunc(state, thread);

	case 3 :
		 return dataThreadFunc(state, thread);

	default :
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


///////////////////////////////////////////////////////
//  Main thread relevant methods
///////////////////////////////////////////////////////
bool	
AosRaftServer::mainThreadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		//init the members based on new role
		//init();
		switch (mRole)
		{
			case eLeader:
				leaderMain();
				break;

			case eFollower:
				followerMain();
				break;

			case eCandidate:
				candidateMain();
				break;

			default:
				OmnShouldNeverComeHere;
		}

		//clear old messages
		clearMessages();
	}

	return true;
}

///////////////////////////////////////////////////////
//  Message thread relevant methods
///////////////////////////////////////////////////////
//
//this thread take one raft message from the message
//queue and handle it
//
bool	
AosRaftServer::msgThreadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	AosRaftMsgPtr msg;

	RAFT_OmnScreen << "Message thread started." << endl;
	while (state == OmnThrdStatus::eActive)
	{
		mMsgLock->lock();
		if (!mMsgQueue.empty())
		{
			//get a message from the front
			msg = mMsgQueue.front();
			mMsgQueue.pop_front();
			mMsgLock->unlock();

			aos_assert_r(msg, false);
			//handle the message
			handleMsg(mRundataRaw, msg.getPtr());
		}
		else
		{
			mMsgLock->unlock();
			//wait until a message arrived
			mMsgSem->wait();
		}
	}

	//clear all the unhandled messages
	//clearMessages();	

	RAFT_OmnScreen << "Message thread ended." << endl;
	return true;
}

bool
AosRaftServer::recvMsg(
		AosRundata* rdata,
		AosBuff *buff)
{
	aos_assert_r(buff, false);

	AosRaftMsgPtr msg = NULL;
	//AosRaftMsgPtr msg = NULL;
	AosRaftMsg::RaftMsgType msgType;

	//the first 4 bytes in buff is the message type
	//the 2nd 4 bytes is the sender Id
	//the 3rd 4 bytes is the sender's term id

	//keep the message start pointer in the buffer
	i64 idx = buff->getCrtIdx();
	msgType = (AosRaftMsg::RaftMsgType)(buff->getU32(0));

	//recover the pointer back to the start of the message
	buff->setCrtIdx(idx);
	switch (msgType)
	{
		case AosRaftMsg::eMsgAppendEntryReq:
		case AosRaftMsg::eMsgHeartbeat:
			msg = OmnNew AosRaftMsgAppendEntryReq();
			break;

		case AosRaftMsg::eMsgAppendEntryRsp:
			msg = OmnNew AosRaftMsgAppendEntryRsp();
			break;

		case AosRaftMsg::eMsgVoteReq:
			msg = OmnNew AosRaftMsgVoteReq();
			break;

		case AosRaftMsg::eMsgVoteRsp:
			msg = OmnNew AosRaftMsgVoteRsp();
			break;

		default:
			RAFT_OmnScreen << "get an invalid message" << endl;
			//mLock->unlock();
			return false;
	}

	msg->setServer(this);
	msg->serializeFrom(rdata, buff);
	if (RAFT_DEBUG)
	{
		RAFT_OmnScreen <<
			"Received new message: " << msg->toString() << endl;
	}

	mMsgLock->lock();
	mMsgQueue.push_back(msg);
	mMsgLock->unlock();
	if (RAFT_DEBUG)
	{
		if (mMsgQueue.size() > 15)
		{
			RAFT_OmnScreen << "Too many messages to handle: " <<
				mMsgQueue.size() << endl;
		}
	}

	mMsgSem->post();
	return true;
}

bool
AosRaftServer::handleMsg(
		AosRundata* rdata,
		AosRaftMsg *msg)
{
	aos_assert_r(msg, false);
	bool rslt;
	u32 termId = msg->getCurTermId();

	//It is in a role change stage. Do not
	//process the message and continue to
	//process in the new role
	if (getCurTermId() < termId)
	{
		stepDown(termId);
	}

	rslt = true;
	switch (mRole)
	{
		case eLeader:
			rslt = leaderHandleMsg(rdata, msg);
			break;

		case eFollower:
			rslt = followerHandleMsg(rdata, msg);
			break;

		case eCandidate:
			rslt = candidateHandleMsg(rdata, msg);
			break;

		default:
			OmnShouldNeverComeHere;
	}

	if (!rslt)
	{
		RAFT_OmnAlarm << 
			"Failed to handle message: " << msg->toString() <<enderr;
	}

	return true;
}

//
//clear all the unhandled messages if any
//
void
AosRaftServer::clearMessages()
{
	RAFT_OmnScreen << "Start to clear unhandled messages:" 
		<< mMsgQueue.size() << endl;

	int num = mMsgQueue.size();
	mMsgLock->lock();
	for (u32 i = 0; i < mMsgQueue.size(); i++)
		mMsgQueue.pop_back();
	mMsgLock->unlock();

	RAFT_OmnScreen << "Cleared unhandled messages:" 
		<< num << endl;
}

////////////////////////////////////////////////////
//   Common message relevant methods
////////////////////////////////////////////////////
bool
AosRaftServer::sendMsg(
		AosRundata* rdata,
		AosRaftMsg *msg,
		u32 recvId)
{
	aos_assert_r(msg, false);

	//use jimocall to send the message
	AosBuffPtr buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
	buildMsg(rdata, msg, buff);

	return sendMsg(rdata, msg, recvId, buff);
}

bool
AosRaftServer::sendMsg(
		AosRundata* rdata,
		AosRaftMsg *msg,
		AosRaftPeer *peer)
{
	aos_assert_r(msg, false);
	aos_assert_r(peer, false);
	return sendMsg(rdata, msg, peer->getServerId());
}

bool
AosRaftServer::sendMsg(
		AosRundata* rdata,
		AosRaftMsg *msg,
		u32 recvId,
		AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	int tries = 3;

	//send message. Try at most 3 times if not succeeded already
	if (RAFT_DEBUG)
	{
		RAFT_OmnScreen << 
			"Start to send message to peer " << 
			recvId << msg->toString() << endl;
	}

	for (int i = 0; i < tries; i++)
	{
		if (Jimo::raftSendMsgAsync(rdata, recvId, buff.getPtr()))
		{
			if (RAFT_DEBUG)
			{
				RAFT_OmnScreen << "finished sending message " 
					<< msg->getMsgStr() << " to peer " << recvId << endl;
			}
			return true;
		}

		//log an sending failure message
		RAFT_OmnScreen << "Failed one time to send " 
			<< msg->getMsgStr() <<" to " << recvId << endl;

		//sleep 10 millisecond for next try
		OmnMsSleep(10); 
	}

	//never return false for now
	return true;
}

//
//build an message into a buff
//
AosBuffPtr
AosRaftServer::buildMsg(
		AosRundata* rdata,
		AosRaftMsg *msg,
		AosBuffPtr buff)
{
	//the following 3 fields will be consumed
	//by the DocFileMgr in the receiver side
	buff->setU64(mJPId);
	buff->setU64(mAseId);
	buff->setU64(mCubeId);

	//append msg data into the buffer
	//RAFT_OmnScreen << msg->toString() << endl;
	msg->serializeTo(rdata, buff.getPtr());
	return buff;
}

bool
AosRaftServer::broadcastMsg(
		AosRundata* rdata,
		AosRaftMsg *msg)
{
	RaftPeerMap::iterator itr;
	AosRaftPeer peer;
	bool rslt;

	itr = mPeerMap.begin();
	while (itr != mPeerMap.end())
	{
		peer = itr->second;
		rslt = sendMsg(rdata, msg, &peer);
		aos_assert_r(rslt, false);

		itr++;
	}

	return true;
}

////////////////////////////////////////////////////
//      common individual message methods
////////////////////////////////////////////////////
bool
AosRaftServer::handleVoteReq(
		AosRundata* rdata,
		AosRaftMsgVoteReq *msg)
{
	//follower and candidate share the same handling
	aos_assert_r(msg, false);
	if (getCurTermId() > msg->getCurTermId())
	{
		RAFT_OmnScreen << "Got vote req from server:" << msg->getSenderId() << " and my current termid:"
				<< getCurTermId() << " > msg->getCurTermId():" << msg->getCurTermId() << endl;

		sendVoteRsp(rdata, msg, false);
		return true;
	}

	//if (mVotedFor >= 0 && mVotedFor != (int)(msg->getSenderId()))
	if (mVotedFor >= 0)
	{
		//already voted
		RAFT_OmnScreen << "Already voted for " << mVotedFor << endl;
		return true;
	}

	if (cmpLogUpToDateWithMe(rdata, msg) <= 0)
	{
		//grant vote
		mVotedFor = (int)(msg->getSenderId());
		mVotedForTermId = msg->getCurTermId();

		RAFT_OmnScreen << "Vote for " << mVotedFor
				<< " for term " << mVotedForTermId << endl;
		sendVoteRsp(rdata, msg, true);
	}

	return true;
}

bool
AosRaftServer::sendVoteRsp(
		AosRundata* rdata,
		AosRaftMsg *msg,
		bool result)
{
	aos_assert_r(msg, false);

    //vote for the requester. Build the voting response
    AosRaftMsgVoteRsp replyMsg(this, result, mVotedFor, mVotedForTermId);

	replyMsg.setReqTimeTag(msg->getTimeTag());
    sendMsg(rdata, &replyMsg, msg->getSenderId());
	return true;
}

bool 
AosRaftServer::sendAppendEntryRsp(
		AosRundata* rdata,
		AosRaftMsg *msg,
		bool result)
{
	aos_assert_r(msg, false);
	AosRaftMsgAppendEntryRsp replyMsg(this, result);

	replyMsg.setReqTimeTag(msg->getTimeTag());
	sendMsg(rdata, &replyMsg, msg->getSenderId());
	return true;
}

////////////////////////////////////////////////////
//  Raft protocol methods (role management)
////////////////////////////////////////////////////
bool
AosRaftServer::changeRole(
		RaftRole role,
		OmnString file,
		int line)
{
	if (mRole == role)
	{
		//if already in the right role, no need to change
		RAFT_OmnScreen 
			<< "Current role is already: " << getRoleStr(role) << endl;

		return false;
	}

	//log the status change
	if (mRole == eLeader)
	{
		//this should not be an alarm since when system is busy, out
		//of order packet is pretty frequent
		RAFT_OmnScreen << "(" << file << ":" << line << ")Role changed from " 
			<< getRoleStr(mRole) << " to " << getRoleStr(role) << endl;
	}
	else
	{
		RAFT_OmnScreen << "(" << file << ":" << line << ")Role changed from " 
			<< getRoleStr(mRole) << " to " << getRoleStr(role) << endl;
	}

	//change the role
	mRole = role;
	notify();

	return true;
}

//
//check if the message sender's log is up-to-date
//with my log or not
//
//1 :  mine is more up-to-date
//-1:  the sender is more up-to-date
//0 :  we are equal
//
int
AosRaftServer::cmpLogUpToDateWithMe(
		AosRundata* rdata,
		AosRaftMsg *msg)
{
	AosRaftMsgVoteReq *msgVoteReq = dynamic_cast<AosRaftMsgVoteReq*>(msg);

	if (!msgVoteReq)
	{
		RAFT_OmnAlarm << "Not a VoteReq message in log up-to-date comparison: "
			<< msg->toString() << enderr;

		return 1;
	}

	//compare last log term
	if (getLastLogTerm(rdata) > msgVoteReq->getLastLogTerm())
		return 1;

	if (getLastLogTerm(rdata) < msgVoteReq->getLastLogTerm())
		return -1;

	//coming here, lastLogTerm must be the same
	if (getLastLogId() > msgVoteReq->getLastLogId())
		return 1;

	if (getLastLogId() < msgVoteReq->getLastLogId())
		return -1;

	return 0;
}

bool 
AosRaftServer::applyCommittedLogs(AosRundata* rdata)
{
	bool rslt;
	AosRaftLogEntryPtr log;
	u64 logApplyStart = getLastLogIdApplied() + 1;

	if (logApplyStart > getCommitIndex())
	{
		//nothing to apply
		return true;
	}

	//Apply the log 
	for (u64 i = logApplyStart; i <= getCommitIndex(); i++)
	{
		log = mLogMgr.getLog(rdata, i);
		aos_assert_r(log, false);

		rslt = log->apply(rdata, getStatMach());
		if (!rslt)
		{
			//Actually, State machine should never fail to apply!!!
			RAFT_OmnAlarm << "Failed to apply logid:" << i << enderr;
			return false;
		}

	}

	mLogMgr.setLastLogIdApplied(rdata, getCommitIndex());
	return true;
}


void
AosRaftServer::stepDown(const u32 termId)
{
	RAFT_OmnScreen << "Step down due to lower termId:"
		<< "OldTermId=" << getCurTermId() << "NewTermId=" << termId << endl;

	mLogMgr.setCurTermId(termId);
	mVotedFor = -1;
	mVotedForTermId = 0;
	mLeaderId = -1;

	//change to follower
	RAFT_ChangeRole(eFollower);
}

u32 
AosRaftServer::getLastLogTerm(AosRundata* rdata)
{
	if (getLastLogId() < 1) return 0;

	AosRaftLogEntryPtr log = mLogMgr.getLog(rdata, getLastLogId());
	aos_assert_r(log, 0);

	return log->getTermId();
}

///////////////////////////////////////////////////
//   Peer methods
///////////////////////////////////////////////////
bool
AosRaftServer::getPeers(
		AosRundata* rdata, 
		const vector<AosCubeMapObj::CubeInfo> &peer_cubes)
{
	// 'peer_cubes' is a set of cubes that form a Raft Group. Each cube
	// is defined by CubeInfo. Among all these cubes, one belongs to 
	// this Raft (i.e., its physical ID equals to this server's physical
	// ID). Otherwise are the peers of this raft. 
	//
	//AosClusterObj* cluster = NULL;
	//AosClusterMgrObj* clusterMgr = NULL;
	vector<AosCubeMapObj::CubeInfo> cubeInfos;
	
	AosRaftPeer peer;
	mPeerMap.clear();
	//AosCubeMapObj::CubeInfo cube;
	for (u32 i = 0; i < peer_cubes.size(); i++)
	{
		//set server Id to be current endpont id
		AosCubeMapObj::CubeInfo	cube = peer_cubes[i];
		int phyid = cube.physical_id;
		int cubeid = cube.cube_id;
		aos_assert_rr(phyid >= 0, rdata, false);

		//peers not including self
		if (mServerId == (u32)phyid) continue;

		peer.setServerId(phyid); 
		peer.setCubeId(cubeid); 
		peer.setLastAgreed(0);
		mPeerMap[phyid] = peer;
	}

	RAFT_OmnScreen << "Get " << mPeerMap.size() << " peers. " << endl;
	return true;
}

AosRaftPeer*
AosRaftServer::getPeer(u32 peerId)
{
	RaftPeerMap::iterator itr;

	itr = mPeerMap.find(peerId);
	if (itr != mPeerMap.end())
	{
		if (itr->second.getServerId() == peerId)
		{
			return &(itr->second);
		}
		else
		{
			OmnString s = "peer map:\n";
			for(itr = mPeerMap.begin();itr != mPeerMap.end();itr++)
			{
				s << itr->second.toString() << "\n";
			}
			RAFT_OmnAlarm <<
				"there is something wrong with the peer map, dumping it...\n" << s << enderr;
		}
	}

	return NULL;
}

///////////////////////////////////////////////////
//   helper methods
///////////////////////////////////////////////////
OmnString
AosRaftServer::toString()
{
	OmnString str = "";

	str <<"(Timestamp=" << OmnGetTimestamp() 
		<<" CubeId=" << mCubeId
		<<" AseId=" << mAseId
	    <<" ServerId=" << mServerId
		<<" TermId=" << getCurTermId() 
		<<" LastLogId=" << getLastLogId()
		<<" LastLogIdApplied=" << getLastLogIdApplied()
		<<" CommitIndex=" << getCommitIndex()
		<<" Role=" << getRoleStr(mRole)
		<<") ";

	return str;
}

OmnString
AosRaftServer::getRoleStr(RaftRole role)
{
	OmnString str = "";
	switch (role)
	{
		case eLeader:
			str << "LEADER";
			break;

		case eCandidate:
			str << "CANDIDATE";
			break;

		case eFollower:
			str << "FOLLOWER";
			break;

		default:
			str << "INVALID";
			break;
	}

	return str;
}

