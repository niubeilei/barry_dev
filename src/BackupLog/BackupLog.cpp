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
// 2013/05/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BackupLog/BackupLog.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"



AosBackupLog::AosBackupLog()
:
mLock(OmnNew OmnMutex())
{
}


AosBackupLog::~AosBackupLog()
{
}


bool 
AosBackupLog::appendLog(	
		const AosRundataPtr &rdata, 
		const u64 docid, 
		const AosBuffPtr &body)
{
	// Entry format is:
	// 		Front Poison		(u64)
	// 		Length				(int64_t)
	// 		docid				(u64)
	// 		body				(variable)
	// 		SH1					(20 bytes)
	// 		End Poison			(u64)
	
	aos_assert_rr(body, rdata, false);
	aos_assert_rr(docid > 0, rdata, false);
	mLock->lock();
	int64_t length = body->dataLen();
	OmnFilePtr file = getLogFileLocked(rdata, length);
	aos_assert_rl(file, mLock, false);

	AosBuffPtr bb = OmnNew AosBuff(length+eHeaderSize);
	aos_assert_rl(bb, mLock, false);

	bb->setU64(eFrontPoison);
	bb->setInt64(length);
	bb->setU64(docid);
	bb->appendBuffAsBinary(buff);
	bb->setU64(eEndPoison);

	file->append(bb->data(), bb->dataLen(), true);
	mLock->unlock();
	return true;
}


bool 
AosBackupLog::checkSystemRestart(const AosRundataPtr &rdata)
{
}


bool 
AosBackupLog::recoverDoc(
		const AosRundataPtr &rdata, 
		const u64 docid,
		AosBuffPtr &body)
{
}


OmnFilePtr
AosBackupLog::getLogFileLocked(
		const AosRundataPtr &rdata, 
		const int64_t length)
{
	if (!mCrtLogFile)
	{
		mCrtLogFile = openLogFile(rdata, length);
		aos_assert_rr(mCrtLogFile, rdata, false);
	}

	if (mCrtLogFileSize + length > mMaxLogFileSize)
	{
		mCrtLogFile = createNextLogFile(rdata, length);
		aos_assert_rr(mCrtLogFile, rdata, false);
	}

	return mCrtLogFile;
}


OmnFilePtr
AosBackupLog::openLogFile(
		const AosRundataPtr &rdata, 
		const int64_t length)
{
	// This function opens the last log file. 
}

