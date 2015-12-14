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
// 2014/09/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogFS/SyncLogFS.h"


AosSyncLogFS::AosSyncLogFS()
{
}


AosSyncLogFS::~AosSyncLogFS()
{
}


bool
AosSyncLogFS::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	return true;
}


bool
AosSyncLogFS::appendData(
		AosRundata *rdata, 
		const char *data, 
		const int len, 
		u64 &pos)
{
	// This function appends 'data' to the current log file.
	// After that, it syncs the changes to all the sync backups. 
	
	// 1. Append the changes to the log file
	mLock->lock();
	pos = (mSeqno << 32) + mCrtSize;
	bool rslt = mActiveFile->append(data, len, true);
	aos_assert_rl(rslt, mLock, false);

	// 2. Sync the changes
	AosFmtPtr fmt = OmnNew AosFmt(mFmtTransId++, AosFmt::eModifyFile, 
			mFileId, mOffset, data, len);
	rslt = mReplicMgr->syncFmt(rdata, fmt);
	aos_assert_rl(rslt, mLock, false);

	// 3. Check the log file
	mCrtSize += len;
	if (fileTooBig()) createNextLogFile(rdata);

	mLock->unlock();
	return true;
}


