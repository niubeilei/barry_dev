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
#ifndef AOS_JimoRaft_RaftMsgAppendEntryRsp_h
#define AOS_JimoRaft_RaftMsgAppendEntryRsp_h

#include "JimoRaft/RaftLogEntry.h"
#include "JimoRaft/RaftMsg.h"

//class AosRaftServer;
class AosRaftMsgAppendEntryRsp : public AosRaftMsg
{
private:
	bool				mResult;

public:
	AosRaftMsgAppendEntryRsp(
			AosRaftServer *server,
			bool result);

	AosRaftMsgAppendEntryRsp();
	~AosRaftMsgAppendEntryRsp();

	bool serializeTo(
			AosRundata*  rdata,
			AosBuff *buff);

	bool serializeFrom(
			AosRundata*  rdata,
			AosBuff *buff);

	//getters and setters
	//u64 getLogId() { return mLogId; }
	//void setLogId(u64 logId) { mLogId = logId; }

	bool getResult() { return mResult; }
	void setResult(bool result) { mResult = result; }
};


#endif
