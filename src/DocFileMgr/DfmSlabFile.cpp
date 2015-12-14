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

#include "DocFileMgr/DfmSlabFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h" 
#include "DocFileMgr/BodyFile.h"
#include "DocFileMgr/SlabBlockFile.h"
#include "ReliableFile/ReliableFile.h"
#include "Util/Buff.h" 

AosDfmSlabFile::AosDfmSlabFile(const bool regflag)
:
AosDfmFile(AosDfmFileType::eSlabFile, regflag)
{
}

AosDfmSlabFile::AosDfmSlabFile(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u32 blocksize,
		const u32 vid,
		const OmnString &prefix,
		const u64 max_fsize)
:
AosDfmFile(rdata, AosDfmFileType::eSlabFile, seqno, vid, prefix, max_fsize),
mBlockSize(blocksize)
{
}

AosDfmSlabFile::~AosDfmSlabFile()
{
}

	
AosDfmFilePtr
AosDfmSlabFile::clone()
{
	return OmnNew AosDfmSlabFile(false);
}


AosSlabBlockFilePtr
AosDfmSlabFile::getSlabFile(const AosRundataPtr &rdata)
{
	mLock->lock();
	if(!mSlabFile)
	{
		AosReliableFilePtr r_file = getFileLocked(rdata);
		aos_assert_r(r_file, 0);
		// Ketty 2014/09/02
		//if(mIsNew)
		if(mIsNew || r_file->getLength() == 0)
		{
			mSlabFile = OmnNew AosSlabBlockFile(r_file, mBlockSize, rdata);
		}
		else
		{
			//mSlabFile = OmnNew AosSlabBlockFile(r_file, rdata);
			AosSlabBlockFilePtr	slab_file = OmnNew AosSlabBlockFile(r_file, rdata); 
			// Ketty 2014/01/08
			if(!slab_file || slab_file->getBlockSize() == 0)
			{
			    // maybe this body has damaged. 
			    mLock->unlock();
			    OmnAlarm << "error! file error! " << r_file->getFileName() << enderr;
			    return 0;
			}
			mSlabFile = slab_file;
		}
	}
	aos_assert_rl(mSlabFile, mLock, 0);
	mLock->unlock();
	return mSlabFile;
}


