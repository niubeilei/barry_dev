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
// 1. Logs are saved in storage files. Each snapshot is saved in one
//    file or more files. The file key is:
//    	AOSFILEKEY_SNAPSHOT_FILENAME_dfmid_snapshot_name_seqno
//    The file has a maximum size. When the maximum size is reached, it creates
//    the next file. The last one is always:
//      AOSFILEKEY_SNAPSHOT_FILENAME + "_" + dfmid + "_" snapshot_name
//    
//    This means that if there are five files, it should be:
//    	AOSFILEKEY_SNAPSHOT_FILENAME_dfmid_snapshot_name
//    	AOSFILEKEY_SNAPSHOT_FILENAME_dfmid_snapshot_name_3
//    	AOSFILEKEY_SNAPSHOT_FILENAME_dfmid_snapshot_name_2
//    	AOSFILEKEY_SNAPSHOT_FILENAME_dfmid_snapshot_name_1
//    	AOSFILEKEY_SNAPSHOT_FILENAME_dfmid_snapshot_name_0
//
// Modification History:
// 12/12/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SystemLog/SystemLog.h"


AosSystemLog::AosSystemLog()
:
mIsSystemLogOn(true)
{
}


AosSystemLog::~AosSystemLog()
{
}


bool
AosSystemLog::config(const AosXmlTagPtr &conf)
{
	AosXmlTagPtr tag = conf->getFirstChild("db_system_log");
	if (!tag) return true;

	mSysLogOn = tag->getAttrBool("system_log_on", true);

	return true;
}


bool
AosSystemLog::docCreated(
		const u64 &dfmid,
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	// A doc is created. It creates a log entry:
	// 	[eCreateDoc, docid]
	if (!mSysLogOn) return true;

	mBuff->setU32((u32)eCreateDoc);
	mBuff->setU64(dfmid);
	mBuff->setU64(docid);
	return false;
}


bool
AosSystemLog::docDeleted(
		const u64 &dfmid,
		const u64 &docid, 
		bool &need_delete, 
		const char *header,
		const int header_len,
		const AosRundataPtr &rdata)
{
	// A doc is deleted. Instead of deleting the doc, the syslog
	// will keep the doc. It is important that DocFileMgr does not
	// remove the doc. An entry is created:
	// 	[eDeleteDoc, dfmid, docid, header]
	//
	// If the syslog is not turned on, it sets 'need_delete' to true.
	// Otherwise, it sets 'need_delete' to false. The caller (which 
	// should be a DocFileMgr) should check 'need_delete'. If it is 
	// set to true, the caller should delete the doc. Otherwise, it 
	// should not.

	if (!mSysLogOn) 
	{
		need_delete = true;
		return true;
	}

	mLock->lock();
	need_delete = false;
	mBuff->setU32((u32)eDeleteDoc);
	mBuff->setU64(dfmid);
	mBuff->setU64(docid);
	mBuff->setData(header, header_len);
	if (mBuff->dataLen() > mMaxDataLen) saveLogs(rdata);
	mLock->unlock();
	return true;
}


bool
AosSystemLog::docModified(
		const u64 &dfmid,
		const u64 &docid, 
		bool &need_modify,
		const char *header,
		const int header_len,
		const AosRundataPtr &rdata)
{
	// A doc is modified. It checks whether the doc was modified already.
	// If yes, it sets 'need_modify' to true and returns true. Otherwise, 
	// it sets 'need_modify' to false, and create a log entry:
	// 	[eModifyDoc, dfmid, docid, header]
	if (!mSystLogOn) 
	{
		need_modify = true;
		return true;
	}

	mLock->lock();
	need_modify = false;
	mBuff->setU32((u32)eModifyDoc);
	mBuff->setU64(dfmid);
	mBuff->setU64(docid);
	mBuff->setData(header, header_len);
	if (mBuff->dataLen() > mMaxDataLen) saveLogs(rdata);
	mLock->unlock();
	return true;
}


bool
AosSystemLog::createSnapshot(
		const docfilemgr_id,
		const OmnString &name, 
		const AosRundataPtr &rdata)
{
	// This function creates a new snapshot. It checks whether the snapshot
	// has been created. If yes, it reports an error return returns.
	// Otherwise, it creates a snapshot, which means that it will 
	// create a doc and a log entry:
	// 	[eSnapshot, docfilemgr_id, name]
	mLock->lock();
	if (mBuff->dataLen() > 0) saveLogs(rdata);
	aos_assert_rl(mBuff->dataLen() == 0, mLock, false);

	mBuff->setU32((u32)eCreateSnapshot);
	mBuff->setOmnStr(name);
	return true;
}


bool
AosSystemLog::deleteSnapshot(
		const u64 &docfilemgr_id,
		const OmnString &name, 
		const AosRundataPtr &rdata)
{
	// This function removes a snapshot. If the snapshot does not exist, 
	// it is an error. Otherwise, it removes all the entries for the 
	// snapshot.
	OmnNotImplementedYet;
	return false;
}


bool
AosSystemLog::restoreSnapshot(
		const u64 &docfilemgr_id,
		const OmnString &name, 
		const AosRundataPtr &rdata)
{
	// This function restores the system from the current point to the
	// specified snapshot. If the snapshot does not exist, it is an error.
	// Otherwise, it processes the entries one by one until the snapshot
	// is recovered.
	OmnNotImplementedYet;
	return false;
}


OmnFilePtr
AosSystemLog::openLogFile(
		const u64 &dfmid,
		const OmnString &snapshot_name, 
		const AosRundataPtr &rdata)
{
	OmnString filekey = createFilekey(dfmid, snapshot_name);
	aos_assert_rr(filekey.length() > 0, rdata, 0);
	OmnFilePtr file = AosOpenStorageFile(mVirtualId, filekey, true, rdata AosMemoryCheckerFile);
	aos_assert_rr(file, rdata, 0);
	return file;
}

