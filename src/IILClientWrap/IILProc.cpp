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
// There are two logs: Current and Finished. The Current Log is used
// as the working log. All new logs are appended to this log. 
// The Finished Log is the one that can be processed. 
//
// IILID Map
// There is an IILID Map:
// 		[iilid, tail]
// Each entry maps an IILID to a tail. All IIL logs are linked
// in the log file backwards. The tail is kept by this map. When
// we want to know whether an IIL is modified, it can use this
// map to quickly determine it.
// This map will be rebuild when the system comes up.
//
// Update Log
// This is a log that stores all the finished logs. 
// 		poison		(4 bytes)
// 		iilid 		(8 bytes)
// 		tail 		(4 bytes)
// 		poison		(4 bytes)
// When an IIL is processed, it adds an entry [iilid, tail], 
// where 'tail' is the last log entry being processed. This 
// is used to rebuild the IILID Map, which means that when 
// rebuilding the IILID Map, it should ignore all the entries
// that are older than the ones recorded by this log.
//
// Processing Logs
// 1. Process all the logs
// 2. Save IIL
// 3. Add the Update Log [iilid, tail]
//
// Modification History:
// 04/01/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILClient/IILProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILClient/IILLogFile.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "IILMgr/IIL.h"


#include <dirent.h>
#include <sys/types.h>


AosIILProc::AosIILProc(const OmnString &dir, 
					   const OmnString &archivename,
					   const u32 seqno) 
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mSeqno(seqno),
mDirname(dir),
mArchivename(archivename),
mCurrentMap(OmnNew AosIILProcMapType()),
mFinishedMap(0),
mProcessFinishedLogs(false),
mStatus(eStatusIdle)
{
	// Create the directory
	aos_assert(mDirname != "");
	mDirname << "/" << mSeqno;

	OmnString cmd = "mkdir -p ";
	cmd << mDirname;
	system(cmd);

	DIR *dd;
	if ((dd= opendir(mDirname)) == NULL)
	{
	 	OmnAlarm << "iilwrap file not created!" << enderr;
	 	closedir(dd);
	}

	// Rebuild Map
	rebuildMap();
	
	// Check iiltemple file
	recoverIIL();

	// Check whether we need to process the finished log file
	initFinishedLogFile();

	mCurrentLogs = OmnNew AosIILLogFile(mDirname, getCurrentFname(), false);
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "buff", 0, false, true, __FILE__, __LINE__);
   	mThread->start();
}


AosIILProc::~AosIILProc()
{
	if (mThread) mThread->stop();
	OmnThreadMgr::getSelf()->removeThread(mThread);
	mThread = 0;
}


bool
AosIILProc::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	// This thread is responsible for processing batch files. It normally
	// waits for batch files to be available. When it is available, 
	// it processes all the iil logs in the batch file. After that, 
	// it checks whether there are more batch files. if yes, it continues
	// processing it until all are processed. After that, it sleeps
	// again. AosIILProcMgr is responsible for waking this thread up.
	// Note that the system may have multiple instances of this class. 
	//
	// There may be multiple batch files. Batch file order is important.
	// Each instance of this class assumes a separate directory. 
	// All the batch files for this instance are stored in that directory.
	// When waking up, it reads in all the files in that directory.
	// All files are named by seqnos, such as 0001, 0002, 0003, etc.
	// It finds the first one in the order, checks whether the file
	// is finished. If not, it is an error. It raises an alarm and 
	// quit. Otherwise, it processes all the finished batches. 
	AosIILProcPtr thisptr(this, false);
	AosIILLogFilePtr logfile;
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (!mProcessFinishedLogs)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		mProcessFinishedLogs = false;

		try
		{
			// Time to process the finished logs.
			if (!mFinishedLogs) openFinishedLog();
			if (!mFinishedLogs)
			{
				OmnAlarm << "Failed to create the finished log!" << enderr;
				mLock->unlock();
				continue;
			}
			mLock->unlock();
			bool rslt = mFinishedLogs->processLogs(thisptr, false);
			if (!rslt)
			{
				OmnAlarm << "Failed to process the logs: " << enderr;
				continue;
			}
			
			// Need to reset the finished logs.
			mLock->lock();
			mFinishedLogs = 0;
			mFinishedMap = 0;
			archiveFinishedLogs();
			mStatus = eStatusIdle;
			mLock->unlock();	
		}

		catch (const OmnExcept &excp)
		{
			OmnAlarm << "Failed processing logs: " << excp.getErrmsg() << enderr;
		}
	}
	return true;
}


bool
AosIILProc::signal(const int threadLogicId)
{
	return true;
}


bool
AosIILProc::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool 
AosIILProc::rebuildMap()
{
	// It reads in the following files:
	// 		current
	// 		finished
	// 		current_iillog
	// 		finished_iillog
	// to construct the map. 
	mLock->lock();
	
	AosIILProcPtr thisPtr(this, false);
	try
	{
		// 1. Read in Finished Update Log File
		aos_assert_rl(openFinishedLog(), mLock, false);
		if (mFinishedLogs->isFileGood())
		{
			AosIILProcMapType updateMap;
			aos_assert_rl(readUpdateLogEntry(updateMap, 
				getFinishedUpdateLogFname()), mLock, false);
	
			// 2. Read in the Finished Log File
			u32 transid;
			aos_assert_rl(mFinishedLogs->rebuildMap(thisPtr, 
				updateMap, transid, false), mLock, false);
		}

		mCurrentLogs = OmnNew AosIILLogFile(mDirname, getCurrentFname(), false);
		if (mCurrentLogs ->isFileGood())
		{
			// 3. Read in Current Update Log File
			AosIILProcMapType updateMap;
			aos_assert_rl(readUpdateLogEntry(updateMap, 
				getCrtUpdateLogFname()), mLock, false);
		
			// 4. Read in Current Log File
			aos_assert_rl(mCurrentLogs->rebuildMap(thisPtr, 
				updateMap, mProcessedTransid, true), mLock, false);
		}

	}	

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to rebuild the map: " << e.getErrmsg() << enderr;
		mLock->unlock();
		return false;
	}

	mLock->unlock();
	return true;
}


bool
AosIILProc::appendLog(const u64 &iilid, AosBuff &buff)
{
	// When an IIL transaction occurs, this function is called. 
	// It creates a log for the transaction. 
	// Entries are stored as:
	// 		size		(4 bytes)
	// 		signature	(4 bytes)
	// 		previous	(4 bytes)
	// 		data		(variable)
	// It tries to put into the memory. If there is no enough space, 
	// it will flush the contents into the log file. 
	aos_assert_r(mCurrentMap, false);
	mLock->lock();
	AosIILProcMapType::iterator itr = mCurrentMap->find(iilid);
	u32 previous = 0;
	if (itr != mCurrentMap->end())
	{
		previous = itr->second;
	}
	aos_assert_rl(mCurrentLogs, mLock, false);
	u32 tail = 0;
	aos_assert_rl(mCurrentLogs->addEntry(previous, tail, buff), mLock, false);
	aos_assert_rl(tail > 0, mLock, false);
	(*mCurrentMap)[iilid] = tail;
	mLock->unlock();
	return true;
}


bool
AosIILProc::docTransStarted(const u32 transid)
{
	// IIL processing is grouped based on doc transactions. 
	// When a doc transaction starts, this function is called.
	return mCurrentLogs->addDocStartLog(transid);
}


bool
AosIILProc::docTransFinished(const u32 transid)
{
	// IIL processing is grouped based on doc transactions. 
	// When a doc transaction finishes, this function is called.
	// It copies the pending iil logs to the in-memory log file.
	// After that, the pending in-memory log file is cleared.
	return mCurrentLogs->addDocEndLog(transid);
}


bool
AosIILProc::isIILModified(const u64 &iilid)
{
	mLock->lock();
	bool rslt = (mCurrentMap->find(iilid) != mCurrentMap->end() || 
				 (mFinishedMap && mFinishedMap->find(iilid) != mFinishedMap->end()));
	mLock->unlock();
	return rslt;
}


bool
AosIILProc::saveIIL(
		const AosIILPtr &iil, 
		const u64 &tail,
		const bool is_current)
{
	// When finished processing an IIL for the Finished Log File, it should
	// call this function to save the IIL. It saves just one IIL (i.e., one
	// subiil). If there are multiple subiils, it will loop over. 
	// 1. Set the working file header
	// 2. Retrieve the IIL contents from file and saves it in the working file
	// 2. Save the IILs into disks
	// 3. Add an Update Log Entry
	// 4. Reset the map
	//
	// 'iil' points to the root IIL.
	
	aos_assert_r(iil, false);
	mLock->lock();
	aos_assert_rl(openWorkingFile(), mLock, false);
	u64 iilid = iil->getIILID();
	
	// 1. Set the working file header
	char mem[eIILWorkingHeadSize];
	AosBuff buff(mem, eIILWorkingHeadSize, 0);
	buff.setU32(eHeadPoison);				// Header Poison
	buff.setU32(eWorkingFinishedOnly);		// Status
	buff.setU64(iilid);						// IILID
	buff.setU32(tail);						// Finished Tail
	buff.setU32(0);							// Current Tail
	buff.setU32(eTailPoison);				// Tail Poison
	mWorkingFile->put(eIILWorkingHeadPos, mem, eIILWorkingHeadSize, false);

	// 2. Copy the original IIL contents to working file
	aos_assert_rl(iil->isRootIIL(), mLock, false);
	u32 crt_pos = eIILWorkingContentsStart;
	aos_assert_rl(iil->saveIILsToFileSafe(mWorkingFile, crt_pos), mLock, false); 

	// 3. Close the working file
	mWorkingFile->closeFile();
	mWorkingFile = 0;
	
	// 4. Save the IIL. 
	aos_assert_rl(iil->saveToFileSafe(false), mLock, false);

	// 5. The IIL is saved. Update the finished log
	aos_assert_rl(addFinishedUpdateLogEntry(iilid, tail), mLock, false);

	// 6. Remove the working file
	aos_assert_rl(removeIILWorkingFile(), mLock, false);

	// 7. Erase the entry from the finished map
	mFinishedMap->erase(iilid);
	mLock->unlock();
	return true;
}


bool
AosIILProc::openWorkingFile()
{
	mWorkingFile = OmnNew OmnFile(getIILWorkingFname(), OmnFile::eCreate);
	return mWorkingFile && mWorkingFile->isGood();
}


bool
AosIILProc::removeIILWorkingFile()
{
	OmnString cmd = "rm -f ";
	cmd << getIILWorkingFname();
	system(cmd);
	return true;
}


bool
AosIILProc::addFinishedUpdateLogEntry(const u64 &iilid, const u32 tail)
{
	// This function is called when logs being processed for an IIL. 
	// There can be multiple logs being processed. The parameter 'tail'
	// is the position of the last log for the IIL. This information 
	// is used to rebuild the map in case the system crashes/restarts. 
	//
	// This function adds an Update Log Entry to the Update Log File:
	// 		poison		(4 bytes)
	// 		iilid 		(8 bytes)
	// 		tail 		(4 bytes)
	// 		poison		(4 bytes)

	mLock->lock();
	if (!mFinishedUpdateLogFile) openFinishedUpdateLogFile();
	aos_assert_rl(mFinishedUpdateLogFile, mLock, false);
	
	char mem[eUpdateLogEntrySize];
	AosBuff buff(mem, eUpdateLogEntrySize, 0);
	buff.setU32(eHeadPoison);
	buff.setU64(iilid);
	buff.setU32(tail);
	buff.setU32(eTailPoison);
	
	mFinishedUpdateLogFile->put(mUpdatePos, mem, eUpdateLogEntrySize, true);
	mUpdatePos += eUpdateLogEntrySize;
	mLock->unlock();
	return true;
}


bool
AosIILProc::readUpdateLogEntry(
		AosIILProcMapType &themap, 
		const OmnString &fname)
{
	// This function reads all the logs in the update log file 'fname' 
	// into 'themap'. Or in other word, it rebuilds the map. 
	// Entry format is:
	// 			poison		(u32)
	// 			iilid		(u64)
	// 			tail		(u32)
	// 			poison		(u32)
	aos_assert_r(fname != "", false);
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	aos_assert_r(ff, false);
	if (!ff->isGood()) return true;
	u64 crt_pos = eUpdateLogStart;
	char mem[eUpdateLogEntrySize];
	while (1)
	{
		int num_bytes = ff->readToBuff(crt_pos, eUpdateLogEntrySize, mem);
		if (num_bytes <= 0) break;
		aos_assert_r(num_bytes == eUpdateLogEntrySize, false);
		AosBuff buff(mem, eUpdateLogEntrySize, 0);
		u32 headPoison = 	buff.getU32(0);
		u64 iilid = 		buff.getU64(0);
		u32 tail = 			buff.getU32(0);
		u32 tailPoison = 	buff.getU32(0);
		
		aos_assert_r(headPoison == eHeadPoison, false);
		aos_assert_r(tailPoison == eTailPoison, false);
		aos_assert_r(iilid != 0, false);

		themap[iilid] = tail;
		crt_pos += eUpdateLogEntrySize;
	}
	return true;
}


bool
AosIILProc::openFinishedUpdateLogFile()
{
	OmnString fname = getFinishedUpdateLogFname();
	mFinishedUpdateLogFile = OmnNew OmnFile(fname, OmnFile::eReadWrite);
	if (!mFinishedUpdateLogFile->isGood())
	{
		mFinishedUpdateLogFile = OmnNew OmnFile(fname, OmnFile::eCreate);
	}
	aos_assert_r(mFinishedUpdateLogFile && mFinishedUpdateLogFile->isGood(), false);
	return true;
}


bool
AosIILProc::initFinishedLogFile()
{
	// This function is called when the system restarts. It checks
	// whether the finished log file exists. If not, it does nothing. 
	// Otherwise, it checks whether the file is ready to be processed. 
	// If not, it does nothing. 
	//
	// The log file should have a flag that indicates whether it is 
	// ready to be processed:
	// 		poison
	// 		"ReadyToProcess"
	// 		poison
	// If the flag exists and the two poisons are good, the file
	// is ready to be processed.
	mLock->lock();
	mProcessFinishedLogs = false;
	aos_assert_rl(openFinishedLog(), mLock, false);
	if (!mFinishedLogs->isReadyToProcess()) 
	{
		mLock->unlock();
		return false;
	}
	mLock->unlock();
	mProcessFinishedLogs = true;
	return true;
}


bool
AosIILProc::recoverIIL()
{
	// This function is called when the system starts. It checks whether
	// the working file exists. If not, it does nothing. If the working
	// file is not good, do nothing.
	mLock->lock();
	OmnString fname = getIILWorkingFname(); 
	OmnFilePtr working_file = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	if(!working_file || !working_file->isGood()) 
	{
		mLock->unlock();
		return true;
	}
	
	// The working file exists. Check whether the file is good (by checking
	// the poison).
	char mem[eIILWorkingHeadSize];
	int num_bytes = working_file->readToBuff(eHeadPosionOffset, eIILWorkingHeadSize, mem);
	if (num_bytes != eIILWorkingHeadSize) 
	{
		// The file is not good. Ignore it.
		mLock->unlock();
		return true;
	}

	// The working file header is:
	// 	header poison		(4 bytes)
	// 	status				(4 bytes)
	// 	iilid				(8 bytes)
	// 	finished tail		(4 bytes)
	// 	current tail		(4 bytes)
	// 	tail poison			(4 bytes)
	AosBuff buff(mem, eHeadSize, 0);
	u32 headPoison 		=  buff.getU32(0);
	u32 status 			=  buff.getU32(0);
	u64 iilid  			=  buff.getU64(0);
	u32 finished_tail   =  buff.getU32(0);
	u64 tailPoison 		=  buff.getU32(0);

	if (headPoison != eHeadPoison || tailPoison != eTailPoison)
	{
		// The record is not good. Ignore it.
		mLock->unlock();
		return true;
	}

	if (finished_tail == 0 || iilid == 0)
	{
		// The record is not good. 
		mLock->unlock();
		return true;
	}

	// The working file is good.
	switch (status)
	{
	case eWorkingFinishedOnly:
		 // It contains the original contents. Check whether an update log entry
		 // is there. If yes, the saving is good. Remove the temporary file. 
		 // Otherwise, the saving is not good. Need to recover it.
		 if (doesUpdateLogExist(getFinishedUpdateLogFname(), iilid, finished_tail))
		 {
			 // The saving is good. Need to remove the IIL temporary file
			 mLock->unlock();
			 return removeIILWorkingFile();	
		 }
		 
		 // The saving is not good. Need to recover the IIL.
		 aos_assert_rl(recoverIIL(working_file), mLock, false);
		 mLock->unlock();
		 return false;

	default:
		 break;
	}

	mLock->unlock();
	OmnAlarm << "Unrecognized status: " << status << enderr;
	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILProc::recoverIIL(const OmnFilePtr &file)
{
	// It copies all the contents back to the IILs. The contents are in 
	// the form:
	// 		Length		(4 bytes)
	// 		Header		(variable)
	// 		Length		(4 bytes)
	// 		Body		(variable)
	// 		...
	if (!file) return true;

	u32 pos = eIILWorkingContentsStart;
	while (1)
	{
		u32 len = file->readU32(pos, 0);
		if (len == 0)
		{
			// No more contents. 
			break;
		}

		// Read the header buff
		pos += 4;
		AosBuffPtr headerbuff = OmnNew AosBuff(len, 0);
		char *data = headerbuff->data();
		int num = file->readToBuff(pos, len, data);
		aos_assert_r(num > 0 && (u32)num == len, false);
		pos += len;

		// Read the bodybuff
		len = file->readU32(pos, 0);
		aos_assert_r(len > 0, false);
		pos += 4;
		AosBuffPtr bodybuff = OmnNew AosBuff(len, 0);
		data = bodybuff->data();
		num = file->readToBuff(pos, len, data);
		aos_assert_r(num > 0 && (u32)num == len, false);
		pos += len;
		
		// Need to save the data to the original locations
		aos_assert_r(AosIIL::saveToFileSafeStatic(headerbuff, bodybuff), false);
	}

	return true;
}


bool
AosIILProc::doesUpdateLogExist(
		const OmnString &fname, 
		const u64 &the_iilid, 
		const u32 the_tail)
{
	// This function reads in the finished update log file to see whether
	// the iil is processed. It is processed if and only if it finds
	// the entry [iilid, tail]. Note that the tail must match 'the_tail'. 
	
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	if (!ff || !ff->isGood()) return false;
	u64 crt_pos = eUpdateLogStart;
	char mem[eUpdateLogEntrySize];
	while (1)
	{
		int num_bytes = ff->readToBuff(crt_pos, eUpdateLogEntrySize, mem);
		if (num_bytes <= 0) return false;
		aos_assert_r(num_bytes == eUpdateLogEntrySize, false);
		AosBuff buff(mem, eUpdateLogEntrySize, 0);
		u64 headPoison = 	buff.getU32(0);
		u64 iilid = 		buff.getU64(0);
		u32 tail = 			buff.getU32(0);
		u64 tailPoison = 	buff.getU32(0);
		aos_assert_r(headPoison == eHeadPoison, false);
		aos_assert_r(tailPoison == eTailPoison, false);
		aos_assert_r(iilid != 0, false);
		if (the_iilid == iilid && tail == the_tail) return true;

		crt_pos += eUpdateLogEntrySize;
	}
	return false;
}


bool
AosIILProc::resetMapLoop(const bool is_current)
{
	mLock->lock();
	if (is_current)
	{
		if (!mCurrentMap) 
		{
			mLock->unlock();
			return true;
		}

		mCurrentItr = mCurrentMap->begin();
		mLock->unlock();
		return true;
	}
	
	if (!mFinishedMap) 
	{
		mLock->unlock();
		return true;
	}

	mFinishedItr = mFinishedMap->begin();
	mLock->unlock();
	return true;
}


u64 
AosIILProc::getNextMapEntry(u32 &tail, const bool is_current)
{
	// This function retrieves the next entry and then erases it.
	tail = 0;
	mLock->lock();
	AosIILProcMapType *map = (is_current)?mCurrentMap:mFinishedMap;
	AosIILProcMapType::iterator itr = (is_current)?mCurrentItr:mFinishedItr;

	aos_assert_rl(map, mLock, 0);
	itr++;
	if (itr == map->end())
	{
		mLock->unlock();
		return 0;
	}
	u64 iilid = itr->first;
	tail = itr->second;
	mLock->unlock();
	return iilid;
}


bool
AosIILProc::switchLogs()
{
	// It checks whether the finished log file exists. If yes, it does nothing. 
	// Otherwise, it sets the flag. 
	mLock->lock();
	switch (mStatus)
	{
	case eStatusProcessingFinished:
		 // It is still processing the finished logs. 
		 mProcessFinishedLogs = true;
		 mLock->unlock();
		 return true;

	case eStatusIdle:
		 // It is not processing the finished logs. 
		 aos_assert_rl(!mFinishedMap, mLock, false);
		 aos_assert_rl(!mFinishedLogs, mLock, false);
		 mFinishedMap = mCurrentMap;
		 mCurrentMap = OmnNew AosIILProcMapType();
		 aos_assert_rl(mCurrentLogs, mLock, false);
		 mCurrentLogs->closeFile();
		 mCurrentLogs = 0;
		 renameLogFile();
		 aos_assert_rl(openFinishedLog(), mLock, false);
		 mCurrentLogs = OmnNew AosIILLogFile(mDirname, getCurrentFname(), false);
		 mCondVar->signal();
		 mLock->unlock();
		 return true;

	default:
		 mLock->unlock();
		 OmnAlarm << "Unrecognized status: " << mStatus << enderr;
		 return false;
	}

	mLock->unlock();
	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILProc::renameLogFile()
{
	aos_assert_r(!mCurrentLogs, false);
	OmnString cmd = "mv ";
	cmd << mDirname << "/" << getCurrentFname() << " " 
		<< mDirname << "/" << getFinishedFname();
	system(cmd);
	return true;
}


bool
AosIILProc::archiveFinishedLogs()
{
	aos_assert_r(!mFinishedLogs, false);
	OmnString cmd = "mv ";
	cmd << mDirname << "/" << getFinishedFname() 
		<< " " << mArchivename << "/";
	cmd << (u32)OmnGetSecond();
	system(cmd);
	return true;
}


bool
AosIILProc::removeFinishedMapEntry(const u64 &iilid)
{
	mLock->lock();
	aos_assert_rl(mFinishedMap, mLock, false);
	mFinishedMap->erase(iilid);
	mLock->unlock();
	return true;
}


bool			
AosIILProc::addMapEntry(const u64 &iilid, const u32 crt_pos, const bool is_current)
{
	mLock->lock();
	AosIILProcMapType *map = (is_current)?mCurrentMap:mFinishedMap;
	aos_assert_rl(map, mLock, false);
	(*map)[iilid] = crt_pos;
	mLock->unlock();
	return true;
}


bool
AosIILProc::processOneIIL(const u64 &iilid)
{
	// This function is called when a query needs to use 
	// an IIL. It checks whether the IIL was modified. 
	// If not, it does nothing. Otherwise, it processes
	// the IIL, save the IIL, and then returns.
	// It does the following:
	// 1. If mFinishedMap is not null, process the finished log
	// 2. Process the current log
	// 3. Process the current IIL operation
	mLock->lock();
	if (mFinishedMap)
	{
		AosIILProcMapType::iterator itr = mFinishedMap->find(iilid);
		if (itr != mFinishedMap->end())
		{
			// It is modified by mFinished Logs
			AosIILProcPtr thisptr(this, false);
			if (!mFinishedLogs) openFinishedLog();
			aos_assert_rl(mFinishedLogs, mLock, false);
			mFinishedLogs->processOneIIL(thisptr, iilid, itr->second, false);
		}
	}

	if (mCurrentLogs && mCurrentMap)
	{
		AosIILProcMapType::iterator itr = mCurrentMap->find(iilid);
		if (itr != mCurrentMap->end())
		{
			// It is modified by mCurrent Logs
			AosIILProcPtr thisptr(this, false);
			mCurrentLogs->processOneIIL(thisptr, iilid, itr->second, true);
		}
	}
	mLock->unlock();
	return true;
}


bool
AosIILProc::openFinishedLog()
{
	if (mFinishedLogs) return true;
	try
	{
		mFinishedLogs = OmnNew AosIILLogFile(mDirname, getFinishedFname(), true);
		if (!mFinishedLogs->isFileGood())
		{
			mFinishedLogs = 0;
			return false;
		}
		return true;
	}
	
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to create finished log: " << e.getErrmsg() << enderr;
		return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


