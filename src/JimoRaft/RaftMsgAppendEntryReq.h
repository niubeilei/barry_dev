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
#ifndef AOS_JimoRaft_RaftMsgAppendEntryReq_h
#define AOS_JimoRaft_RaftMsgAppendEntryReq_h

#include "JimoRaft/RaftLogEntry.h"
#include "JimoRaft/RaftMsg.h"
#include <vector>
using namespace std;

//class AosRaftServer;
class AosRaftMsgAppendEntryReq : public AosRaftMsg
{
private:
	u32					mPrevLogTermId;
	u64					mFirstLogId;
	//vector<AosRaftLogEntryPtr> *mLogList;
	vector<AosRaftLogEntryPtr> mLogList;
	AosBuffPtr			mBuff;

public:
	AosRaftMsgAppendEntryReq(
			RaftMsgType msgType,
			AosRaftServer *server,
			u64 firstLogId,
			u32 prevLogTermId);

	AosRaftMsgAppendEntryReq();
	~AosRaftMsgAppendEntryReq();

	bool serializeTo(
			AosRundata*  rdata,
			AosBuff *buff);

	bool serializeFrom(
			AosRundata*  rdata,
			AosBuff *buff);

	//getters and setters
	void addLog(AosRaftLogEntryPtr &log);
	AosRaftLogEntryPtr getLog(int i) { return mLogList[i]; }

	u64 getFirstLogId() { return mFirstLogId; }
	void setFirstLogId(u64 logId) { mFirstLogId = logId; }

	u32 getPrevLogTermId() { return mPrevLogTermId; }
	void setPrevLogTermId(u32 prevLogTermId) { mPrevLogTermId = prevLogTermId; }

	int	getNumLogs() { return mLogList.size(); }

	AosBuffPtr &getData() { return mBuff; }

	OmnString toString();
};


#endif
