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
// 07/19/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/DiskBlockMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Security/Session.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/DiskBlock.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "SiteMgr/SyncServer.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

extern AosXmlTagPtr gAosAppConfig;

const bool sgSanityCheck = false;

AosDiskBlockMgr::AosDiskBlockMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	for (u32 i=0; i<eAosMaxFileSeqno; i++) mBlocksizes[i] = -1;
}


AosDiskBlockMgr::~AosDiskBlockMgr()
{
}


AosDiskBlockPtr
AosDiskBlockMgr::getBlock2(
		const OmnFilePtr &file, 
		const u32 seqno, 
		const u32 offset, 
		const u32 blocksize,
		const bool createnew)
{
	// It keeps a list of currently opened blocks. When someone wants
	// a block, it checks whether it is already loaded into memory. 
	// If yes, it returns the block. 

	aos_assert_r(blocksize > 0, 0);
	u32 blockStart = (offset/blocksize) * blocksize; 
	aos_assert_r(blockStart >= blocksize, NULL);

	while (1)
	{
		mLock->lock();
		aos_assert_rl(sanityCheck(), mLock, 0);
		AosDiskBlockPtr idleblock;
		int empty_idx = -1;
		for (int i=0; i<eMaxBlocks; i++)
		{
			if (!mBlocks[i])
			{
				// It is an empty block. It may be used to create a new one.
				empty_idx = i;
				continue;
			}
			
			if (mBlocks[i]->blockMatch(seqno, blockStart, blocksize))
			{
				// This is the block to use. 
				aos_assert_rl(sanityCheck(), mLock, 0);
				mBlocks[i]->addReference();
				aos_assert_rl(sanityCheck(), mLock, 0);
				mLock->unlock();
				return mBlocks[i];
			}

			if (mBlocks[i]->isIdle() && !idleblock)
			{
				idleblock = mBlocks[i];
			}
		}

		// This means that it did not find the block. Check whether 
		// an empty block was found
		if (empty_idx != -1)
		{
			mBlocks[empty_idx] = OmnNew AosDiskBlock(file,
						seqno, blockStart, blocksize, createnew);
			
			aos_assert_rl(mBlocks[empty_idx], mLock, 0);
			mBlocks[empty_idx]->addReference();

			aos_assert_rl(sanityCheck(), mLock, 0);

			mLock->unlock();
			return mBlocks[empty_idx];
		}

		// Check whether an idle block is found
		if (idleblock)
		{
			aos_assert_rl(sanityCheck(), mLock, 0);
			bool rslt = idleblock->reload(file, seqno, blockStart, blocksize, createnew);
			//bool rslt = idleblock->reload(file, seqno, offset, createnew);
			aos_assert_rl(rslt, mLock, 0);
			idleblock->addReference();
			aos_assert_rl(sanityCheck(), mLock, 0);

			mLock->unlock();
			return idleblock;
		}

		// Did not find any. This is a serious problem
		OmnAlarm << "Failed to find a block!" << enderr;
		mCondVar->wait(mLock);
		aos_assert_rl(sanityCheck(), mLock, 0);
		mLock->unlock();
	}

	OmnShouldNeverComeHere;
	return 0;
}//enl

AosDiskBlockPtr
AosDiskBlockMgr::getBlock(
		const OmnFilePtr &file, 
		const u32 seqno, 
		const u32 offset, 
		const u32 blocksize,
		const bool createnew,
		const OmnString &fname, 
		const int lineno)
{
	// It keeps a list of currently opened blocks. When someone wants
	// a block, it checks whether it is already loaded into memory. 
	// If yes, it returns the block. 

	u32 blockStart = (offset/blocksize) * blocksize; 
	aos_assert_r(blockStart >= blocksize, NULL);

	while (1)
	{
		mLock->lock();
		aos_assert_rl(sanityCheck(), mLock, 0);
		AosDiskBlockPtr idleblock;
		int empty_idx = -1;
		for (int i=0; i<eMaxBlocks; i++)
		{
			if (!mBlocks[i])
			{
				// It is an empty block. It may be used to create a new one.
				empty_idx = i;
				continue;
			}
			
			if (mBlocks[i]->blockMatch(seqno, blockStart, blocksize))
			{
				// This is the block to use. 
				aos_assert_rl(sanityCheck(), mLock, 0);
				mBlocks[i]->addReference();
				aos_assert_rl(sanityCheck(), mLock, 0);
				mLock->unlock();
				return mBlocks[i];
			}

			if (mBlocks[i]->isIdle() && !idleblock)
			{
				idleblock = mBlocks[i];
			}
		}

		// This means that it did not find the block. Check whether 
		// an empty block was found
		if (empty_idx != -1)
		{
			mBlocks[empty_idx] = OmnNew AosDiskBlock(file,
						seqno, blockStart, blocksize, createnew);
			
			mFnames[empty_idx] = fname;
			mLines[empty_idx] = lineno;

			aos_assert_rl(mBlocks[empty_idx], mLock, 0);
			mBlocks[empty_idx]->addReference();

			mBlocksizes[seqno] = blocksize;
			aos_assert_rl(sanityCheck(), mLock, 0);

			mLock->unlock();
			return mBlocks[empty_idx];
		}

		// Check whether an idle block is found
		if (idleblock)
		{
			aos_assert_rl(sanityCheck(), mLock, 0);
			bool rslt = idleblock->reload(file, seqno, blockStart, blocksize, createnew);
			//bool rslt = idleblock->reload(file, seqno, offset, createnew);
			aos_assert_rl(rslt, mLock, 0);
			idleblock->addReference();
			aos_assert_rl(sanityCheck(), mLock, 0);

			mLock->unlock();
			return idleblock;
		}

		// Did not find any. This is a serious problem
		OmnAlarm << "Failed to find a block!" << enderr;

		mCondVar->wait(mLock);
		aos_assert_rl(sanityCheck(), mLock, 0);
		mLock->unlock();
	}

	OmnShouldNeverComeHere;
	return 0;
}

bool
AosDiskBlockMgr::returnBlock(const AosDiskBlockPtr &block)
{
	mLock->lock();
	block->removeReference();
	if (block->isIdle()) mCondVar->signal();
	mLock->unlock();
	return true;
}

bool 
AosDiskBlockMgr::sanityCheck()
{
	if(!sgSanityCheck)
	{
		return true;
	}

	for (int i=0; i<eMaxBlocks; i++)
	{
		if (!mBlocks[i])continue;
		u32 seqno = mBlocks[i]->getSeqno();
		aos_assert_r(seqno >= 0 && seqno < eAosMaxFileSeqno, false);
		u32 blocksize = mBlocks[i]->getBlocksize();
		if (mBlocksizes[seqno] < 0)
		{
			mBlocksizes[seqno] = blocksize;
		}
		else
		{
			if ((u32)mBlocksizes[seqno] != blocksize)
			{
				OmnAlarm << "blocksize error: " << seqno 
					<< ":" << (u32)mBlocksizes[seqno] << ":" << blocksize << enderr;
				return false;
			}
		}
				
		aos_assert_r(mBlocks[i]->getReference()>=0, false);

		for (int j=0; j<eMaxBlocks; j++)
		{
			if (!mBlocks[j] || i==j) continue; 
			if(mBlocks[i]->getSeqno() == mBlocks[j]->getSeqno())
			{
				aos_assert_r(mBlocks[i]->getBlocksize() == mBlocks[j]->getBlocksize(),false);
				aos_assert_r(mBlocks[i]->getBlockStart()!= mBlocks[j]->getBlockStart(),false);
			}
		}

	}
	return true;
}


bool
AosDiskBlockMgr::saveAllBlocks()
{
	mLock->lock();
	for (int i=0; i<eMaxBlocks; i++)
	{
		if (mBlocks[i]) mBlocks[i]->saveToDisk(true);
	}
	mLock->unlock();
	return true;
}
