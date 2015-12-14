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
// 02/07/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_ReadOnlyBlock_h
#define AOS_DocFileMgr_ReadOnlyBlock_h

#include "Rundata/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <map>
#include <vector>
using namespace std;

struct AosReadOnlyBlock : virtual public OmnRCObject
{
	OmnDefineRCObject;
	enum
	{
		//eAosInvFseqno = 0xffffffff,
		eAosInvFseqno = 0,
		eAosInvFoffset = 0xffffffff,
		eMaxFileSize = 1000000000 //1G
	};

	typedef map<u32, AosDfmReadOnlyFilePtr> FileMap;
	typedef map<u32, AosDfmReadOnlyFilePtr>::iterator FileMapItr;

private:
	OmnMutexPtr			mLock;
	u32					mCrtSeqno;
	u64					mCrtLastOffset;
	AosReliableFilePtr	mFile;

	AosReliableFilePtr	mReadFile;
	u32 				mReadSeqno;

	u32                 mVirtualId;		// Ketty 2013/01/26
	u32                 mDfmId;
	OmnString           mPrefix;

	FileMap				mDfmFiles;
	bool				mShowLog;

public:
	AosReadOnlyBlock(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		vector<AosDfmFilePtr> &files,
		const bool show_log);
	~AosReadOnlyBlock();

	bool 	addDoc(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				u32 &seqno,
				u64 &offset,
				const char *data,
				const u32 orig_len,
				const u32 compress_len);
	bool	readDoc(
				const AosRundataPtr &rdata,
				const u32 seqno,
				const u64 offset,
				AosBuffPtr &data_buff,
				u32 &orig_len,
				u32 &compress_len);

	bool	removeDoc();

private:
	bool	createFileLocked(
				const AosDocFileMgrObjPtr &dfm,
				u32 &seqno,
				const AosRundataPtr &rdata);

	bool	openFileLocked(
				const u32 &seqno,
				const AosRundataPtr &rdata);
	bool	sanityCheck(const u32 blockId);
};
#endif
