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
// 2015/05/05 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/RaftLogMgr.h"

#include "StorageMgr/StorageMgr.h"
#include "JimoRaft/RaftServer.h"
#include "JimoRaft/RaftStateMachineSimple.h"
#include "Util/DirDesc.h"

AosRaftLogMgr::AosRaftLogMgr()
{
}

AosRaftLogMgr::~AosRaftLogMgr()
{
}

void
AosRaftLogMgr::init(
		AosRundata*				rdata,
		AosRaftServer*			server,
		AosRaftStateMachinePtr	stateMachine)
{
	bool rslt;

	mLogQueue.clear();
	mAppliedLogQueue.clear();

	//init member values
	mServer = server;
	mCurTermId = 0;
	mLastLogId = 0;
	mCommitIndex = 0;
	mLastLogIdApplied = 0;
	mLock = OmnNew OmnMutex();

	//init the applied log cache
	mAppliedLogCache.clear();
	mAppliedLogCacheStart = 0;
	
	aos_assert(stateMachine);
	mStatMach = stateMachine;

	if (RAFT_USE_STATMACH_SIMPLE)
	{
		loadRaftStatMach(rdata, 1, 12); 
	}

	mStatMachRaw = mStatMach.getPtrNoLock();
	mLogDir = mStatMachRaw->getBaseDir();
	aos_assert(!mLogDir.isEmpty());

	//create raft log sub-directory
	mLogDir << "/raft/";
	//Verify if the dir existing or not. If
	//not, we need to create the directory
	if (!OmnDirDesc::dirExist(mLogDir.data()))
	{
		rslt = OmnDirDesc::createDir(mLogDir.data());
		aos_assert(rslt);
	}

	mLogFileName << mLogDir << "raftlog";
	mLogFile = OmnNew OmnFile(mLogFileName, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!mLogFile->isGood())
	{
		RAFT_OmnScreen << "First time. Create a new raft log file "
			<< mLogFileName << endl;

		//must be a new file, create and open it
		mLogFile = OmnNew OmnFile(mLogFileName, OmnFile::eReadWriteCreate AosMemoryCheckerArgs);

		//call writeLogFile to save initial data
		aos_assert(mLogFile && mLogFile->isGood());

		//need to write initial data
		mLogFileRaw = mLogFile.getPtrNoLock();
		aos_assert(mLogFileRaw);
		writeLogFile(rdata);
	}

	//read log data to memory at the beginning
	rslt = readLogFile(rdata);
	aos_assert(rslt);
}

///////////////////////////////////////
//  Disk operations
///////////////////////////////////////

//Write the non-applied log entries
//
//Assume the log history data is not big and
//can be read to buff and save to disk
//at one time
//
//The caller need to take care of locking
//
//For the log data format, refer to readLogFile
bool
AosRaftLogMgr::writeLogFile(AosRundata* rdata)
{
	AosBuffPtr buff = OmnNew AosBuff(8 * 10000 AosMemoryCheckerArgs);

	//Nonapply log file contains the following
	//4 fields at the beginning
	buff->setU32(mCurTermId);
	buff->setU64(mLastLogId);
	buff->setU64(mLastLogIdApplied);
	buff->setU64(mCommitIndex);

	//Loop to write each log entry info including
	//     TermId: 4B (the log's term)
	//     StatMachHint: 24 bytes for now
	//
	//The log Id starts from the first non-applied
	//long entry
	AosRaftLogEntryPtr log;
	int total = mLogQueue.size();
	if (total > 0)
	{
		aos_assert_r(mLogQueue[0]->getLogId() 
				== mLastLogIdApplied + 1, false);

		aos_assert_r(mLogQueue[total - 1]->getLogId() 
				== mLastLogId, false);

		for (u32 i = 0; i < mLogQueue.size(); i++)
		{
			log = mLogQueue[i];
			aos_assert_r(log, false);

			buff->setU32(log->getTermId());
			buff->setBuff(log->getStatMachHint());
		}
	}

	//save all the data to disk
	//assume log history is not big
	aos_assert_r(mLogFileRaw, false);
	bool rslt = mLogFileRaw->write(0,
			buff->data(), buff->dataLen(), true);
	aos_assert_r(rslt, false);

	if (mLastLogId % RAFT_SAMPLE_PRINT == 0)
	{
		RAFT_OmnScreen << "Write log file " 
			<< mLogFileName << " with lastLogId="
			<< mLastLogId << " (every "  
			<< RAFT_SAMPLE_PRINT << ")" << endl;
	}

	return true;
}

//
//Read non-applied log entries + header information
//Log file is in the following format
//    3 50 45 47
//    1 24Bytes hint
//    1 24Bytes hint
//    1 24Bytes hint
//    2 24Bytes hint
//    2 24Bytes hint
//
//It means:
//The first line:
//   1. current term is 5
//   2. last log id is 50
//   3. last applied log id is 45
//   4. last commit index is 47
// 
//Rest lines (each line one non-applied log):
//   LogId  TermId   StatMachHint
//   46     1        hint
//   47     1        hint
//   48     1        hint
//   49     2        hint
//   50     2        hint
//
//For above data, logid is calculated, but not saved:
//   The first non-applied log Id = mLastLogIdApplied + 1
//   The last non-applied log Id = mLastLogId
//
bool
AosRaftLogMgr::readLogFile(AosRundata*	rdata)
{
	//non-applied log should not be too many
	//Therefore the buff needs not be very big
	u32 buffSize = 8*10000;
	AosBuffPtr buff = OmnNew AosBuff(buffSize AosMemoryCheckerArgs);

	//read all the data to memory at once
	//assume log history is not big
	u64 fileSize = OmnFile::getFileLengthStatic(mLogFileName);
	aos_assert_r(fileSize < buffSize, false); 
	bool rslt = mLogFileRaw->readToBuff(0, fileSize, buff->data());

	aos_assert_rr(rslt, rdata, false);
	buff->setDataLen(fileSize);
	buff->reset();

	//set log entry data based on buff data
	//the first 8 bytes is last applied
	//log Id
	mCurTermId = buff->getU32(0);
	mLastLogId = buff->getU64(0);
	mLastLogIdApplied = buff->getU64(0);
	mCommitIndex = buff->getU64(0);
		
	//get the log file name
	RAFT_OmnScreen << "Read log file: "
		<< mLogFileName << endl;

	//get total number of terms
	RAFT_OmnScreen << "Current data in log history file is: "
		<< "TermId=" << mCurTermId 
		<< ", logId=" << mLastLogId
		<< ", logIdApplied=" << mLastLogIdApplied  
		<< ", commitIndex=" << mCommitIndex << endl;

	AosRaftLogEntryPtr log;
	u32 termId;
	AosBuffPtr hint;
	i64 hintSize = RAFT_MAX_STATMACH_HINT_SIZE;
	for (u32 i = mLastLogIdApplied + 1; i <= mLastLogId; i++)
	{
		termId = buff->getU32(0);
		aos_assert_r(termId, false);
		hint = buff->getBuff(hintSize,true AosMemoryCheckerArgs);

		log = OmnNew AosRaftLogEntry(i, termId AosMemoryCheckerArgs);
		RAFT_LOG_TAG(log);
		log->setStatMachHint(hint);
		mLogQueue.push_back(log);
#if 0
		if (mLogQueue.size() > 100)
		{
			RAFT_OmnScreen << "Too many logs in mLogQueue: "
				<< mLogQueue.size() << endl;
		}
#endif
	}

	return true;
}

///////////////////////////////////////////
//    log methods
///////////////////////////////////////////
bool
AosRaftLogMgr::removeLogs(
		AosRundata *rdata,
		u64 startLogId)
{
	aos_assert_r(startLogId <= mLastLogId, false);
	aos_assert_r(startLogId > mCommitIndex, false);

	RAFT_OmnScreen << "Remove conflict logs from logId: "
		<< startLogId << endl;

	bool rslt;
	AosRaftLogEntryPtr log;

	mLock->lock();
	while (mLastLogId >= startLogId)
	{
		log = mLogQueue.back();

		//remove from blobSE active log
		rslt = mStatMachRaw->removeEntry(rdata, 
				mLastLogId, log->getStatMachHint());
		if (!rslt)
		{
			RAFT_OmnAlarm << "Failed to remove log entry from statmachine:"
				<< mLastLogId << enderr;

			mLock->unlock();
			return false;
		}

		//remove the log from log queue
		mLogQueue.pop_back();
		mLastLogId--;
	}

	mLock->unlock();
	return true;
}

//return the new log entry
bool
AosRaftLogMgr::appendLog(
		AosRundata* rdata,
		AosRaftLogEntryPtr &log)
{
	aos_assert_r(log, false);
	mLock->lock();

	//If a lot of non-applied log entries, 
	//refuse to append the log
	if (mLastLogId - mLastLogIdApplied > RAFT_LOG_QUEUE_SIZE)
	{
		RAFT_OmnAlarm << "Failed to append logId:"
			<< mLastLogId + 1 << " because too many non-applied logs." << enderr;
		mLock->unlock();
		return false;
	}

	//call RaftStateMachine method to appenEntry
	u64 logId = log->getLogId();
	if (!logId) 
	{
		logId = mLastLogId + 1;
		log->setLogId(logId);
	}

	u32 termId = log->getTermId();
	if (!termId) 
	{
		termId = mCurTermId;
		log->setTermId(termId);
	}

	AosBuffPtr hint = mStatMachRaw->appendEntry(
			rdata, logId, termId, log->getData(rdata, mStatMachRaw));
	if (!hint) 
	{
		RAFT_OmnAlarm << "Failed to append logId: "
			<< log->getLogId() << enderr;
		mLock->unlock();
		return false;
	}

	log->setStatMachHint(hint);

	//update counters
	mLastLogId++;
	if (logId != mLastLogId)
	{
		RAFT_OmnAlarm << "Append log with wrong log id: " <<
			"logId:" << logId << " mLastLogId:" << mLastLogId << enderr;

		return false;
	}

	//update log queue
	mLogQueue.push_back(log);
#if 0
	if (mLogQueue.size() > 100)
	{
		RAFT_OmnScreen << "Too many logs in mLogQueue: "
			<< mLogQueue.size() << endl;
	}
#endif
	
	//
	//StateMachine will save appended data in real
	//time, Raft will not write in real time
	//
	//Raft will save the data when the data is applied
	//writeLogFile(rdata);
	mLock->unlock();
	return true;
}

//
//This is normally called by the leader
//to append new data from a client
//
bool
AosRaftLogMgr::appendLog(
		AosRundata* rdata,
		AosBuffPtr &buff)
{
	//build the log entry
	AosRaftLogEntryPtr log;
	log = OmnNew AosRaftLogEntry(mLastLogId + 1, mCurTermId AosMemoryCheckerArgs);
	RAFT_LOG_TAG(log);

	log->setData(buff);
	return appendLog(rdata, log);
}

AosRaftLogEntryPtr
AosRaftLogMgr::getLog(
		AosRundata*	rdata,
		u64	logId)
{
	mLock->lock();
	if (0 == logId || logId > mLastLogId)
	{
		//For a new entry, the log Id is Ok to be non-existing
		//RAFT_OmnAlarm << "Requested log id out of range: " 
		//	<< logId << enderr;
		mLock->unlock();
		return NULL;
	}

	AosRaftLogEntryPtr log = NULL;
	if (logId > mLastLogIdApplied)
	{
		//the log should be in log queue already
		int pos;
		pos = logId - mLastLogIdApplied - 1;
		log = mLogQueue[pos];
	}
	else
	{
		log = getAppliedLog(rdata, logId);
	}

	mLock->unlock();
	return log;
}

bool
AosRaftLogMgr::notifyJimoCall(AosRundata *rdata)
{
	aos_assert_r(mServer, false);

	//1. Get jimocall from the logs in the
	//log queue which are not applied yet
	//
	//2. Send false result to the jimocall clients and 
	//clear the jimocalls
	AosRaftLogEntryPtr log;
	for (u32 i = 0; i < mLogQueue.size(); i++)
	{
		log = mLogQueue[i];
		aos_assert_r(log, false);

		log->notifyJimoCall(rdata, mServer->getLeaderId());
	}

	return true;
}

bool
AosRaftLogMgr::setLastLogIdApplied(AosRundata *rdata, u64 logId)
{ 
	mLock->lock();
	setLastLogIdAppliedNoLock(rdata, logId);
	mLock->unlock();

	return true;
}

bool 
AosRaftLogMgr::setLastLogIdAppliedNoLock(AosRundata *rdata, u64 logId)
{ 
	bool rslt;
	mLastLogIdApplied = logId; 
	
	//remove applied log from log queue
	AosRaftLogEntryPtr log;
	u64 localLogId;
	AosBuffPtr hint;
	AosBuffPtr buff;
	while (!mLogQueue.empty())
	{
		log = mLogQueue[0];
		localLogId = log->getLogId();	
		if (localLogId > logId)
			break;

		//If no applied log file or the current one is 
		//full, create a new one
		if (localLogId % RAFT_LOG_PER_FILE == 1)
		{
			rslt = createAppliedLogFile(rdata, localLogId);
			aos_assert_r(rslt, false);
		}

		//Append the log entry to the applied log
		//history files
		aos_assert_r(mCurAppliedLogFileRaw, false);
		hint = log->getStatMachHint();

		//consists of a termId and a hint
		buff = OmnNew AosBuff(sizeof(AppliedLogFileEntry) AosMemoryCheckerArgs);
		buff->setU32(log->getTermId());
		buff->setChar(0x0);  //default is not compacted
		buff->setBuff(hint);
		bool rslt = mCurAppliedLogFileRaw->
			append(buff->data(), buff->dataLen(), true);
		aos_assert_r(rslt, false);

		//remove the applied log from log queue
		mLogQueue.pop_front();
		mAppliedLogQueue.push_back(log);
#if 0
		if (mAppliedLogQueue.size() > 100)
		{
			RAFT_OmnScreen << "Too many logs in mAppliedLogQueue: "
				<< mAppliedLogQueue.size() << endl;
		}
#endif

		if (mAppliedLogQueue.size() > RAFT_APPLIED_LOG_QUEUE_SIZE)
			mAppliedLogQueue.pop_front();
	}

	return true;
}

//
//Get an log history file based on a logId
//If the file doesn't exist, create a new one
//
OmnString
AosRaftLogMgr::getAppliedLogFileName(u64 logId)
{
	aos_assert_r(logId, 0);

	//Get the file Id
	OmnString name = mLogDir;
	int fileId = (logId - 1)/RAFT_LOG_PER_FILE;
	OmnString prefix = "raftlog_";
	name << prefix << fileId;

	return name;
}

//
//Create a log history file to hold the log Id 
//starting from logId
//
bool
AosRaftLogMgr::createAppliedLogFile(
		AosRundata *rdata,
		u64 logId)
{
	bool rslt;
	mCurAppliedLogFileName = getAppliedLogFileName(logId);

	RAFT_OmnScreen << "Create an applied log file:" << mCurAppliedLogFileName << endl;
	mCurAppliedLogFile = OmnNew OmnFile(mCurAppliedLogFileName, OmnFile::eReadWriteCreate AosMemoryCheckerArgs);
	
	aos_assert_r(mCurAppliedLogFile, false);
	mCurAppliedLogFileRaw = mCurAppliedLogFile.getPtrNoLock();
	aos_assert_r(mCurAppliedLogFileRaw, false);
	
	int totalCompacted = 0;
	rslt = mCurAppliedLogFileRaw->
		append((char*)&totalCompacted, sizeof(totalCompacted), true);
	aos_assert_r(rslt, false);
	return true;
}

//
//1. Get applied log from applied log queue
//2. If not in the queue, get applied log 
//   from the applied log cache
//3. If not in applied log cache, get it from log
//   history file
//
//Entering this method:
//1. logId should be in the valid range
//2. the caller need to take care of locking issue
//
AosRaftLogEntryPtr
AosRaftLogMgr::getAppliedLog(AosRundata *rdata, u64 logId)
{
	//get the log from the applied log queue firstly
	int count = mAppliedLogQueue.size();
	AosRaftLogEntryPtr log;
	if (count > 0)
	{
		log = mAppliedLogQueue[0];

		int pos = logId - log->getLogId();
		if (pos >= 0) return mAppliedLogQueue[pos];
	}

	//get from applied log cache
	if (!mAppliedLogCacheStart ||
		logId < mAppliedLogCacheStart || 
		logId >= mAppliedLogCacheStart + mAppliedLogCache.size())
	{
		//Cache miss, refresh the cache from the log
		//history file firstly, first clear cache
		mAppliedLogCache.clear();

		//Since the prevLog term is also needed, need to
		//get the log from logId - 1
		//if (logId > 1) startLogId = logId - 1;

		OmnString name = getAppliedLogFileName(logId);
		RAFT_OmnScreen << "Get log from applied log file:" << name << endl;

		OmnFilePtr logFile;
		logFile = OmnNew OmnFile(name, OmnFile::eReadOnly AosMemoryCheckerArgs);
		aos_assert_r(logFile, NULL);

		//Read data from the file to cache
		//Decide read number and reading offset
		//Each history log entry contains:
		//   TermId: 4 bytes
		//   StatMach_Hint: 24 bytes
		//
		u64 startLogId = logId;
		int entrySize = sizeof(AppliedLogFileEntry);
		int logOffset = (startLogId - 1) % RAFT_LOG_PER_FILE;
		int byteOffset = sizeof(AppliedLogFileHeader) + logOffset * entrySize;

		int readLogNum = RAFT_LOG_PER_FILE - logOffset;
		if (readLogNum > RAFT_APPLIED_LOG_CACHE_SIZE)
			readLogNum = RAFT_APPLIED_LOG_CACHE_SIZE; 

		if (readLogNum > (mLastLogIdApplied - startLogId + 1))
			readLogNum = mLastLogIdApplied - startLogId + 1;

		//Read all the data to a buff firstly
		int readSize = readLogNum * entrySize;
		AosBuffPtr buff = OmnNew AosBuff(readSize AosMemoryCheckerArgs);

		//Read a group of history log data
		bool rslt = logFile->readToBuff(
				byteOffset, readSize, buff->data());

		aos_assert_rr(rslt, rdata, NULL);
		buff->setDataLen(readSize);
		buff->reset();

		//Read history log entry from the log file 
		//to cache one by one
		u32 termId;
		AosBuffPtr hint;
		char compacted;
		int hintSize = RAFT_MAX_STATMACH_HINT_SIZE;
		for (int i = 0; i < readLogNum; i++)
		{
			termId = buff->getU32(0);
			aos_assert_r(termId, NULL);

			compacted = buff->getChar(0);
			hint = buff->getBuff(hintSize, true AosMemoryCheckerArgs);

			log = OmnNew AosRaftLogEntry(startLogId + i, termId AosMemoryCheckerArgs);
			RAFT_LOG_TAG(log);
			log->setStatMachHint(hint);
			mAppliedLogCache.push_back(log);
#if 0
			if (mAppliedLogCache.size() > 100)
			{
				RAFT_OmnScreen << "Too many logs in mAppliedLogCache: "
					<< mAppliedLogCache.size() << endl;
			}
#endif
		}

		mAppliedLogCacheStart = startLogId;
		RAFT_OmnScreen << "Get Applied log from disk: startLogId=" 
			<< mAppliedLogCacheStart << " totalCachedLog=" << readLogNum << endl;
	}
	
	//RAFT_OmnScreen << "Get Applied log" << endl;
	//Return the cache value
	return mAppliedLogCache[logId - mAppliedLogCacheStart];
}

bool
AosRaftLogMgr::loadRaftStatMach(
		AosRundata*	rdata,
		const int			version,
		const u32			header_custom_data_size)
{
	//AosJimoPtr jimo = AosCreateJimoByClassname(rdata, className, version);
	//aos_assert_r(jimo, false);

	//mStatMach = dynamic_cast<AosRaftStateMachine *>(jimo.getPtrNoLock());
	//For debugging
	mStatMach = OmnNew AosRaftStateMachineSimple();
	aos_assert_r(mStatMach, false);
	mStatMachRaw = mStatMach.getPtrNoLock();
	if (!mStatMachRaw->init(rdata, mLastLogIdApplied))
	{
		RAFT_OmnAlarm << 
			"mStatMach->init failed, mLastLogIdApplied:" << mLastLogIdApplied << enderr;

		return false;
	}

	return true;
}

OmnString 
AosRaftLogMgr::toString() 
{ 
	aos_assert_r(mServer, "");
	return mServer->toString(); 
}
