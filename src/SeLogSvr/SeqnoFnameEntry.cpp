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
// Modification History:
// 09/04/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeLogSvr/SeqnoFnameEntry.h"

#include "SEInterfaces/VfsMgrObj.h"
#include "SeLogSvr/MdlLogSvr.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "UtilHash/HashedObj.h"

AosSeqnoFnameEntry::AosSeqnoFnameEntry()
:
mStatus(AosMdlLogSvr::eInvalidFlag),
mSeqno(0),
mFileId(0)
{
}


AosSeqnoFnameEntry::AosSeqnoFnameEntry(
		const OmnString &key, 
		const u32 seqno,
		const u64 &file_id)
:
AosHashedObj(key, AosHashedObj::eSeqnoFnameEntry),
mStatus(AosMdlLogSvr::eActiveFlag),
mSeqno(seqno),
mFileId(file_id)
{
}


bool 
AosSeqnoFnameEntry::isSame(const AosHashedObjPtr &rhs) const
{
	aos_assert_r(rhs, false);
	if (rhs->getType() != eSeqnoFnameEntry) return false;
	AosSeqnoFnameEntry *rr = (AosSeqnoFnameEntry *)(rhs.getPtr());
	OmnString rhsKey = rr->getKeyStr();
	u64 file_id = rr->getFileId();
	return mKey == rhsKey && mFileId == file_id;
}


int
AosSeqnoFnameEntry::doesRecordMatch(
		AosBuff &buff,
		const OmnString &key_in_buff,
		const OmnString &expected_key,
		AosHashedObjPtr &obj)
{
	// The format is:
	// 		filename	(string)
	//		file seqno	(u64)
	//		crt_seqno	(u32)
	int idx = buff.getCrtIdx();
	if (key_in_buff != expected_key) 
	{
		obj = 0;
		return buff.getCrtIdx() - idx;
	}
	u64 filesqno = buff.getU64(0);
	u32 crtSeqno = buff.getU32(0);
	obj = OmnNew AosSeqnoFnameEntry(expected_key, crtSeqno, filesqno);
	return buff.getCrtIdx() - idx;
}


int
AosSeqnoFnameEntry::setContentsToBuff(AosBuff &buff)
{
	// The format is:
	//		file seqno	(u64)
	//		crt_seqno	(u32)
	int idx = buff.getCrtIdx();
	buff.setU64(mFileId);
	buff.setU32(mSeqno);
	return buff.getCrtIdx() - idx;
}


AosHashedObjPtr 
AosSeqnoFnameEntry::clone()
{
	return OmnNew AosSeqnoFnameEntry();
}


bool 
AosSeqnoFnameEntry::reset() 
{
	mKey = "";
	mFileId = 0;
	return true;
}


u32
AosSeqnoFnameEntry::getNextSeqno()
{
	mSeqno++;
	u32 seqno = mSeqno;
	if (mSeqno >= 0xffffffff)
	{
		mSeqno = AosMdlLogSvr::eInitialSeqno;
	}
	return seqno;
}


void 
AosSeqnoFnameEntry::setDeleteFlag()
{
	mStatus = AosMdlLogSvr::eDeletedFlag;
}


OmnFilePtr
AosSeqnoFnameEntry::openFile(const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	if (mFile) return mFile;

	try
	{
		AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
		aos_assert_r(vfsMgr, 0);
		
		mFile = (OmnFile*)vfsMgr->openRawFile(mFileId,  rdata.getPtr()).getPtr();
		if (mFile->isGood()) return mFile;
		mFile = 0;
	}

	catch (...)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateFile);
		OmnAlarm << rdata->getErrmsg() << ". File ID: " << mFileId << enderr;
		return 0;
	}

	AosSetError(rdata, AosErrmsgId::eFailedOpenFile);
	OmnAlarm << rdata->getErrmsg() << ". File ID: " << mFileId << enderr;
	return 0;
}

