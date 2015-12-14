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
#ifndef AOS_JimoRaft_RaftMsgVoteRsp_h
#define AOS_JimoRaft_RaftMsgVoteRsp_h

#include "JimoRaft/RaftMsg.h"

//class AosRaftServer;
class AosRaftMsgVoteRsp : public AosRaftMsg
{
private:
	bool				mResult;
	u32					mVotedFor;
	u32					mVotedForTermId;

public:
	AosRaftMsgVoteRsp(
			AosRaftServer *server, 
			bool result,
			u32 votedFor,
			u32 votedForTermId);

	AosRaftMsgVoteRsp();
	~AosRaftMsgVoteRsp();

	bool serializeTo(
			AosRundata*  rdata,
			AosBuff *buff);

	bool serializeFrom(
			AosRundata*  rdata,
			AosBuff *buff);

	//getters and setters
	u32 getVotedFor() { return mVotedFor; }
	u32 getVotedForTermId() { return mVotedForTermId; }
	void setVotedFor(u32 VotedFor) { mVotedFor = VotedFor; }

};


#endif
