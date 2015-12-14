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
// 04/08/2013	Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "DocFileMgr/DfmDiskFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h" 
#include "DocFileMgr/BodyFile.h"
#include "ReliableFile/ReliableFile.h"
#include "Util/Buff.h" 

AosDfmDiskFile::AosDfmDiskFile(const bool regflag)
:
AosDfmFile(AosDfmFileType::eDiskFile, regflag)
{
}

AosDfmDiskFile::AosDfmDiskFile(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u32 block_size,
		const u32 vid,
		const OmnString &prefix,
		const u64 max_fsize)
:
AosDfmFile(rdata, AosDfmFileType::eDiskFile, seqno, vid, prefix, max_fsize),
mBlockSize(block_size)
{
}

AosDfmDiskFile::~AosDfmDiskFile()
{
}

	
AosDfmFilePtr
AosDfmDiskFile::clone()
{
	return OmnNew AosDfmDiskFile(false);
}


bool
AosDfmDiskFile::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDfmFile::serializeFrom(buff); 
	aos_assert_r(buff && rslt, 0);
	
	mBlockSize = buff->getU32(0);
	return true;
}


bool
AosDfmDiskFile::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDfmFile::serializeTo(buff); 
	aos_assert_r(buff && rslt, 0);
	
	buff->setU32(mBlockSize);
	return true;
}


AosBodyFilePtr
AosDfmDiskFile::getDiskFile(const AosRundataPtr &rdata)
{
	mLock->lock();
	if(!mDiskFile)
	{
		AosReliableFilePtr ff = getFileLocked(rdata);
		aos_assert_r(ff, 0);
		if(mIsNew)
		{
			mDiskFile = OmnNew AosBodyFile(ff, mBlockSize, mBlockSize, rdata);
		}
		else
		{
			//mDiskFile = OmnNew AosBodyFile(ff, rdata);
			AosBodyFilePtr disk_file = OmnNew AosBodyFile(ff, rdata);            
			// Ketty 2014/01/08
			if(!disk_file || disk_file->getBlockSize() == 0)
			{
			    // maybe this body has damaged. 
			    mLock->unlock();
			    OmnAlarm << "error! file error! " << ff->getFileName() << enderr;
			    return 0;
			}
			mDiskFile = disk_file;
		}
	}

	aos_assert_rl(mDiskFile, mLock, 0);
	mLock->unlock();
	return mDiskFile;
}

// Ketty 2014/02/21
bool
AosDfmDiskFile::closeFile()
{
	if(!mDiskFile)	return true;
	mLock->lock();
	mDiskFile->closeFile();
	mDiskFile = 0;
	mLock->unlock();
	return true;
}

