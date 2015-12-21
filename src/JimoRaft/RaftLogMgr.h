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
#ifndef AOS_JimoRaft_RaftLogMgr_h
#define AOS_JimoRaft_RaftLogMgr_h

#include "BlobSE/Ptrs.h"
#include "JimoRaft/RaftLogEntry.h"
#include "JimoRaft/RaftStateMachine.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"

#include <hash_map>
#include <deque>
using namespace std;

#define RAFT_LOG_FILEID_START  50		

//If a server has a lot of non-applied logs
//it will refuse to accept new ones
//
//If a leader accumulate a big number of 
//non-applied logs, it may realize that the
//cluster might have less than 1/2 nodes. Then
//it will yield leadership to be candidate
//
//All the non applied log entries will be saved
//in the log queue
#define RAFT_LOG_QUEUE_SIZE 1000
#define RAFT_APPLIED_LOG_QUEUE_SIZE 1000

//Every time, we read into this number of history
//log entries from the applied log history file
//
//This cache is mostly used for leader to bring
//a slow follower up-to-date
#define RAFT_APPLIED_LOG_CACHE_SIZE RAFT_MAX_LOG_IN_MSG + 1

//Applied log will be saved to a big file which
//hold about 2M log entries per file
//#define RAFT_LOG_PER_FILE	  2000000
#define RAFT_LOG_PER_FILE	  20000000

class AosRaftServer;

/////////////////////////////////////////////
//structs for applied raft log file
/////////////////////////////////////////////
typedef struct
{
	//how many log entries compacted, 0 at beginning
	u32 numCompacted;  
} AppliedLogFileHeader;

#pragma pack(1)
typedef struct
{
	u32 termId;  //log entry's termId
	char compacted;
	char hint[RAFT_MAX_STATMACH_HINT_SIZE];
} AppliedLogFileEntry;
#pragma pack()

/////////////////////////////////////////////
//structs for non-applied raft log file
/////////////////////////////////////////////
typedef struct
{
	u32 curTermId;
	u64 lastLogId;
	u64 lastLogIdApplied;
	u64 lastCommitIndex;
} LogFileHeader;

#pragma pack(1)
typedef struct
{
	u32 termId;  //log entry's termId
	char hint[RAFT_MAX_STATMACH_HINT_SIZE];
} LogFileEntry;
#pragma pack()

/////////////////////////////////////////////
// RaftLogMgr class
/////////////////////////////////////////////
class AosRaftLogMgr : public OmnRCObject
{
	OmnDefineRCObject;

public:
	typedef hash_map<u64, AosRaftLogEntryPtr, u64_hash, u64_cmp> RaftLogMap;
	
private:
	//raft data relevant
	OmnMutexPtr					mLock;
	AosRaftServer*				mServer;
	AosRaftStateMachinePtr		mStatMach;
	AosRaftStateMachine*		mStatMachRaw;

	u32							mCurTermId;
	u64 						mLastLogId;
	u64 						mLastLogIdApplied;
	u64 						mCommitIndex;

	OmnString					mLogDir;
	//
	//Log file for non-applied log entries 
	//The file name is:
	//  raftlog_<cube_id>_nonapplied
	//
	OmnString					mLogFileName;
	OmnFilePtr					mLogFile;
	OmnFile*					mLogFileRaw;

	//
	//For non-applied log entries 
	//
	//Log entries in this queue will be saved into
	//the non applied log file
	//
	deque<AosRaftLogEntryPtr>	mLogQueue; 

	//
	//For caching nearest applied log entries:
	//1. These log entries are latest group of log entries
	//2. A little slow follower need these old logs to 
	//sync up
	//
	deque<AosRaftLogEntryPtr>	mAppliedLogQueue; 

	//Log history files for applied log entries. They 
	//will be save into a file named as:
	//  raftlog_<cube_id>_<file_id>
	//  raftlog_0_0:  save log entries from 1~2M
	//  raftlog_0_1:  save log entries from 2M+1~4M
	//  ..............................
	//  Each log entry consists of the following fields:
	//     TermId: 4 Bytes
	//     Compaction flag: 1 Byte
	//     Log cache opr data: 24 Bytes
	//
	//  At the beginning of the file, there is a number:
	//     CompactedLogs: 4Bytes
	OmnString 					mCurAppliedLogFileName;
	OmnFilePtr					mCurAppliedLogFile;
	OmnFile*					mCurAppliedLogFileRaw;

	//
	//For caching applied log entries:
	//1. These log entries normally are pretty old logs
	//2. A quite slow follower or rebooted node reqeust
	//   these old logs to sync up
	//
	vector<AosRaftLogEntryPtr>	mAppliedLogCache; 
	
	//The log Id of the first cached history log
	//0 means not log in the cache
	u64							mAppliedLogCacheStart;

public:
	AosRaftLogMgr();
	~AosRaftLogMgr();

	void init(AosRundata*				rdata,
			  AosRaftServer*			server,
		 	  AosRaftStateMachinePtr	stateMachine);

	//raft protocol methods
	bool removeLogs(
			AosRundata *rdata,
			u64 startLogId);

	bool appendLog(
			AosRundata* rdata,
			AosRaftLogEntryPtr &log);

	bool appendLog(
			AosRundata* rdata,
			AosBuffPtr &buff);

	AosRaftLogEntryPtr getLog(
			AosRundata* 	rdata,
			const u64			logId);

	bool notifyJimoCall(AosRundata *rdata);

	//disk operations
	bool writeLogFile(AosRundata* rdata);
	bool readLogFile(AosRundata* rdata);

	//getters and setters
	void setServer(AosRaftServer *server) { mServer = server;} 
	u32  getCurTermId() { return mCurTermId; }
	void setCurTermId(u32 termId) { mCurTermId = termId; }
	void incCurTermId() { setCurTermId(mCurTermId + 1); }

	u64 getLastLogId() { return mLastLogId; }
	AosRaftLogEntryPtr getLastLog(AosRundata* rdata) { return getLog(rdata, mLastLogId); }

	u64 getLastLogIdApplied() { return mLastLogIdApplied; }
	bool setLastLogIdApplied(AosRundata* rdata, u64 logId);
	bool setLastLogIdAppliedNoLock(AosRundata* rdata, u64 logId);

	u64 getCommitIndex() { return mCommitIndex; }
	void setCommitIndex(u64 logId) { mCommitIndex = logId; }

	OmnString toString();

	bool loadRaftStatMach(
			AosRundata* 	rdata,
			const int			version,
			const u32			header_custom_data_size);

	AosRaftStateMachine *getStatMach() { return mStatMachRaw; }

	//methods to handle applied log history data
	OmnString getAppliedLogFileName(u64 logId);
	AosRaftLogEntryPtr getAppliedLog(AosRundata *rdata, u64 logId);
	bool createAppliedLogFile(AosRundata *rdata, u64 logId);

};

#endif 
