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
#ifndef AOS_JimoRaft_RaftMsg_h
#define AOS_JimoRaft_RaftMsg_h

#include "Util/Buff.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "JimoRaft/RaftStateMachine.h"


class AosRaftServer;

class AosRaftMsg : virtual public OmnRCObject,
				   virtual public AosMemoryCheckerObj
{
	OmnDefineRCObject;

public:
	enum RaftMsgType
	{
		eMsgInvalid			= 0,
		eMsgAppendEntryReq	= 1,
		eMsgAppendEntryRsp	= 2,
		eMsgVoteReq			= 3,
		eMsgVoteRsp			= 4,
		eMsgHeartbeat 		= 5,
	};

protected:
	RaftMsgType	 mMsgType;
	u32			 mSenderId;
	u64			 mTimeTag; //sender set the timetag
	u64			 mReqTimeTag; //if the msg is a response to
							  //a request, this tag is the 
							  //request packet's timetag

	//used to track the server's current status
	int			 mRole;  //need to change mRole type in cpp
	u32			 mCurTermId;
	u64			 mLastLogId;
	u64			 mCommitIndex;
	u64			 mLastLogIdApplied;
	int			 mResult;  // 0: means successful

	AosRaftServer *mServer;

public:
	AosRaftMsg(
			RaftMsgType msgType,
			AosRaftServer *server AosMemoryCheckDecl);

	AosRaftMsg(AosMemoryCheckDeclBegin);
	~AosRaftMsg();

	virtual bool serializeTo(
			AosRundata* rdata,
			AosBuff *buff);

	virtual bool serializeFrom(
			AosRundata* rdata,
			AosBuff *buff);

	//getter and setters
	RaftMsgType getMsgType() { return mMsgType; }
	void setMsgType(RaftMsgType msgType) { mMsgType = msgType; }

	u32 getSenderId() { return mSenderId; }
	void setSenderId(u32 senderId) { mSenderId = senderId; }

	u32 getCurTermId() { return mCurTermId; }
	void setCurTermId(u32 termId) { mCurTermId = termId; }

	u64 getLastLogId() { return mLastLogId; }
	void setLastLogId(u64 LastLogId) { mLastLogId = LastLogId; }

	u64 getLastLogIdApplied() { return mLastLogIdApplied; }
	void setLastLogIdApplied(u64 LastLogIdApplied) { mLastLogIdApplied = LastLogIdApplied; }

	u64 getCommitIndex() { return mCommitIndex; }
	void setCommitIndex(u64 CommitIndex) { mCommitIndex = CommitIndex; }

	void setReqTimeTag(u64 timeTag) { mReqTimeTag = timeTag; }
	u64 getTimeTag() { return mTimeTag; }

	void setServer(AosRaftServer *server) { mServer = server; }
	//helper method
	OmnString getMsgStr();
	static OmnString getMsgStr(RaftMsgType msgType);

	virtual OmnString toString();
};


#endif
