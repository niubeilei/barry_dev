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
// An IIL Log File contains a collection of IIL logs. Each IIL log is 
// an IIL operation. IIL Logs are linked backwards based on IILID. 
//
// Processing an IIL 
// When processing an IIL, it retrieves the last IIL log position. 
// It collects all the IIL logs into an IIL Log Buff. After that, 
// it processes all the IIL logs for the IIL. It then saves the 
// IIL. It then adds an Update Log Entry into the Update Logs. The
// Update Logs are used to determine which entries are processed. 
// Under normal conditions, the Update Logs are not used. If system
// crashes, it can use the Update Log to rebuild the map, determining
// which log entries were processed. 
//
// Modification History:
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILClient/IILLogFile.h"

#include "Util/File.h"
#include "Util/Buff.h"
#include "IILMgr/IIL.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILProc.h"
#include "IILClient/IILLogBuff.h"

#include <dirent.h>
#include <sys/types.h>

const OmnString sgFlagComplete = "Complete";
const OmnString sgFlagActive = "Active";

AosIILLogFile::AosIILLogFile(
		const OmnString &dirname, 
		const OmnString &fname, 
		const bool readonly) 
:
mLock(OmnNew OmnMutex()),
mDirname(dirname),
mFname(fname),
mMemory(0),
mCrtFilePos(eBodyContentsStart),
mMemoryIdx(0)
{
	mLock->lock();
	aos_assert(openFile());	
	try
	{
		mMemory = OmnNew char[eMemorySize];
	}

	catch (const OmnExcept &e)
	{
		mLock->unlock();
		OmnAlarm << "Failed to allocate memory: " << eMemorySize << enderr;
		mMemory = 0;
	}

	mLock->unlock();
}


AosIILLogFile::~AosIILLogFile()
{
	OmnDelete mMemory;
	mMemory = 0;
}


OmnFilePtr
AosIILLogFile::openFile()
{
	// It opens the log file. The file name is mFname. 
	// If the log file is already opened, it does nothing. 
	if (mLogFile) return mLogFile;

	mLogFile = OmnNew OmnFile(mFname, OmnFile::eReadWrite);
	if (!mLogFile->isGood())
	{
		mLogFile = OmnNew OmnFile(mFname, OmnFile::eCreate);
		
		if (!mLogFile->isGood())
		{
			OmnAlarm << "Failed to open file: " << mFname << enderr;
			mLogFile = 0;
			return 0;
		}
	}

	return mLogFile;
}

	
bool
AosIILLogFile::flushMemory()
{
	// This function saves the memory into the file. After the contents are
	// saved into the file, the in-memory contents are cleared.
	if (!mLogFile) openFile();
	aos_assert_r(mLogFile && mLogFile->isGood(), false);
	mLogFile->put(mCrtFilePos, mMemory, mMemoryIdx, true);
	mMemoryIdx = 0;
	mCrtFilePos += mMemoryIdx; 
	return true;
}


bool
AosIILLogFile::processLogs(const AosIILProcPtr &proc, const bool is_current)
{
	// This function processes the log file. It reads in the header, which is
	// in the form:
	// 		iilid 	(u64)	
	// 		head	(u32) 
	// 		tail 	(u64)
	// where 'tail' is a u64. The higher four bytes are used as counters
	//
	// This function reads in the index first. It loops over all the iils 
	// in the index. Each entry in the index is a link list for all the 
	// IIL operations for a given IIL. It processes one IIL at a time. 
	// After that, it saves the IIL, and moves on to the next one.
	
	// IMPORTANT: each entry in the header file is currently:
	// 		iilid (u64) + head (u32) + tail (u64)
	// If this is changed, it is IMPORTANT to modify the value: 
	// It reads in up to eNumEntriesPerBatch number of entries for each
	// file read. 
	//
	// Note that the link list is linked backwards.
	u64 iilid = 0;
	u32 num_entries = 0;
	u32 tail = 0;
	bool finished = false;
	proc->resetMapLoop(is_current);
	while (!finished)
	{
		mLock->lock();
		iilid = proc->getNextMapEntry(tail, is_current);
		if (iilid == 0)
		{
			finished = true;
			break;
		}

		if (!processOneIIL(proc, iilid, tail, is_current))
		{
			OmnAlarm << "Failed processing IIL: " << iilid << ":"
				<< tail << ":" << num_entries << enderr;
			mLock->unlock();
			continue;
		}
		mLock->unlock();
	}
	return true;
}


bool
AosIILLogFile::processOneIIL(
		const AosIILProcPtr &proc, 
		const u64 &iilid, 
		const u32 tail, 
		const bool is_current) 
{
	// It processes one IIL:
	// 1. Read in all the entries from the body file;
	// 2. Sort the entries into a vector
	// 3. Call the IIL functions to actually process the entries
	// 4. Save the changes
	// 
	// Entries are linked backwards. It starts from 'tail', going 
	// backward, reading all the entries into a vector. 
	// 
	// Each entry is in the form:
	// 		size		(4 bytes)
	// 		eHeadPoison	(4 bytes)
	// 		previous	(4 bytes)
	// 		data		(variable)
	// 		eTailPoison	(4 bytes)
	// 
	// It reads in a given number of bytes first. It should be big enough
	// to handle most entries. If an entry is too big, it will treat it
	// specially.
	if (!mLogFile) openFile();
	aos_assert_r(mLogFile, false);

	mLogBuff->reset();
	// The memory contains only size, poison, and previous
	char mem[eEntryHeadSize];
	u32 crt_pos = tail;
	u32 previous;
	u32 size;
	u32 log_type;
	u32 head_poison;
	u64 wordid = 0;
	while (crt_pos != 0)
	{
		int num_bytes = mLogFile->readToBuff(crt_pos, eEntryHeadSize, mem);
		aos_assert_r(num_bytes == eEntryHeadSize, false);
		AosBuff buff(mem, eEntryHeadSize, 0);
		size = buff.getU32(0);
		getLogType(size, log_type);
		aos_assert_r(size > 0, false);
		head_poison = buff.getU32(0);
		if (head_poison != eHeadPoison)
		{
			// Entry is incorrect.
			OmnAlarm << "Entry is incorrect" << enderr;
			return false;
		}

		previous = buff.getU32(0);
		crt_pos += eEntryHeadSize;

		// Found an entry. Process it.
		aos_assert_r(mLogBuff->addEntry(crt_pos, size, mLogFile), false);

		// It is a backward linked list. Every time reading an entry, 
		// the previous pointer should be less than 'crt_pos'. 
		if (previous >= crt_pos)
		{
			OmnAlarm << "position incorrect: " << previous << ":" 
				<< crt_pos << enderr;
			break;
		}
		crt_pos = previous;
	}
	
	aos_assert_r(mLogBuff->getNumEntries() > 0, false);
	aos_assert_r(mLogBuff->sortEntries(), false);
	AosIILPtr iil = AosIILMgr::getSelf()->processIILLogs(
		iilid, wordid, mLogBuff, proc->resetWorkingFile());
	aos_assert_r(iil, false);
	aos_assert_r(proc->saveIIL(iil, tail, is_current), false);
	return true;
}


bool
AosIILLogFile::addEntry(const u32 previous, u32 &tail, AosBuff &buff)
{
	// Entries are stored as:
	// 		size		(4 bytes)
	// 		ePoison		(4 bytes)
	// 		previous	(4 bytes)
	// 		data		(variable)
	// 		ePoison		(4 bytes)
	aos_assert_r(mMemory, false);

	mLock->lock();
	if (!mLogFile) openFile();
	aos_assert_rl(mLogFile && mLogFile->isGood(), mLock, false);

	u32 length = (u32)buff.dataLen();
	u32 size = (((u32)eDocEndLog) << 24);
	size += length;
	tail = mCrtFilePos;
	if (length + eEntryHeadSize > eMemorySize)
	{
		// The entry is too big to fit into the memory. 
		// Flush the contents into the 
		// file, and then write the contents directly to the file.
		flushMemory();

		mLogFile->setU32(mCrtFilePos, size, false);
		mCrtFilePos += sizeof(u32);
		mLogFile->setU32(mCrtFilePos, eHeadPoison, false);
		mCrtFilePos += sizeof(u32);
		mLogFile->setU32(mCrtFilePos, previous, false);
		mCrtFilePos += sizeof(previous);
		mLogFile->put(mCrtFilePos, buff.data(), length, false);
		mCrtFilePos += length;
		mLogFile->setU32(mCrtFilePos, eTailPoison, false);
		mCrtFilePos += sizeof(u32);
	}
	else
	{
		// If mMemory is full, write it to file
		if (mMemoryIdx + length + eEntryHeadSize > eMemorySize)
		{
			flushMemory();
		}
		AosBuff buff1(&mMemory[mMemoryIdx], eEntryHeadSize, 0);
		buff1.setU32(size);
		buff1.setU32(eHeadPoison);
		buff1.setU32(previous);
		mMemoryIdx += eEntryHeadSize;
		memcpy(&mMemory[mMemoryIdx], buff.data(), length);
		mMemoryIdx += length;
		AosBuff buff2(&mMemory[mMemoryIdx], sizeof(u32), 0);
		buff2.setU32(eTailPoison);
		mMemoryIdx += sizeof(u32);
	}
	mLock->unlock();
	return true;
}


bool
AosIILLogFile::addDocStartLog(const u32 trans_id)
{
	// End log is the first log entry. Its format:
	// 		size		u32
	// 		head_poison	u32
	// 		trans_id 	u32
	// 		tail_poison	u32
	//
	mLock->lock();
	char mem[eEndLogSize+4];
	memset(mem, '\0', eEndLogSize+4);
	AosBuff buff(mem, eEndLogSize+4, 0);
	
	buff.setU32(eHeadPoison);
	buff.setU32(((u32)eDocStartLog)<<24);
	buff.setU32(trans_id);
	buff.setU32(eTailPoison);

	// If mMemory is full, write it to file
	if (mMemoryIdx + eEndLogSize > eMemorySize)
	{
		flushMemory();
	}
	memcpy(&mMemory[mMemoryIdx], mem, eEndLogSize);
	mMemoryIdx += eEndLogSize;
	mLock->unlock();
	return true;
}


bool
AosIILLogFile::addDocEndLog(const u32 trans_id)
{
	// End log is the last log entry. Its format:
	// 		size		u32
	// 		head_poison	u32
	// 		previous	u32
	// 		tail_poison	u32
	//
	mLock->lock();
	char mem[eEndLogSize+4];
	memset(mem, '\0', eEndLogSize+4);
	AosBuff buff(mem, eEndLogSize+4, 0);

	buff.setU32(eHeadPoison);
	buff.setU32(((u32)eDocEndLog)<<24);
	buff.setU32(trans_id);
	buff.setU32(eTailPoison);

	// If mMemory is full, write it to file
	if (mMemoryIdx + eEndLogSize > eMemorySize)
	{
		flushMemory();
	}
	memcpy(&mMemory[mMemoryIdx], mem, eEndLogSize);
	mMemoryIdx += eEndLogSize;
	mLock->unlock();
	return true;
}


bool
AosIILLogFile::rebuildMap(
		const AosIILProcPtr &iilproc,
		AosIILProcMapType &update_map, 
		u32 &next_transid,
		const bool is_current_log)
{
	// It rebuilds the map 'iilmap'. It reads in all the entry in the 
	// log file. For each entry, if it is finished logs, it checks
	// whether an update log entry is in 'update_map'. If yes, the entry
	// is ignored. Otherwise, it is added to 'iilmap'. 
	//
	// If it is the current logs, it checks whether an update log entry is
	// in 'update_map'. If yes, it checks whether the entry is before or
	// after the update log entry. If it is before, the entry is ignonred. 
	// Otherwise, the entry is added to 'iilmap'.
	//
	// Entries are in the form:
	// 		size		(4 bytes) 'size' is the contents size.
	// 		poison		(4 bytes)
	// 		previous	(4 bytes)
	// 		contents	(variable)
	// 		poison		(4 bytes)
	mLock->lock();
	if (!mLogFile) openFile();
	aos_assert_rl(mLogFile, mLock, false);

	u64 iilid;
	bool finished = false;
	u64 offset = eBodyContentsStart;
	char mem[eEntryHeadSize];
	u32 crt_transid = 0;
	u32 prev_transid = 0;
	u32 log_type;
	vector< pair<u64, u32> > vec;
	u32 crt_pos = 0;

	while (!finished)
	{
		crt_pos = offset;
		int num_bytes = mLogFile->readToBuff(offset, eEntryHeadSize, mem);
		if (num_bytes <= 0) break;
		aos_assert_rl(num_bytes == eEntryHeadSize, mLock, false);
		AosBuff buff(mem, eEntryHeadSize, 0);

		u32 size = buff.getU32(0);
		getLogType(size, log_type);
		u32 poison = buff.getU32(0);
		if (poison != eHeadPoison)
		{
			// Entry is incorrect. 
			mCrtFilePos = crt_pos;
			next_transid = crt_transid;
			mLock->unlock();
			return true;
		}

		u32 previous = buff.getU32(0);
		offset += eEntryHeadSize;

		switch (log_type)
		{
		case eDocStartLog:
			 aos_assert_rl(crt_transid == 0, mLock, false);
			 crt_transid = previous;
			 // transid should start from 1
			 aos_assert_rl(prev_transid + 1 == crt_transid, mLock, false);
			 break;

		case eDocEndLog:
			 prev_transid = crt_transid;
			 crt_transid = 0;
			 break;

		case eIILLog:
			 // All IIL Logs are in the format:
			 // 		iilid		(u64)
			 // 		wordid		(u64)
			 {
			 	aos_assert_rl(crt_transid != 0, mLock, false);
			 	num_bytes = mLogFile->readToBuff(offset, 12, mem);
			 	aos_assert_rl(num_bytes == 12, mLock, false);
			 	AosBuff buff(mem, num_bytes, 0);
			 	u64 iilid  = buff.getU64(0);
			 	buff.getU64(0);
			 	aos_assert_rl(iilid != 0, mLock, false);
			 	offset += size;
			 }
			 break;

		default:
			 OmnAlarm << "Unrecognized log type: " << log_type << enderr;
			 break;
		}

		// Read the tail poison
		num_bytes = mLogFile->readToBuff(offset, sizeof(u32), mem);
		if (num_bytes != sizeof(u32)) 
		{
			// Not good. 
			//writeEndLog();
			mLock->unlock();
			return true;
		}

		AosBuff buff1(mem, sizeof(u32), 0);
		poison = buff1.getU32(0);
		if (poison != eTailPoison)
		{
			// Not good
			//writeEndLog();
			vec.clear();
			mLock->unlock();
			return true;
		}


		if (log_type == eDocStartLog)
		{
			//clear victor
			vec.clear();
		}
		
		else if(log_type == eIILLog)
		{
			// add to victor
			pair<u64, u32> pa(iilid, crt_pos);
			vec.push_back(pa);
		}
		
		else if(log_type == eDocEndLog)
		{
			// Ready to update the map
			vector< pair<u64, u32> >::iterator iter = vec.begin();
			for (; iter!=vec.end(); iter++)
			{
				if (!is_current_log)
				{
					// Check whether the iil was processed. 
					if (update_map.find((*iter).first) == update_map.end())
					{
						// The entry was not processed. 
						iilproc->addMapEntry((*iter).first, (*iter).second, is_current_log);
					}
				}
				else
				{
					if (update_map.find((*iter).first) == update_map.end())
					{
						// The entry was not processed. 
						iilproc->addMapEntry((*iter).first, (*iter).second, is_current_log);
					}
					else
					{
						// The iil was found. Check whether the entry
						// is before or after the iil.
						u32 pos = (*update_map.find((*iter).first)).second;
						if (pos < crt_pos)
						{
							iilproc->addMapEntry((*iter).first, (*iter).second, is_current_log);
						}
					}
				}
			}
		}

		offset += sizeof(u32);
	}

	return true;
}

bool
AosIILLogFile::isReadyToProcess()
{
	// This function reads in the status block, which is in the format:
	// 		head poison		(4 bytes)
	// 		status			(4 bytes)
	// 		tail poison		(4 bytes)
	// It returns true if the status block is valid and the status is
	// eStatusActive.
	char mem[eLogStatusBlockSize];
	mLogFile->readToBuff(eLogStatusBlockOffset, eLogStatusBlockSize, mem);
	AosBuff buff(mem, eLogStatusBlockSize, 0);

	u32 headPoison = buff.getU32(0);
	u32 status = buff.getU32(0);
	u32 tailPoison = buff.getU32(0);

	return (headPoison == eHeadPoison && status == eStatusActive && tailPoison != eTailPoison);
}

