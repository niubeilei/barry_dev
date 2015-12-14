////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
//
// Modification History:
// 01/24/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DiskBlockMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocFileMgr/DiskBlock.h"
#include "DocFileMgr/DiskBlockCache.h"
#include "DocFileMgr/BodyFile.h"
#include "DocFileMgr/DfmDiskFile.h"
#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "StorageMgr/SystemId.h"
#include "SEUtil/SeConfig.h"


AosDiskBlockMgr::AosDiskBlockMgr(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		vector<AosDfmFilePtr> &files,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mVirtualId(v_id),
mDfmId(dfm_id),
mPrefix(prefix),
mMaxSeqno(-1),
mShowLog(show_log)
{
	for(u32 i=0; i<files.size(); i++)
	{
		if(files[i]->getType() != AosDfmFileType::eDiskFile) continue;
		
		AosDfmDiskFilePtr dfm_file = (AosDfmDiskFile *)files[i].getPtr();
		u32 seqno = dfm_file->getFileSeqno();       
		mFiles.insert(make_pair(seqno, dfm_file));
		if((int)seqno > mMaxSeqno) mMaxSeqno = seqno;
	}
	
	initActiveBlock();
}
	

AosDiskBlockMgr::~AosDiskBlockMgr()
{
}


bool
AosDiskBlockMgr::stop(const AosRundataPtr &rdata)
{
	//saveSlabFileBitmap(seqno, rdata);                   
	//mCrtDiskBlock[crt_saveidx]->saveToDiskPublic(rdata);
	return AosDiskBlockCache::getSelf()->stop(mDfmId, mVirtualId, rdata);
}


bool
AosDiskBlockMgr::initActiveBlock()
{
	memset(mActiveSeqno, eInvalidSeqno, sizeof(u32) * (AosDfmUtil::eMaxBlockSizes));
	if(mMaxSeqno < 0)	return true;	// this means there is no doc	
	
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(AOS_SYS_SITEID);
	AosReliableFilePtr file;
	u32 finish = 0;
	int index = mMaxSeqno;

	mLock->lock();
	while(index-- || finish == AosDfmUtil::eMaxBlockSizes)
	{
		u32 seqno = (u32)index;
		AosBodyFilePtr body_file = openBodyFilePriv(seqno, rdata);	
		//Ketty 2014/02/12
		//aos_assert_rl(body_file, mLock, false);
		if(!body_file)                                                
		{                                                             
			// maybe this bodyFile has error!                         
			OmnScreen << "maybe this bodyFile has error: " <<  mPrefix
				<< "_" << seqno << endl;                              
			continue;                                                 
		}

		file = body_file->getFile();
		if (!file || !file->isGood())
		{
			OmnScreen << "Failed opening file: " <<  mPrefix
			 	<< "_" << seqno << endl;
			continue;
		}
		
		u32 block_size = body_file->getBlockSize();
		int size_idx = AosDfmUtil::blocksize2SizeIndex(block_size);
		aos_assert_rl(size_idx >=0, mLock, false);
		
		if(mActiveSeqno[size_idx] != eInvalidSeqno)
		{
			mActiveSeqno[size_idx] = seqno;
			finish++;
			continue;	
		}
	}
	mLock->unlock();	
	return true;
}


bool
AosDiskBlockMgr::addDoc(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno,
		u64 &offset,
		const u32 orig_len,
		const u32 compress_len,
		const char *data)
{
	u32 data_len = compress_len ? compress_len : orig_len;
	u32 block_size = AosDfmUtil::docsize2Blocksize(data_len);
	AosDiskBlock2Ptr block = findActiveBlock(dfm, seqno, block_size, rdata);
	aos_assert_r(block, false);

	offset = block->addRcd(rdata, data, orig_len, compress_len);
	if(offset <=0)
	{
		// means this block has full. need find a empty block.
		AosDiskBlockCache::getSelf()->returnBlock(block);

		block = getEmptyBlock(dfm, seqno, block_size, rdata);
		aos_assert_r(block, false);

		offset = block->addRcd(rdata, data, orig_len, compress_len);
		aos_assert_r(offset, false);
	}
	block->saveToDiskPublic(rdata);
	AosDiskBlockCache::getSelf()->returnBlock(block);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; disk. add finish:"
			<< "; seqno:" << seqno 
			<< "; offset:" << offset 
			<< endl;
	}
	return true;
}


bool
AosDiskBlockMgr::modifyDoc(
		const AosRundataPtr &rdata,
		const u32 &seqno,
		u64 &offset,
		const u32 orig_len,
		const u32 compress_len,
		const char *data,
		bool &need_kickout)
{
	AosDiskBlock2Ptr block = getDiskBlock(rdata, seqno, offset, false);
	aos_assert_r(block, false);

	bool rslt = block->modifyRcd(rdata, offset, data,
		      need_kickout, orig_len, compress_len);
	aos_assert_r(rslt, false);
	
	block->saveToDiskPublic(rdata);
	AosDiskBlockCache::getSelf()->returnBlock(block);
	return true;
}


bool
AosDiskBlockMgr::removeDoc(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u64 offset)
{
	AosDiskBlock2Ptr block = getDiskBlock(rdata, seqno, offset, false);
	aos_assert_rr(block, rdata, false);
	bool rslt = block->deleteRcd(rdata, offset);
	aos_assert_r(rslt, false);
	    
	block->saveToDiskPublic(rdata);
	AosDiskBlockCache::getSelf()->returnBlock(block);
	return true;
}


bool
AosDiskBlockMgr::readDoc(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u64 offset,
		AosBuffPtr &data_buff,
		u32 &orig_len,
		u32 &compress_len)
{
	AosDiskBlock2Ptr block = getDiskBlock(rdata, seqno, offset, false);
	aos_assert_rr(block, rdata, false);

	bool rslt = block->readRcd(offset, data_buff, orig_len, compress_len);
	AosDiskBlockCache::getSelf()->returnBlock(block);
	aos_assert_r(rslt, false);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; readFrom DiskBlock, readBody:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; orig_len:" << orig_len
			<< "; compress_len:" << compress_len
			<< endl;
	}
	
	return true;
}


AosDiskBlock2Ptr
AosDiskBlockMgr::getDiskBlock(
		const AosRundataPtr &rdata,
		const AosDiskBlock2::BlockId &block_id,
		const bool create_new)
{
	mLock->lock();
	AosBodyFilePtr body_file = openBodyFilePriv(block_id.fileSeqno, rdata);
	mLock->unlock();

	aos_assert_rr(body_file, rdata, 0);
	AosReliableFilePtr ff = body_file->getFile();
	aos_assert_r(body_file->getBlockSize() == block_id.blockSize, 0);

	// Ketty 2014/02/21
	AosDiskBlockMgrPtr thisptr(this, false);
	AosDiskBlock2Ptr block = AosDiskBlockCache::getSelf()->getBlock2(rdata, block_id, ff, create_new, thisptr);
	aos_assert_rr(block, rdata, 0);
	return block;
}


AosDiskBlock2Ptr
AosDiskBlockMgr::getDiskBlock(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u64 offset,
		const bool create_new)
{
	AosDiskBlock2::BlockId block_id = composeBlockId(seqno, offset, rdata);
	aos_assert_r(block_id.blockSize != 0, 0);
	AosDiskBlock2Ptr block = getDiskBlock(rdata, block_id, false);
	return block;
}


AosDiskBlock2Ptr
AosDiskBlockMgr::findActiveBlock(
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno, 
		const u32 block_size,
		const AosRundataPtr &rdata)
{
	// This function checks whether the current file 'seqno'
	// has an active block. If yes, it creates the block and
	// returns the block. A file has an active block if the
	// current block start (eOffsetBlockStart) + block size
	// is smaller than the max file size.
	//if (mActiveBlocks[size_idx].getPtr()) 
	
	int size_idx = AosDfmUtil::blocksize2SizeIndex(block_size);
	aos_assert_r(size_idx >= 0 && (u32)size_idx < AosDfmUtil::eMaxBlockSizes, 0);
	
	bool createNew = false;
	AosBodyFilePtr body_file;
	mLock->lock();
	if (mActiveSeqno[size_idx] != eInvalidSeqno) 
	{
		seqno = mActiveSeqno[size_idx];
		body_file = openBodyFilePriv(seqno, rdata);
		createNew = false;
	}
	else
	{
		//it happens when there is no any file of this blocksize.  will create a new file,	
		body_file = createBodyFilePriv(dfm, seqno, block_size, rdata);
		aos_assert_rl(body_file, mLock, 0);
		mActiveSeqno[size_idx] = seqno;
		createNew = true;
	}
	//mLock->unlock();

	aos_assert_rl(body_file, mLock, 0);
	AosReliableFilePtr ff = body_file->getFile();
	u32 blockstart = body_file->getLastBlock();
	
	AosDiskBlock2::BlockId block_id = {mVirtualId, mDfmId, seqno, blockstart, block_size};
	// Ketty 2014/02/21
	AosDiskBlockMgrPtr thisptr(this, false);
	AosDiskBlock2Ptr block = AosDiskBlockCache::getSelf()->getBlock2(
			rdata, block_id, ff, createNew, thisptr);
	aos_assert_rl(block, mLock, 0);
	
	mLock->unlock();
	return block;
	
}


AosDiskBlock2Ptr
AosDiskBlockMgr::getEmptyBlock(
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno,
		u32 block_size,
		const AosRundataPtr &rdata)
{
	// It closes the current block and creates the next block, if possible.
	// Otherwise, it looks for a new one.
	int size_idx = AosDfmUtil::blocksize2SizeIndex(block_size);
	aos_assert_r(size_idx >= 0 && (u32)size_idx < AosDfmUtil::eMaxBlockSizes, 0);
	aos_assert_r(mActiveSeqno[size_idx] != eInvalidSeqno, 0);
	
	mLock->lock();
	AosBodyFilePtr body_file = openBodyFilePriv(mActiveSeqno[size_idx], rdata);
	aos_assert_rl(body_file, mLock, 0);

	u32 blockstart = body_file->appendBlock(rdata);
	aos_assert_rl(blockstart > 0, mLock, 0);

	// Chen Ding, 10/15/2010
	if (blockstart + block_size > eAosMaxFileSize)
	{
		//it will create a new file,
		body_file = createBodyFilePriv(dfm, seqno, block_size, rdata);
		aos_assert_rl(body_file, mLock, 0);
		
		OmnScreen << "Create a new file: " << seqno << ":" << block_size << endl;
		blockstart = body_file->getLastBlock();
		
		mActiveSeqno[size_idx] = seqno;
	}
	else
	{
		seqno = mActiveSeqno[size_idx];
	}
	//mLock->unlock();
	
	AosReliableFilePtr ff = body_file->getFile();
	aos_assert_rl(ff, mLock, 0);
	
	AosDiskBlock2::BlockId block_id = {mVirtualId, mDfmId, seqno, blockstart, block_size};
	// Ketty 2014/02/21
	AosDiskBlockMgrPtr thisptr(this, false);
	AosDiskBlock2Ptr new_block = AosDiskBlockCache::getSelf()->getBlock2(
			rdata, block_id, ff, true, thisptr);
	aos_assert_rl(new_block, mLock, 0);

	mLock->unlock();
	return new_block;
}

AosDiskBlock2::BlockId
AosDiskBlockMgr::composeBlockId(const u32 fseqno, const u32 foffset, const AosRundataPtr &rdata)
{
	mLock->lock();
	AosBodyFilePtr body_file = openBodyFilePriv(fseqno, rdata);
	mLock->unlock();
	AosDiskBlock2::BlockId invalid = {0, 0, 0, 0, 0};
	aos_assert_rr(body_file, rdata, invalid);

	//block_id = AosDiskBlock2::composeBlockId(mVirtualId, mId, fseqno, foffset, body_file->getBlockSize(), true);
	u32 block_size = body_file->getBlockSize();
	aos_assert_r(block_size, invalid);
	u32 block_start = (foffset/block_size) * block_size;;
	AosDiskBlock2::BlockId block_id = {mVirtualId, mDfmId, fseqno, block_start, block_size};
	return block_id;
}


AosBodyFilePtr
AosDiskBlockMgr::openBodyFilePriv(
		const u32 seqno, 
		const AosRundataPtr &rdata) 
{
	AosBodyFilePtr disk_ff;
	FileMapItr itr = mFiles.find(seqno);
	if(itr != mFiles.end())
	{
		AosDfmDiskFilePtr dfm_file = itr->second;
		disk_ff = dfm_file->getDiskFile(rdata);
	}

	return disk_ff;
}


AosBodyFilePtr
AosDiskBlockMgr::createBodyFilePriv(
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno, 
		const u32 blocksize, 
		const AosRundataPtr &rdata) 
{
	seqno = (++mMaxSeqno);
	
	AosDfmDiskFilePtr dfm_file = OmnNew AosDfmDiskFile(rdata, 
			seqno, blocksize, mVirtualId, mPrefix, eAosMaxFileSize);
	aos_assert_r(dfm_file, 0);

	mFiles.insert(make_pair(seqno, dfm_file));
	dfm->addBodyFile(rdata, dfm_file);
	
	AosBodyFilePtr disk_ff = dfm_file->getDiskFile(rdata);
	return disk_ff;
}

// Ketty 2014/02/21
bool
AosDiskBlockMgr::closeBodyFile(
		const u32 file_seq,
		const u64 file_id)
{
	AosBodyFilePtr disk_ff;
	FileMapItr itr = mFiles.find(file_seq);
	if(itr != mFiles.end())
	{
		AosDfmDiskFilePtr dfm_file = itr->second;
		aos_assert_r(dfm_file->getFileId() == file_id, false);
		return dfm_file->closeFile();
	}
	
	OmnShouldNeverComeHere;
	return false;
}


