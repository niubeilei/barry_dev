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
// This class manages all the disk blocks. 
// Blocks have different sizes. When one needs a block, it requests one 
// from this class. After finishing using it, the requester returns the
// block. 
//
// Block Management
// Blocks are managed based on block sizes (in the current implementations,
// there are only four block sizes). There are active blocks (being used
// by someone) and idle blocks (not used by anyone). Each size has a 
// maximum number of blocks to create. THERE SHALL NEVER BE THE CASE THAT
// THE CLASS FAILS ALLOCATING A BLOCK.
//
// When retrieving a block, it checks whether the block has already been
// loaded into memory. If yes, it will use it. If not, it checks whether
// it can create new blocks. If yes, it creates a new block. Otherwise, 
// it uses one of the idle blocks.
//
// A block is identified by:
// 		DocFileMgrID + Seqno + BlockStart
// It uses a hashmap mBlockMap to quickly determine whether a block is
// in memory or not.
//
// Modification History:
// 01/24/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DiskBlockCache.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocFileMgr/DiskBlock.h"
#include "DocFileMgr/DiskBlockMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosDiskBlockCacheSingleton,
		         AosDiskBlockCache,
				 AosDiskBlockCacheSelf,
				 "AosDiskBlockCache");

AosDiskBlockCache::AosDiskBlockCache()
:
mLock(OmnNew OmnMutex())
{
}


AosDiskBlockCache::~AosDiskBlockCache()
{
}

	
bool
AosDiskBlockCache::config(const AosXmlTagPtr &conf)
{
	return true;
}


bool
AosDiskBlockCache::start()
{
	return true;
}


bool
AosDiskBlockCache::stop()
{
	return true;
}

	
bool
AosDiskBlockCache::stop(
		const u32 dfm_id, 
		const u32 virtual_id,
		const AosRundataPtr &rdata)
{
	// A DocFileMgr is stopped. It needs to save all the blocks it has
	// used. 
	//
	// NOTE: in the current implementations, changes to blocks are saved
	// whenever changes were made. There is no need to do the saving. 
	//
	// When a docfilemgr stops, it should not use any blocks. This function
	// will do the checking.

	// Ketty. 2013/03/05.
	// This func is called when the main server a is up. need remove this disks who's 
	// in the server a from the cache. 
	bool rslt = true;
	mLock->lock();
	//mBlockMap.erase(block->getBlockId());
	
	BlockMapItr_t itr = mBlockMap.begin();
	while(itr != mBlockMap.end())
	//for(; itr != mBlockMap.end(); itr++)
	{
		AosDiskBlock2Ptr block = itr->second;
		AosDiskBlock2::BlockId block_id = block->getBlockId();
		
		if ( block_id.dfmId == dfm_id && block_id.virtualId == virtual_id)
		{
			BlockMapItr_t tmp = itr;
			itr++;
			mBlockMap.erase(tmp);	
		}
		else
		{
			itr++;
		}
	}

	for (int i=0; i<AosDfmUtil::eMaxBlockSizes; i++)
	{
		for (u32 k=0; k<mBlocks[i].size(); k++)
		{
			AosDiskBlock2Ptr block = mBlocks[i][k];
			AosDiskBlock2::BlockId block_id = block->getBlockId();
			
			//if (!block->isIdle() && (block->getBlockId()).dfmId == dfm_id)
			if ( block_id.dfmId == dfm_id && block_id.virtualId == virtual_id)
			{
				if(!block->isIdle())
				{
					OmnAlarm << "this block should idle!" << enderr;
				}
			}
		}
	}
	mLock->unlock();
	return rslt;
}


AosDiskBlock2Ptr
AosDiskBlockCache::getBlock2(
		const AosRundataPtr &rdata,
		const AosDiskBlock2::BlockId &block_id,
		const AosReliableFilePtr &rfile,
		const bool createnew,
		const AosDiskBlockMgrPtr &diskblock_mgr)
{
	int size_idx = AosDfmUtil::blocksize2SizeIndex(block_id.blockSize);
	AosDiskBlock2Ptr block;
	
	mLock->lock();
	BlockMapItr_t itr = mBlockMap.find(block_id);
	if(itr != mBlockMap.end())
	{
		// Found it. Add a reference to it. 
		block = itr->second;
		int ref = block->addReference();
		if (ref == 0)
		{
			// It is an idle block. Need to remove it from the idle
			bool rslt = removeFromIdlePriv(block, size_idx);
			aos_assert_rl(rslt, mLock, 0);
		}
		mLock->unlock();
		// OmnScreen << "Retrieved block (shared): size=" << size_idx << ":" << block.getPtr()
		//	<< ":" << block->getReference() << endl;
		return block;
	}

	// It did not find the block. Check whether can create an new distblock
	aos_assert_rl(AosDfmUtil::isSizeIdxValid(size_idx), mLock, NULL);
	if(mBlocks[size_idx].size() <= (u32)AosDfmUtil::getMaxBlocks(size_idx))
	{
		// It can create more
		//block = OmnNew AosDiskBlock2(rdata, virtual_id, docFileMgrId, file,
		//		seqno, blockStart, blocksize, createnew);
		block = OmnNew AosDiskBlock2(rdata, block_id, rfile, createnew);
		mBlocks[size_idx].push_back(block);
		block->addReference();
		mLock->unlock();
		// OmnScreen << "Retrieved block (new): size=" << size_idx << ":" << block.getPtr() 
		// 	<< ":" << block->getReference() << endl;
		return block;
	}

	// It cannot create more blocks. Will reuse an idle block.
	block = getIdleBlockPriv(size_idx);
	aos_assert_rl(block, mLock, NULL);
	aos_assert_rl(block->isIdle(), mLock, NULL);
	block->addReference();

	// Delete it from the map
	//u64 oldIndex = calculateBlockId(block->getDocFileMgrId(), 
	//		block->getSeqno(), block->getBlockStart());
	//AosDiskBlock2::BlockId oldIndex = {block->getVirtualId(), block->getDocFileMgrId(), block->getSeqno(), block->getBlockStart()};
	mBlockMap.erase(block->getBlockId());
	bool rslt = tryCloseBodyFileLocked(diskblock_mgr, block, mBlocks[size_idx]);
	aos_assert_rl(rslt, mLock, 0);

	// Reload the block
	//bool rslt = block->reload(rdata, virtual_id, docFileMgrId, file, seqno, blockStart, blocksize, createnew);
	rslt = block->reload(rdata, block_id, rfile, createnew);
	aos_assert_rl(rslt, mLock, 0);

	// Add it to the map
	mBlockMap[block_id] = block;
	mLock->unlock();
	// OmnScreen << "Retrieved block (reloaded): size=" << size_idx << block.getPtr() 
	//	<< ":" << block->getReference() << endl;
	return block;
	
}


bool
AosDiskBlockCache::tryCloseBodyFileLocked(
		const AosDiskBlockMgrPtr &diskblock_mgr,
		const AosDiskBlock2Ptr &deleted_block,
		Blocks_t	&all_blocks)
{
	// Ketty 2014/02/21
	
	u64 delete_file_id = deleted_block->getFileId();
	
	AosDiskBlock2Ptr check_block; 
	for(u32 i=0; i<all_blocks.size(); i++)
	{
		check_block = all_blocks[i];	
		if(check_block == deleted_block)	continue;
	
		// the same file.
		if(check_block->getFileId() == delete_file_id)	return true;
	}
	
	// need close
	u32 delete_file_seq = deleted_block->getBlockId().fileSeqno;
	diskblock_mgr->closeBodyFile(delete_file_seq, delete_file_id);
	return true;
}


/*
AosDiskBlock2Ptr
AosDiskBlockCache::getBlock2(
		const AosRundataPtr &rdata,
		const u32 virtual_id,
		const u32 &docFileMgrId,
		const AosReliableFilePtr &file, 
		const u32 seqno, 
		const u32 offset, 
		const u32 blocksize,
		const bool createnew)
{
	// This function checks whether the requested block is already in 
	// memory. If yes, use it. Otherwise, it checks whether it should 
	// create a new one. If yes, it will create a new one. Otherwise, 
	// it reuses one.
	aos_assert_r(blocksize > 0, 0);
	u32 blockStart = (offset/blocksize) * blocksize; 
	aos_assert_r(blockStart >= blocksize, NULL);

	mLock->lock();
	AosDiskBlock2Ptr block;
		
	int size_idx = AosDfmUtil::blocksize2SizeIndex(blocksize);
	BlockId index = {virtual_id, docFileMgrId, seqno, blockStart};
	//u64 index = calculateBlockId(docFileMgrId, seqno, blockStart);
	BlockMapItr_t itr = mBlockMap.find(index);
	if(itr != mBlockMap.end())
	{
		// Found it. Add a reference to it. 
		block = itr->second;
		int ref = block->addReference();
		if (ref == 0)
		{
			// It is an idle block. Need to remove it from the idle
			bool rslt = removeFromIdlePriv(block, size_idx);
			aos_assert_rl(rslt, mLock, 0);
		}
		mLock->unlock();
		// OmnScreen << "Retrieved block (shared): size=" << size_idx << ":" << block.getPtr()
		//	<< ":" << block->getReference() << endl;
		return block;
	}

	// It did not find the block. Check whether can create an new distblock
	aos_assert_rl(AosDfmUtil::isSizeIdxValid(size_idx), mLock, NULL);
	if(mBlocks[size_idx].size() <= (u32)AosDfmUtil::getMaxBlocks(size_idx))
	{
		// It can create more
		block = OmnNew AosDiskBlock2(rdata, virtual_id, docFileMgrId, file,
				seqno, blockStart, blocksize, createnew);
		mBlocks[size_idx].push_back(block);
		block->addReference();
		mLock->unlock();
		// OmnScreen << "Retrieved block (new): size=" << size_idx << ":" << block.getPtr() 
		// 	<< ":" << block->getReference() << endl;
		return block;
	}

	// It cannot create more blocks. Will reuse an idle block.
	block = getIdleBlockPriv(size_idx);
	aos_assert_rl(block, mLock, NULL);
	aos_assert_rl(block->isIdle(), mLock, NULL);
	block->addReference();

	// Delete it from the map
	//u64 oldIndex = calculateBlockId(block->getDocFileMgrId(), 
	//		block->getSeqno(), block->getBlockStart());
	BlockId oldIndex = {block->getVirtualId(), block->getDocFileMgrId(), block->getSeqno(), block->getBlockStart()};
	mBlockMap.erase(oldIndex);

	// Reload the block
	bool rslt = block->reload(rdata, virtual_id, docFileMgrId, file, seqno, blockStart, blocksize, createnew);
	aos_assert_rl(rslt, mLock, 0);

	// Add it to the map
	mBlockMap[index] = block;
	mLock->unlock();
	// OmnScreen << "Retrieved block (reloaded): size=" << size_idx << block.getPtr() 
	//	<< ":" << block->getReference() << endl;
	return block;
}
*/


bool
AosDiskBlockCache::returnBlock(const AosDiskBlock2Ptr &block)
{
	mLock->lock();
	aos_assert_rl(idleListSanityCheck(), mLock, false);
	block->removeReference();
	int size_idx = AosDfmUtil::blocksize2SizeIndex(block->getBlockSize());
	if (block->isIdle())
	{
		aos_assert_rl(AosDfmUtil::isSizeIdxValid(size_idx), mLock, false);
		if (!mIdleBlocks[size_idx])
		{
			block->setPrev(block);
			block->setNext(block);
			mIdleBlocks[size_idx] = block;
			aos_assert_rl(idleListSanityCheck(), mLock, false);
			//OmnScreen << "Return block(empty) size=" << size_idx << ":" 
			//	<< block.getPtr() << ":" << endl;
		}
		else
		{
			block->addToList(mIdleBlocks[size_idx]);
			mIdleBlocks[size_idx] = block;
			aos_assert_rl(idleListSanityCheck(), mLock, false);
			//OmnScreen << "Return block(head) size=" << size_idx 
			//	<< ":" << block.getPtr() << ":"
			//	<< ": next:" << block->getNext() << ":" 
			//	<< endl;
		}
	}
	else
	{
		// OmnScreen << "Return block(not idle) size=" << size_idx 
		//	<< ":" << block.getPtr() << ":" 
		//	<< block->getReference() << endl;
	}
	mLock->unlock();
	return true;
}

	
AosDiskBlock2Ptr
AosDiskBlockCache::getIdleBlockPriv(const u32 size_idx)
{
	// All idle blocks are stored in double linked lists identified
	// by mIdleBlocks. 
	aos_assert_r(AosDfmUtil::isSizeIdxValid(size_idx), 0);
	aos_assert_r(idleListSanityCheck(), NULL);
	AosDiskBlock2Ptr head = mIdleBlocks[size_idx];
	if (head)
	{
		AosDiskBlock2Ptr block = head->getPrev();
		if (head == block)
		{
			// This is the last one. 
			mIdleBlocks[size_idx] = 0;
			block->setPrev(0);
			block->setNext(0);
			//OmnScreen << "getIdleBlockPriv: block:" << block.getPtr() << endl; 
			aos_assert_r(idleListSanityCheck(), NULL);
			return block;
		}

		block->removeFromList();
		aos_assert_r(idleListSanityCheck(), NULL);
		return block;
	}

	// There are no more idle blocks. This should normally happen
	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDiskBlockCache::removeFromIdlePriv(
		const AosDiskBlock2Ptr &block, 
		const int size_idx)
{
	// This function removes the block 'block' from the idle 
	// list. 
	aos_assert_r(AosDfmUtil::isSizeIdxValid(size_idx), false);
	aos_assert_r(idleListSanityCheck(), false);
	AosDiskBlock2Ptr head = mIdleBlocks[size_idx];
	aos_assert_r(head, false);
	if (block == head) head = head->getNext();
	block->removeFromList();
	
	if (block->getNext() == block)
	{
		// This is the last one.
		mIdleBlocks[size_idx] = 0;
	}
	else
	{
		mIdleBlocks[size_idx] = head;
	}

	aos_assert_r(idleListSanityCheck(), false);
	return true;
}


bool
AosDiskBlockCache::idleListSanityCheck()
{
	for (int i=0; i<AosDfmUtil::eMaxBlockSizes; i++)
	{
		AosDiskBlock2Ptr head = mIdleBlocks[i];
		if (head)
		{
			AosDiskBlock2Ptr next = head->getNext();
			int guard = 1000;
			while (guard-- && next != head)
			{
				aos_assert_r(next, false);
				next = next->getNext();
			}
			aos_assert_r(guard > 0, false);

			AosDiskBlock2Ptr prev = head->getPrev();
			guard = 1000;
			while (guard-- && prev != head)
			{
				aos_assert_r(prev, false);
				prev = prev->getPrev();
			}
			aos_assert_r(guard > 0, false);
		}
	}
	return true;
}


