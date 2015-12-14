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
#ifndef AOS_JimoRaft_RaftPeer_h
#define AOS_JimoRaft_RaftPeer_h

#include "Util/String.h"

class AosRaftPeer
{
private:
	u32	 mServerId;
	u32	 mCubeId;
	OmnString mIp;
	int	 mPort;

	//the biggest log Id replicated to the peer and confirmed
	u64  mMatchIndex;

	//next log Id to replicate to the peer
	u64  mNextIndex;

	//voted for me?
	bool mVotedForMe;

public:
	AosRaftPeer();
	~AosRaftPeer();

	//getter and setters
	u32 getServerId() { return mServerId; }
	void setServerId(u32 serverId) { mServerId = serverId;}

	u32 getCubeId() { return mCubeId; }
	void setCubeId(u32 cubeId) { mCubeId = cubeId;}

	bool getVotedForMe() { return mVotedForMe; }
	void setVotedForMe(bool flag) { mVotedForMe = flag;}

	u64	 getLastAgreed() {return mMatchIndex;}
	void setLastAgreed(u64 logId) { mMatchIndex = logId; }

	u64 getNextToReplicate() { return mNextIndex; }
	void setNextToReplicate(u64 logId) { mNextIndex = logId; }
	void decNextToReplicate(u64 logId) { mNextIndex--; }

	OmnString toString();	//by White, 2015-09-10 15:53:38
};


#endif
