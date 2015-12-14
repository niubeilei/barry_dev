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
#ifndef AOS_DocFileMgr_DiskBlockCache_h
#define AOS_DocFileMgr_DiskBlockCache_h

#include "DocFileMgr/DfmUtil.h"
#include "DocFileMgr/DiskBlock.h"
#include "DocFileMgr/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include <vector>
#include <map>
using namespace std;

OmnDefineSingletonClass(AosDiskBlockCacheSingleton,
		                AosDiskBlockCache,
						AosDiskBlockCacheSelf,
						OmnSingletonObjId::eDiskBlockCache,
						"DiskBlockCache");

class AosDiskBlockCache : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	//typedef hash_map<BlockId, AosDiskBlock2Ptr, u64_hash, u64_cmp> BlockMap_t;
	//typedef hash_map<BlockId, AosDiskBlock2Ptr, u64_hash, u64_cmp>::iterator BlockMapItr_t;
	typedef map<AosDiskBlock2::BlockId, AosDiskBlock2Ptr> BlockMap_t;
	typedef map<AosDiskBlock2::BlockId, AosDiskBlock2Ptr>::iterator BlockMapItr_t;
	typedef vector<AosDiskBlock2Ptr> Blocks_t;

	OmnMutexPtr			mLock;
	BlockMap_t			mBlockMap;
	Blocks_t			mBlocks[AosDfmUtil::eMaxBlockSizes];
	AosDiskBlock2Ptr	mIdleBlocks[AosDfmUtil::eMaxBlockSizes];

public:
	AosDiskBlockCache();
	~AosDiskBlockCache();

	// Singleton class interface
	static AosDiskBlockCache* getSelf();
	virtual bool            start();
	virtual bool            stop();
	virtual bool            config(const AosXmlTagPtr &def);

	bool 	stop(
			const u32 dfm_id, 
			const u32 virtual_id,
			const AosRundataPtr &rdata);

	AosDiskBlock2Ptr getBlock2(
			const AosRundataPtr &rdata,
			const AosDiskBlock2::BlockId &block_id,
			const AosReliableFilePtr &rfile,
			const bool createnew,
			const AosDiskBlockMgrPtr &diskblock_mgr);
	/*
	AosDiskBlock2Ptr getBlock2(
			const AosRundataPtr &rdata,
			const u32 virtual_id,
			const u32	&docFileMgrId,
			//const OmnFilePtr &file, 
			const AosReliableFilePtr &file, 
			const u32 seqno, 
			const u32 offset, 
			const u32 blocksize,
			const bool createnew);

	AosDiskBlock2Ptr getBlock(
			const AosRundataPtr &rdata,
			const u32 virtual_id,
			const u32 docFileMgrId,
			//const OmnFilePtr &file, 
			const AosReliableFilePtr &file, 
			const u32 seqno, 
			const u32 offset, 
			const u32 blocksize,
			const bool createnew,
			const OmnString &fname, 
			const int lineno)
	{
		return getBlock2(rdata, virtual_id, docFileMgrId, file, seqno, offset, blocksize, createnew);
	}
	*/

	bool returnBlock(const AosDiskBlock2Ptr &block);

	inline static u64 calculateBlockId(
			const u32 filemgrid, 
			const u32 seqno,
			const u32 blockstart)
	{
		return (u64((filemgrid << 16) | seqno) << 32) | blockstart;
	}

	/*
	// Ketty 2012/09/22
	bool isNeedKickOut( const AosRundataPtr &rdata,
			const u32 virtual_id,
			const u32 &dfm_id,
			const AosReliableFilePtr &file, 
			const u32 seqno, 
			const u32 offset, 
			const u32 blocksize,
			const u32 new_docsize,
			bool &need_kickout);
	*/

private:
	AosDiskBlock2Ptr getIdleBlockPriv(const u32 size_idx);
	bool idleListSanityCheck();
	bool removeFromIdlePriv(const AosDiskBlock2Ptr &block, const int size_idx);

	// Ketty 2014/02/21
	bool tryCloseBodyFileLocked(
			const AosDiskBlockMgrPtr &diskblock_mgr,
			const AosDiskBlock2Ptr &deleted_block,
			Blocks_t	&all_blocks);

};
#endif

