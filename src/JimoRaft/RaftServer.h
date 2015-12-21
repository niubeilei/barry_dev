////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/05/01 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoRaft_RaftServer_h
#define AOS_JimoRaft_RaftServer_h

//#include "Rundata/Ptrs.h"
#include "JimoRaft/RaftLogMgr.h"
#include "JimoRaft/RaftPeer.h"
#include "JimoRaft/RaftMsg.h"
#include "JimoRaft/RaftMsgAppendEntryReq.h"
#include "JimoRaft/RaftMsgAppendEntryRsp.h"
#include "JimoRaft/RaftMsgVoteReq.h"
#include "JimoRaft/RaftMsgVoteRsp.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "UtilData/ModuleId.h"
#include "Thread/Sem.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Porting/Sleep.h"
#include "JimoAPI/JimoRaftAPI.h"
#include "SEInterfaces/CubeMapObj.h"

#include <map>
#include <deque>
using namespace std;

//define some constants
#define RAFT_MAX_LOG_IN_MSG 	1000
#define RAFT_DEBUG 	false
#define RAFT_USE_STATMACH_SIMPLE false 

#define RAFT_TIME_SCALE		  	15	
#define RAFT_ChangeRole(a)		changeRole((a), __FILE__,  __LINE__)

#define RAFT_LOG_TAG(log)		log->setTag(__FILE__, __LINE__)

#define RAFT_OmnScreen	OmnScreen << "[JimoRaft]" << toString()
#define RAFT_OmnAlarm	OmnAlarm << "[JimoRaft]"  << toString()
#define RAFT_SAMPLE_PRINT	500  //sample print once per 500

class AosJimoCall;
class AosRaftServer : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	typedef map<u32, AosRaftPeer> RaftPeerMap;
	//typedef hash_map<u64, AosJimoCallPtr, u64_hash, u64_cmp> JimoCallMap;

	enum RaftTimer
	{
		eMinElectionTimeout = 150 * RAFT_TIME_SCALE,	// ms
		eMaxElectionTimeout = 300 * RAFT_TIME_SCALE,	// ms
		eHeartbeatInterval = 50 * RAFT_TIME_SCALE,	// ms
		eMaxMsgDelay = 100 * RAFT_TIME_SCALE //ms
	};

	enum RaftRole
	{
		eInvalid,
        eFollower,
        eCandidate,
        eLeader
    };
 	
protected:
	///////////////////////////////////////
	//     Basic members
	///////////////////////////////////////
	RaftRole 			mRole;
	u32 				mServerId;
	u32 				mLeaderId;
	AosRundataPtr		mRundata;
	AosRundata*			mRundataRaw;	

	//For each voting round, a server can only
	//vote once, either itself or another server
	int					mVotedFor;
	u32					mVotedForTermId;

	//log and peer info
	AosRaftLogMgr		mLogMgr;
	RaftPeerMap 		mPeerMap;

	//DocFileMgr relevant info
	u64 				mJPId;
	u64					mCubeId;
	u64					mAseId;	

	///////////////////////////////////////
	//     Thread relevant members
	///////////////////////////////////////
	//main thread
	OmnThreadPtr		mMainThread;
	OmnMutexPtr			mLock;
	OmnSemPtr			mSem;

	//message thread relevant
	OmnThreadPtr		mMsgThread;
	OmnMutexPtr			mMsgLock;
	OmnSemPtr			mMsgSem;
	deque<AosRaftMsgPtr>	mMsgQueue;

	//client data thread relevant
	OmnThreadPtr		mDataThread;
	OmnMutexPtr			mDataLock;
	OmnSemPtr			mDataSem;
	deque<AosRaftLogEntryPtr>	mDataQueue;

	//////////////////////////////////////////
	//  leader members
	//////////////////////////////////////////
	int					mHeartBeatSent;
	//JimoCallMap 		mCallMap;

	//////////////////////////////////////////
	//  Follower members
	//////////////////////////////////////////
	u32		mHeartBeatRecved;
	//last time to get a log from the leader successfully
	u64		mLastLogTime;

	//////////////////////////////////////////
	//  Candidate members
	//////////////////////////////////////////
	u32	  	mNumVotedForMe;

public:
	/////////////////////////////////
	//constructor & destructor
	/////////////////////////////////
	AosRaftServer();
	AosRaftServer(
		AosRundata								*rdata,
		int										cubeId,
		const vector<AosCubeMapObj::CubeInfo>	&peer_cubes,
		AosRaftStateMachinePtr					stateMachine);

	~AosRaftServer();

	virtual void init();
	
	/////////////////////////////////
	//thread relevant methods
	/////////////////////////////////
	virtual bool threadFunc(
			OmnThrdStatus::E &state, 
			const OmnThreadPtr &thread); 

	virtual bool checkThread(
			OmnString &err, 
			const int thrdLogicId) const
	{ return true;}
	
	virtual bool signal(const int threadLogicId)
	{ return true; }

	virtual bool mainThreadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread);

	virtual bool msgThreadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread);

	virtual bool start();
	virtual bool stop();
	virtual bool notify();

	void clearMessages();
	void clearClientData();

	/////////////////////////////////
	//message relevant methods
	/////////////////////////////////
	//networking methods
	//common methods
	virtual bool sendMsg(
			AosRundata* rdata,
			AosRaftMsg *msg,
			u32 recvId);

	virtual bool sendMsg(
			AosRundata* rdata,
			AosRaftMsg *msg,
			AosRaftPeer *peer);

	virtual bool sendMsg(
				AosRundata* rdata,
				AosRaftMsg *msg,
				u32 recvId,
				AosBuffPtr &buff);

	virtual AosBuffPtr buildMsg(
				AosRundata* rdata,
				AosRaftMsg *msg,
				AosBuffPtr buff);

	virtual bool broadcastMsg(
			AosRundata* rdata,
			AosRaftMsg *msg);

	virtual bool recvMsg(
			AosRundata* rdata,
			AosBuff *buff);

	bool handleMsg(
				AosRundata* rdata,
				AosRaftMsg *msg);

	//common individule message methods
	virtual bool handleVoteReq(
			AosRundata* rdata,
			AosRaftMsgVoteReq *msg);

	virtual bool sendVoteRsp(
			AosRundata* rdata,
			AosRaftMsg *msg,
			bool result);

	virtual bool sendAppendEntryRsp(
			AosRundata* rdata,
			AosRaftMsg *msg,
			bool result);

	///////////////////////////////////////////
	//  Raft protocol methods
	///////////////////////////////////////////
	virtual bool changeRole(
			RaftRole role,
			OmnString file,
			int line);

	void stepDown(const u32 ulNewTermID);
	bool isLeader() { return mRole == eLeader; }

	u32 getLastLogTerm(AosRundata* rdata);
	int cmpLogUpToDateWithMe(
			AosRundata* rdata,
			AosRaftMsg *msg);

	bool applyCommittedLogs(AosRundata* rdata);

	OmnString toString();
	OmnString getRoleStr() { return getRoleStr(mRole); }
	static OmnString getRoleStr(RaftRole role);

	///////////////////////////////////////
	//  peer relevant methods
	////////////////////////////////////////
	bool getPeers(AosRundata* rdata, 
				  const vector<AosCubeMapObj::CubeInfo> &peer_cubes);
	AosRaftPeer* getPeer(u32 peerId);
	int getTotalServers() { return mPeerMap.size() + 1; }

	//////////////////////////////
	// Getters and setters
	//////////////////////////////
	RaftRole getRole() { return mRole; }

	u32 getServerId() { return mServerId; }
	void setServerId(u32 ServerId) { mServerId = ServerId; }

	u32 getLeaderId() { return mLeaderId; }
	void setLeaderId(u32 leaderId) { mLeaderId = leaderId; }

	u64 getLastLogId() { return mLogMgr.getLastLogId(); }
	u64 getLastLogIdApplied() { return mLogMgr.getLastLogIdApplied(); }
	u64 getCommitIndex() { return mLogMgr.getCommitIndex(); }
	u32 getCurTermId() { return mLogMgr.getCurTermId(); }

	AosRundata* getRundata() { return mRundataRaw; }

	u64 getAseId() { return mAseId; }
	void setAseId(u64 aseId) { mAseId = aseId; }

	u64 getCubeId() { return mCubeId; }
	void setCubeId(u64 cubeId) { mCubeId = cubeId; }

	u64 getJPId() { return mJPId; }
	void setJPId(u64 jpId) { mJPId = jpId; }

	AosRaftStateMachine *getStatMach() { return mLogMgr.getStatMach(); }

	////////////////////////////////////////////////////////
	//   Leader methods
	////////////////////////////////////////////////////////
	//thread methods
	void leaderInit();
	bool leaderMain();
	bool dataThreadFunc(
			OmnThrdStatus::E &state,
			const OmnThreadPtr &thread);

	//client data handling method
	bool handleClientData(
			AosRundata *rdata,
			AosJimoCall &call,
			AosBuffPtr &buff);

	//networking methods
	bool sendAppendEntryReq(
			AosRundata* rdata,
			u32 recvId,
			u64 logId);

	bool broadcastAppendEntryReq(
			AosRundata* rdata,
			AosRaftLogEntryPtr &log);

	bool broadcastHeartbeat(AosRundata* rdata);

	bool leaderHandleMsg(
				AosRundata* rdata,
				AosRaftMsg *msg);

	bool leaderHandleAppendEntryReq(
			AosRundata* rdata,
			AosRaftMsgAppendEntryReq *msg);

	bool leaderHandleAppendEntryRsp(
				AosRundata* rdata,
				AosRaftMsgAppendEntryRsp *msg);

	bool leaderHandleVoteReq(
			AosRundata* rdata,
			AosRaftMsgVoteReq *msg);

	////////////////////////////////////////////////////////
	//   Follower methods
	////////////////////////////////////////////////////////
	void followerInit();
	bool followerMain();

	bool followerHandleMsg(
				AosRundata* rdata,
				AosRaftMsg *msg);

	bool followerHandleAppendEntryReq(
			AosRundata* rdata,
			AosRaftMsgAppendEntryReq *msg);

	////////////////////////////////////////////////////////
	//   Candidate methods
	////////////////////////////////////////////////////////
	void candidateInit();
	bool candidateMain();

	bool broadcastVoteReq(AosRundata* rdata);

	bool candidateHandleMsg(
				AosRundata* rdata,
				AosRaftMsg *msg);

	bool candidateHandleAppendEntryReq(
			AosRundata* rdata,
			AosRaftMsgAppendEntryReq *msg);

	bool candidateHandleVoteRsp(
			AosRundata* rdata,
			AosRaftMsgVoteRsp *msg);

};


#endif


