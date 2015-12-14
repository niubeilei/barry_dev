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
// 01/24/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DiskBlockMgr_h
#define AOS_DocFileMgr_DiskBlockMgr_h

#include "DocFileMgr/DfmUtil.h"
#include "DocFileMgr/DiskBlock.h"
#include "DocFileMgr/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"

#include <vector>
#include <map>
using namespace std;

class AosDiskBlockMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

	typedef map<u32, AosDfmDiskFilePtr> FileMap;
	typedef map<u32, AosDfmDiskFilePtr>::iterator FileMapItr;

	enum
	{
		eInvalidSeqno = 0xffffffff,
	};

private:
	OmnMutexPtr		mLock;

	u32				mVirtualId;
	u32				mDfmId;
	OmnString		mPrefix;

	int				mMaxSeqno;
	FileMap        	mFiles;
	u32             mActiveSeqno[AosDfmUtil::eMaxBlockSizes];
	
	bool			mShowLog;

public:
	AosDiskBlockMgr(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		vector<AosDfmFilePtr> &files,
		const bool show_log);
	~AosDiskBlockMgr();
	
	bool 	stop(const AosRundataPtr &rdata);

	bool    addDoc(const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				u32 &seqno,
				u64 &offset,
				const u32 orig_len,
				const u32 compress_len,
				const char *data);

	bool 	modifyDoc(const AosRundataPtr &rdata,
				const u32 &seqno,
				u64 &offset,
				const u32 orig_len,
				const u32 compress_len,
				const char *data,
				bool &need_kickout);
	
	bool 	removeDoc(const AosRundataPtr &rdata,
				const u32 seqno,
				const u64 offset);

	bool 	readDoc(
				const AosRundataPtr &rdata,
				const u32 seqno,
				const u64 offset,
				AosBuffPtr &data_buff,
				u32 &orig_len,
				u32 &compress_len);

	// Ketty 2014/02/21
	bool	closeBodyFile(
				const u32 file_seq,
				const u64 file_id);

private:
	bool	initActiveBlock();
	
	AosDiskBlock2Ptr 	getDiskBlock(
							const AosRundataPtr &rdata,
							const AosDiskBlock2::BlockId &block_id, 
							const bool create_new);
	AosDiskBlock2Ptr 	getDiskBlock(
							const AosRundataPtr &rdata,
							const u32 seqno,
							const u64 offset,
							const bool create_new);
	AosDiskBlock2Ptr	findActiveBlock(
							const AosDocFileMgrObjPtr &dfm,
							u32 &seqno, 
							const u32 block_size, 
							const AosRundataPtr &rdata);
	AosDiskBlock2Ptr	getEmptyBlock(
							const AosDocFileMgrObjPtr &dfm,
							u32 &seqno, 
							const u32 block_size, 
							const AosRundataPtr &rdata);
	AosDiskBlock2::BlockId  composeBlockId(const u32 fseqno, const u32 foffset, const AosRundataPtr &rdata);

	AosBodyFilePtr     openBodyFilePriv(const u32 seqno, const AosRundataPtr &rdata);
	AosBodyFilePtr     createBodyFilePriv(
			const AosDocFileMgrObjPtr &dfm,
			u32 &seqno, 
			const u32 blocksize, 
			const AosRundataPtr &rdata);
	

};
#endif

