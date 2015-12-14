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
// 08/15/2013 	Created by Ketty 
// the same as TransIdMgr.
////////////////////////////////////////////////////////////////////////////
#include "FmtMgr/FmtIdMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/File.h"


AosFmtIdMgr::AosFmtIdMgr(const OmnString &dir_name) 
:
mLock(OmnNew OmnMutex())
{
	mFname = dir_name;
	if(mFname != "")	mFname << "/";
	mFname << "FmtId";
	
	mFile = OmnNew OmnFile(mFname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!mFile || !mFile->isGood())
	{
		mFile = OmnNew OmnFile(mFname, OmnFile::eCreate AosMemoryCheckerArgs);
		if(!mFile || !mFile->isGood())
		{
			OmnAlarm << "Failed to open file: " << mFname << enderr;
		}
	}
}


AosFmtIdMgr::~AosFmtIdMgr()
{
}


bool
AosFmtIdMgr::init()
{
	mFile->lock();
	u32 offset = 0;
	u64 seq_in_file = 0;
	
	if(mFile->getLength() != 0)
	{
		seq_in_file = mFile->readBinaryU32(offset, 0);
		aos_assert_r(seq_in_file, false);
	}

	// 0 is error check.
	mNextFmtId = seq_in_file + 1;
	mNumFmtId = eNumFmtId;
	u64 new_seq = seq_in_file + eNumFmtId;

	mFile->setU64(offset, new_seq, true);  
	mFile->unlock();
	OmnScreen << "FmtIdMgr; saveFmtId:" << new_seq << endl;
	return true;
}


u64
AosFmtIdMgr::nextFmtId()
{
	mLock->lock();
	aos_assert_rl(mNextFmtId != 0, mLock, 0);

	u64 fmt_id = mNextFmtId++;
	mNumFmtId--;

	if(mNumFmtId == 0)
	{
		loadNewFmtId(fmt_id);
		mNumFmtId = eNumFmtId;
	}
	mLock->unlock();

	return fmt_id;
}


u64
AosFmtIdMgr::nextSendFmtId()
{
	mLock->lock();
	u64 fmt_id = mNextFmtId;
	mLock->unlock();
	return fmt_id;
}


bool
AosFmtIdMgr::loadNewFmtId(const u64 crt_id)
{
	mFile->lock();

	u32 offset = 0;
	u64 seq_in_file = mFile->readBinaryU32(offset, 0);
	aos_assert_r(seq_in_file, false);

	if(crt_id != seq_in_file)
	{
		OmnAlarm << "trans seq id mismatch: " << crt_id
				<< ":" << seq_in_file << enderr;
	}

	u64 new_id = seq_in_file + eNumFmtId;
	mFile->setU64(offset, new_id, true);  
	mFile->unlock();
	OmnScreen << "FmtIdMgr; saveFmtId:" << new_id << endl;

	return true;
}


bool
AosFmtIdMgr::resetFmtId(const u64 fmt_id)
{
	// this func is called. when master is switchToMaster. 
	// this fmt_id is created by bkp.
	// the same as init() func.
	mLock->lock();
	
	mNextFmtId = fmt_id;
	mNumFmtId = eNumFmtId;
	u64 seq_in_file = mNextFmtId - 1;
	saveNewFmtId(seq_in_file + eNumFmtId);
	
	mLock->unlock();
	return true;
}

bool
AosFmtIdMgr::saveNewFmtId(const u64 new_id)
{
	mFile->lock();
	
	u32 offset = 0;
	u64 seq_in_file = mFile->readBinaryU32(offset, 0);
	aos_assert_r(seq_in_file, false);

	if(seq_in_file > new_id)
	{
		OmnAlarm << "seq_in file is bigger: " << new_id 
				<< ":" << seq_in_file << enderr;
	}
	
	mFile->setU64(offset, new_id, true);  
	mFile->unlock();
	OmnScreen << "FmtIdMgr; saveFmtId:" << new_id << endl;

	return true;
}


