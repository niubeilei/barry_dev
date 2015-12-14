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

#include "DocFileMgr/DfmFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h" 
#include "DocFileMgr/BodyFile.h"
#include "ReliableFile/ReliableFile.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h" 

extern AosDfmFilePtr 	sgDfmFiles[AosDfmFileType::eMax];

AosDfmFile::AosDfmFile(const AosDfmFileType::E type, const bool regflag)
:
mLock(OmnNew OmnMutex()),
mIsNew(false),
mType(type),
mSeqno(0),
mFileId(0)
{
	if(regflag)
	{
		AosDfmFilePtr thisptr(this, false);
		registerDfmFile(thisptr);
	}
}

AosDfmFile::AosDfmFile(
		const AosRundataPtr &rdata,
		const AosDfmFileType::E tp,
		const u32 seqno,
		const u32 vid,
		const OmnString prefix,
		const u64 max_fsize)
:
mLock(OmnNew OmnMutex()),
mIsNew(true),
mType(tp),
mSeqno(seqno)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);

	mFile = vfsMgr->createRlbFile(rdata.getPtr(), vid, prefix,
			max_fsize, AosRlbFileType::eLog, true);
	aos_assert(mFile);	
	mFileId = mFile->getFileId();
}

AosDfmFile::~AosDfmFile()
{
}


bool
AosDfmFile::registerDfmFile(const AosDfmFilePtr &dfm_file)
{
	AosDfmFileType::E type = dfm_file->getType();

	aos_assert_r(AosDfmFileType::isValid(type), false);
	aos_assert_r(!sgDfmFiles[type], false);
	sgDfmFiles[type] = dfm_file;	
	return true;

}


AosDfmFilePtr
AosDfmFile::serializeFromStatic(const AosBuffPtr &buff)
{
	aos_assert_r(buff, 0);
	
	AosDfmFileType::E tp = (AosDfmFileType::E)buff->getU32(0);
	aos_assert_r(AosDfmFileType::isValid(tp) && sgDfmFiles[tp], 0);

	AosDfmFilePtr dfm_file = sgDfmFiles[tp]->clone();
	dfm_file->serializeFrom(buff);
	if(dfm_file->mFileId == 0)
	{
		OmnScreen << "Error!!!! FileId error!" << endl;
		return 0;
	}

	return dfm_file;
}


bool
AosDfmFile::serializeFrom(const AosBuffPtr &buff)
{
	aos_assert_r(buff, 0);
	
	mSeqno = buff->getU32(0);
	mFileId = buff->getU64(0);
	//aos_assert_r(mFileId, false);
	
	return true;
}


bool
AosDfmFile::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, 0);
	
	buff->setU32(mType);
	buff->setU32(mSeqno);
	buff->setU64(mFileId);
	return true;
}


AosReliableFilePtr
AosDfmFile::getFile(const AosRundataPtr &rdata)
{
	mLock->lock();
	AosReliableFilePtr file = getFileLocked(rdata);
	mLock->unlock();
	
	return file;
}


AosReliableFilePtr
AosDfmFile::getFileLocked(const AosRundataPtr &rdata)
{
	if(mFile)	return mFile;
	
	if(!mFile)
	{
		aos_assert_r(mFileId, 0);
		AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
		aos_assert_r(vfsMgr, 0);
		mFile = vfsMgr->openRlbFile(mFileId,
			AosRlbFileType::eLog, rdata.getPtr());
	}

	aos_assert_r(mFile, 0);
	return mFile;
}


