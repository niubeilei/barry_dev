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
// This class is used as the secondary IIL transaction caching. When there
// are too many cached IIL transactions in the first level cacher 'IDTransMap', 
// some of the IIL transactions are pushed to this class. This class 
// maintains a linked list for each IIL. IIL transactions are appended to
// the end of the file. There is an index file that contains only the
// list tails. For each transaction to be cached by this class, 
// it checks whether there is a linked list for the IIL. If not, it creates
// one. Only linked list tails are saved. IIL tails are saved as u32, which
// means the transaction log file has a maximum size limitation. Every N
// number of Linked List Tails are grouped into one block. If N is 250,000,
// this means each block is 1M. The class can hold up to M number of such 
// blocks (using 10M bytes to save the headers). 
//
// When adding a new transaction, the transaction is appended to a cached 
// list. If the list is too big, all the transactions in the list are 
// appended to the file. If the list can hold up to 1000 transactions, this
// means it will generate one I/O request per 1,000 thousand transactions.
// The block is a u32 array. 
//
// For each transaction, it checks whether the Block is loaded into memory. 
// If not, it loads the block into memory. It then checks whether the linked
// list exists. If not, the current position is saved. Otherwise, the tail
// is saved in the transaction, and the current position is used as the 
// linked list's new tail.
//
// There can be multiple trans log files. When the current trans log file is
// full, it will use the next one. All files are named as:
// 			prefix_<dddddddddd>
// where <dddddddddd> is a ten-digit number. When a file is processed, it is 
// moved to a backup directory. Files are not processed until it is full. 
//
// Modification History:
// Created: 2011/10/20 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IDTransMap/SecondTransMap.h"
/*
#include "TransUtil/IILTrans.h"
#include "IDTransMap/IDTransMap.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util1/Wait.h"
#include "XmlUtil/XmlTag.h"
#include <list>
using namespace std;

const OmnString sgDftFilename = "second_trans";

AosSecondTransMap::AosSecondTransMap()
:
mLock(OmnNew OmnMutex()),
mCrtPos(0),
mCrtTail(0),
mNumBlocksLoaded(0),
mMaxBlocksLoaded(eDftMaxBlocksLoaded),
mNumIILsPerBlock(eDftNumIILsPerBlock),
mBlockSize(sizeof(u32) * mNumIILsPerBlock),
//mBuff(OmnNew AosBuff(eBuffLen, eBuffIncSize AosMemoryCheckerArgs)), //felicia, 2012/09/26
mBuff(OmnNew AosBuff(eBuffLen AosMemoryCheckerArgs)),
mThreadFreq(1000)
{
}


AosSecondTransMap::~AosSecondTransMap()
{
	for (int i=0; i<=mMaxBlocksLoaded; i++)
	{
		releseBlockMemory(mBlocks[i]);
	}
}


bool 
AosSecondTransMap::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	// This thread wakes up every given number of milliseconds. 
	// After that, it saves some of the outstanding transactions.
	u64 lastTimestamp = OmnGetTimestamp();
	while (state == OmnThrdStatus::eActive)
	{
		mFinishedLock->lock();
		while (!mFinishedFile)
		{
			mFinishedFile = getNextLogFile();
			if (mFinishedFile) break;
				
			mFinishedCondVar->wait(mFinishedLock);
		}
		mFinishedLock->unlock();

		while (mFinishedFile)
		{
			OmnWait::getSelf()->wait(mThreadFreq);
			u64 crtTimestamp = OmnGetTimestamp();
			if (crtTimestamp > lastTimestamp &&
				(crtTimestamp - lastTimestamp)/1000 >= mThreadFreq)
			{
				procTrans();
			}
		}
	}
	return true;
}


bool
AosSecondTransMap::procTrans()
{
	OmnNotImplementedYet;
	return false;
	
	// This function processes some transactions. 
	//aos_assert_r(mFinishedFile, false);
	//if (mLastIILID == 0)
	//{
	//	// Need to read the first block 
	//	mLastBlockIdx = 0;
	//	bool rslt = readBlock(mFinishedFile, mLastBlockIdx, (char *)mFinishedBlock);
	//	aos_assert_r(rslt, false);
	//	mLastIILID = eStartIILID;
	//	mLastBlockIdx = 0;
	//}
	//else
	//{
	//	u32 block_idx = getBlockIdx(mLastIILID);
	//	if (mLastBlockIdx != block_idx)
	//	{
	//		mLastBlockIdx = block_idx;
	//		bool rslt = readBlock(mFinishedfile, block_idx, (char *)mFinishedBlock);
	//		aos_assert_r(rslt, false);
	//	}
	//}

	//int idx = mLastIILID % mNumIILsPerBlock;
	//for (int nn=0; nn<eLogProcBatchSize; nn++)
	//{
	//	u32 tail = mFinishedBlock[idx];
	//	if (tail > 0)
	//	{
	//		bool rslt = procOneIIL(mLastIILID, tail, mFinishedFile);
	//		aos_assert_r(rslt, false);
	//	}
	//	idx++;
	//	mLastIILID++;
	//	if (idx >= mNumIILsPerBlock)
	//	{
	//		return true;
	//	}
	//}
	//return true;
}


bool
AosSecondTransMap::procOneIIL(
		const u64 &iilid, 
		const u32 tail,
		const OmnFilePtr &file)
{
	// This function retrieves all the transactions for the IIL: 'iilid'. 
	// Transactions are chained backwards. 
	// Each transaction is stored as:
	// 		contents
	// 		previous position
	// 		size
	//
	OmnNotImplementedYet;
	return false;
}


bool
AosSecondTransMap::config(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	if (conf)
	{
		AosXmlTagPtr ff = conf->getFirstChild("idtrans");
		if (ff)
		{
			int max_cached = ff->getAttrInt("max_cached_size", eDftMaxCachedSize);
			if (max_cached <= 0)
			{
			 	OmnAlarm << "Invalid Max Cached Trans: " << max_cached << enderr;
			 	mMaxCachedSize = eDftMaxCachedSize;
			}
			else
			{
				mMaxCachedSize = (u32)max_cached;
			}

			int iils_per_block = ff->getAttrInt("iils_per_block", eDftIILsPerBlock);
			if (iils_per_block <= 0)
			{
			 	OmnAlarm << "Invalid Number of IILs per Block: " << iils_per_block << enderr;
			 	mNumIILsPerBlock = eDftIILsPerBlock;
			}
			else
			{
				mNumIILsPerBlock = (u32)iils_per_block;
			}
			mBlockSize = sizeof(u32) * mNumIILsPerBlock;

			int loaded_blocks = ff->getAttrInt("max_loaded_blocks", eDftMaxBlocksLoaded);
			if (loaded_blocks <= 0)
			{
			 	OmnAlarm << "Invalid Max number of loaded blocks: " << loaded_blocks << enderr;
			 	mMaxBlocksLoaded = eDftMaxBlocksLoaded;
			}
			else
			{
				mMaxBlocksLoaded = (u32)loaded_blocks;
			}

			mDirname = ff->getAttrStr("work_dir", ".");
			mBackupDir = ff->getAttrStr("back_dir", "trans_log_back");
		}
	}

	OmnString fname = mDirname;
	fname << "/" << sgDftFilename << (u32)OmnGetSecond();
	mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_rr(mFile && mFile->isGood(), rdata, false);
	mCrtPos = mFile->getLength();
	if (mCrtPos < eFileStart)
	{
		mCrtPos = eFileStart;
	}
	mCrtTail = mCrtPos;
	return true;
}


OmnFilePtr
AosSecondTransMap::getNextLogFile()
{
	// This function searches all the files and find the 
	// first trans log file. 
	//list<OmnString> filenames;
	//int num = OmnFile::getFiles(mDirname, filenames, false);
	//list<OmnString>::iterator itr;
	///u32 file_seqno = 0xffffffff;
	//for (itr = filenames::begin(); itr != filenames::end(); itr++)
	//{
	//	OmnString fname = *itr;
	//	if (strncmp(fname.data(), sgDftFilename, sizeof(sgDftFilename)) == 0)
	//	{
	//		fname.remove(0, sizeof(sgDftFilename));
	//		if (fname.isDigitStr())
	//		{
	//			// It is int he form: 
	//			// 		sgDftFilename_dddddd
	//			u32 num = atoll(fname.data());
	//			if (num < file_seqno) file_seqno = num;
	//		}
	//	}
	//}

	//if (file_seqno == 0)
	//{
	//	// Did not find it. 
	//	return 0;
	//}

	//OmnString fname = sgDftFilename;
	//fname << file_seqno;
	//OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	//aos_assert_r(ff && ff->isGood(), 0);
	//Omnstring status = ff->readStr(eStatusOffset, eStatusLen, "");
	//if (status != sgStatusFinished) return 0;
	//return ff;
	OmnNotImplementedYet;
	return 0;
}


bool 
AosSecondTransMap::addTrans(
		const u64 &iilid,
		const AosIDTransVectorPtr &transes,
		const AosRundataPtr &rdata)
{
	// It adds all the transactions 'transes' to the corresponding queue. 
	// If the queue is full, it will save the transactions.
	mLock->lock();
	AosIDTransVector::iterator itr = transes->begin();
	for(; itr != transes->end(); itr++)
	{
//		bool rslt = addTransToTail(iilid, *itr, rdata);
//		aos_assert_rr(rslt, rdata, false);
	}

	int size = mBuff->dataLen();
	if (size > 0 && (u32)size > mMaxCachedSize)
	{
		// Need to save the cached transactions.
		bool rslt = saveCachedTrans(rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	mLock->unlock();
	return true;
}

 
bool
AosSecondTransMap::addTransToTail(
		const u64 &iilid, 
		const AosIILTransPtr &trans,
		const AosRundataPtr &rdata)
{
	// This function uses mCrtTail as the new tail to 'iilid'. 
	// If the entry does not exist, simply update it. Otherwise, 
	// it retrieves the iilid's current tail, and then update it. 
	// The existing tail is returned.
	int block_idx = getBlockIdx(iilid);
	aos_assert_rr(block_idx >= 0 && block_idx < eMaxNumBlocks, rdata, false);
	if (!mBlocks[block_idx])
	{
		// The block has not been loaded into memory yet. 
		u32 *memory = 0;
		if (mNumBlocksLoaded < mMaxBlocksLoaded)
		{
			// There is no need to pop anyone. Need to allocate memory.
			memory = allocateBlockMemory();
			mNumBlocksLoaded++;
		}
		else
		{
			// There are too many blocks being loaded into memory. 
			// Need to push out the least used one. The for-loop
			// will find the least used one.
			u32 count = 0xffffffff;
			int remove_idx = -1;
			for (int i=0; i<eMaxNumBlocks; i++)
			{
				if (mBlocks[i])
				{
					if (mCounts[i] == 0)
					{
						// This is the block to remove out.
						remove_idx = i;
						break;
					}

					if (mCounts[i] < 0)
					{
						OmnAlarm << "Counts is negative: " << mCounts[i] << enderr;
						mCounts[i] = 0;
					}

					if ((u32)mCounts[i] < count) 
					{
						count = mCounts[i];
						remove_idx = i;
					}
				}
			}

			if (remove_idx == -1)
			{
				AosSetError(rdata, AosErrmsgId::eInternalError);
				OmnAlarm << rdata->getErrmsg() << ". mNumBlocksLoaded: " 
					<< mNumBlocksLoaded << ". mMaxLoadedBlocks: "
					<< mMaxBlocksLoaded << enderr;
				for (int k=0; k<eMaxNumBlocks; k++)
				{
					if (mBlocks[k]) 
					{
						remove_idx = k;
						break;
					}
				}
				aos_assert_rr(remove_idx >= 0 && remove_idx < eMaxNumBlocks, rdata, false);
			}

			aos_assert_rr(mBlocks[remove_idx], rdata, false);
			memory = mBlocks[remove_idx];
			mBlocks[remove_idx] = 0;
			mCounts[remove_idx] = 0;
			u32 offset = getBlockOffset(remove_idx);
			bool rslt = mFile->put(offset, (char *)memory, sizeof(u32) * eMaxNumBlocks, true);
			aos_assert_rr(rslt, rdata, false);
		}

		// Read the contents into memory
		u32 offset = getBlockOffset(block_idx);
		char *data = (char *)memory;
		int num_bytes = mFile->readToBuff(offset, mBlockSize, data); 
		aos_assert_rr(num_bytes > 0 && (u32)num_bytes == mBlockSize, rdata, false);
		mBlocks[block_idx] = memory;
		mCounts[block_idx] = 0;
	}

	// The current tail for the entry 'trans' is 'mBlocks[block_idx][relative_id]'.
	// It puts the transaction 'trans' to mBuff, then its current tail to 'mBuff'. 
	// After that, it updates the tail to 'mBlocks'.
	int relative_id = getRelativeId(iilid);
	aos_assert_rr(relative_id >= 0 && (u32)relative_id < mNumIILsPerBlock, rdata, false);
	u32 tail = mBlocks[block_idx][relative_id];
	int datalen = mBuff->dataLen();

	// 1. Serialize the transaction to 'mBuff'.
	// 2. Append the tail
	// 3. Append the length
	trans->serializeTo(mBuff);					// 1. Serialize the trans.
	mBuff->setU32(tail);						// 2. Append tail
	int entry_len = mBuff->dataLen() - datalen;	// 3. Append the entry length
	mBuff->setInt(entry_len);

	mCrtTail += mBuff->dataLen() - datalen + AosBuff::getIntSize();
	mBlocks[block_idx][relative_id] = mCrtTail;
	mCounts[block_idx]++;
	return true;
}


bool
AosSecondTransMap::saveCachedTrans(const AosRundataPtr &rdata)
{
	aos_assert_rr(mFile, rdata, false);
	aos_assert_rr(mCrtPos + mBuff->dataLen() == mCrtTail, rdata, false);
	mFile->put(mCrtPos, mBuff->data(), mBuff->dataLen(), true);
	mCrtPos += mBuff->dataLen();
	return true;
}


u32 
AosSecondTransMap::getBlockIdx(const u32 &iilid) const
{
	// A block is an array of U32. Each block holds mNumIILsPerBlock. 
	// This function calculates the index of the block in which 
	// the iilid 'iilid' resides. 
	return iilid % mNumIILsPerBlock;
}


u32 *
AosSecondTransMap::allocateBlockMemory() const
{
	return OmnNew u32[mNumIILsPerBlock];
}

void
AosSecondTransMap::releseBlockMemory(const u32 * ptr)
{
	OmnDelete [] ptr;
}


u32 
AosSecondTransMap::getBlockOffset(const int block_idx) const
{
	return mBlockSize * block_idx;
}


int 
AosSecondTransMap::getRelativeId(const u64 &iilid) const
{
	aos_assert_r(mNumIILsPerBlock > 0, 0);
	return iilid % mNumIILsPerBlock;
}
*/
