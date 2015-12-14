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
#ifndef AOS_JimoRaft_RaftMsgVoteReq_h
#define AOS_JimoRaft_RaftMsgVoteReq_h

#include "JimoRaft/RaftMsg.h"

//class AosRaftServer;
class AosRaftMsgVoteReq : public AosRaftMsg
{
private:
	u32					mLastLogTerm;

public:
	AosRaftMsgVoteReq(AosRaftServer *server);

	AosRaftMsgVoteReq();
	~AosRaftMsgVoteReq();

	bool serializeTo(
			AosRundata*  rdata,
			AosBuff *buff);

	bool serializeFrom(
			AosRundata*  rdata,
			AosBuff *buff);

	//getters and setters
	u64 getLastLogId() { return mLastLogId; }
	void setLastLogId(u64 logId) { mLastLogId = logId; }

	u32 getLastLogTerm() { return mLastLogTerm; }
	void setLastLogTerm(u32 TermId) { mLastLogTerm = TermId; }

};


#endif
