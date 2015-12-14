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
#ifndef AOS_DocFileMgr_DfmFile_h
#define AOS_DocFileMgr_DfmFile_h

#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmUtil.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

class AosDfmFile: public OmnRCObject 
{
	OmnDefineRCObject;

protected:
	OmnMutexPtr		mLock;
	bool			mIsNew;


private:
	AosDfmFileType::E	mType;
	u32				mSeqno;
	u64 			mFileId;
	AosReliableFilePtr mFile;

public:
	AosDfmFile(const AosDfmFileType::E type,
			const bool regflag);
	AosDfmFile(
		const AosRundataPtr &rdata,
		const AosDfmFileType::E tp,
		const u32 seqno,
		const u32 vid,
		const OmnString prefix,
		const u64 max_fsize);
	~AosDfmFile();

	static AosDfmFilePtr serializeFromStatic(const AosBuffPtr &buff);
	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosDfmFilePtr clone() = 0;

	AosReliableFilePtr getFile(const AosRundataPtr &rdata);
	u32	getFileSeqno(){ return mSeqno; };
	u64	getFileId(){ return mFileId; };
	AosDfmFileType::E getType(){ return mType; };

protected:	
	AosReliableFilePtr getFileLocked(const AosRundataPtr &rdata);

private:
	bool registerDfmFile(const AosDfmFilePtr &dfm_file);

};

#endif
