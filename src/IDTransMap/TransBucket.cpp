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
// 10/23/2011	Created by chen Ring
////////////////////////////////////////////////////////////////////////////
#include "IDTransMap/TransBucket.h"
/*
#include "IDTransMap/IDTransMap.h"
#include "TransUtil/IILTrans.h"
#include "SEInterfaces/IILMgrObj.h"
#include "IILUtil/IILFuncType.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"

u32 AosTransBucket::smHeaderSize;
u32 AosTransBucket::smHeaderNumEntries;
u32 AosTransBucket::smBodyBlockSize;
u32 AosTransBucket::smMaxBuckets;
u32 AosTransBucket::smMaxTrans;
u32 AosTransBucket::smFreeBlockStart;
OmnMutexPtr AosTransBucket::smLock= OmnNew OmnMutex();

AosTransBucket::AosTransBucket(
		const u64 &start_id, 
		const OmnFilePtr &file, 
		const AosRundataPtr &rdata):
mLock(OmnNew OmnMutex),
mFile(file),
mStartId(start_id),
mBlockOffset(0),			
mBlockCrtLen(0),	
mNumTrans(0),
mRefCount(0)
{
	//mHeaderBuff = OmnNew u32[smHeaderNumEntries][2];
	mHeaderBuff = (u32(*)[2])(OmnNew char[smHeaderSize]);
	mBlockBuff = OmnNew AosBuff(smBodyBlockSize, 0 AosMemoryCheckerArgs);
	readHeader();
}


AosTransBucket::~AosTransBucket()
{
	OmnDelete[] mHeaderBuff;
}


void
AosTransBucket::config(
		const u32 numEntries, 
		const u32 maxBuckets,
		const u32 datablockSize,
		const u32 maxTrans)
{
	smHeaderSize = numEntries*eHeaderEntrySize + eBlockOffsetSize + eBlockCrtLenSize;
	smHeaderNumEntries = numEntries;
	smBodyBlockSize = datablockSize;
	smMaxBuckets = maxBuckets; 
	smMaxTrans = maxTrans; 
	smFreeBlockStart = smHeaderSize*smMaxBuckets;
}


bool 
AosTransBucket::readFromFile(
		const u64 &start_id, 
		const OmnFilePtr &file, 
		const AosRundataPtr &rdata)
{
	// This funciton saves the current bucket and reads a new bucket from file.
	mLock->lock();
	bool rslt = saveHeader();
	aos_assert_rl(rslt, mLock, false);

	rslt = saveBlock();
	aos_assert_rl(rslt, mLock, false);

	mStartId = start_id;
	mFile = file;
	readHeader();
	mLock->unlock();
	return true;
}


//bool 
//AosTransBucket::saveToFile(const AosRundataPtr &rdata)
//{
//	bool rslt = saveHeader();
//	aos_assert_r(rslt, false);
//	rslt = saveBlock();
//	aos_assert_r(rslt, false);
//	return true;
//}


bool
AosTransBucket::readHeader()
{
	// Block header format:
	// 		IIL head, IIL tail	u32 + u32
	// 		IIL head, IIL tail	u32 + u32
	//		...
	// 		Block Offset		u32
	// 		Block Current Len	u32
	//
	u32 bucketid = mStartId / smHeaderNumEntries;
	aos_assert_r(bucketid < smMaxBuckets, false);

	mFile->lock();
	int size = mFile->readToBuff(
			bucketid*smHeaderSize, smHeaderSize, (char*)mHeaderBuff);
	mFile->unlock();

	if (size <= 0)
	{
		memset(mHeaderBuff, 0, smHeaderSize);	
	}
	else
	{
		aos_assert_r((u32)size == smHeaderSize, false);
	}
	mBlockOffset = mHeaderBuff[smHeaderNumEntries][0];
	if (mBlockOffset == 0)
	{
		smLock->lock();
		mBlockOffset = smFreeBlockStart;
		smFreeBlockStart += smBodyBlockSize;
		smLock->unlock();
	}

	mBlockCrtLen = mHeaderBuff[smHeaderNumEntries][1];
	aos_assert_r(mBlockCrtLen <= smBodyBlockSize, false);

	return true;
}


bool
AosTransBucket::saveHeader()
{
	u32 bucketid = mStartId / smHeaderNumEntries;
	aos_assert_r(bucketid < smMaxBuckets, false);
	mHeaderBuff[smHeaderNumEntries][0] = mBlockOffset;
	mHeaderBuff[smHeaderNumEntries][1] = mBlockCrtLen;
	aos_assert_r(mBlockCrtLen <= smBodyBlockSize, false);

	mFile->lock();
	bool rslt = mFile->put(bucketid*smHeaderSize, (char*)mHeaderBuff, smHeaderSize, true);
	mFile->unlock();
	return rslt;
}


bool 
AosTransBucket::addTrans(const u64 &id, 
		const AosIDTransVectorPtr &transes,
		const AosRundataPtr &rdata)
{
	//1.Get the header and tail posision 
	//  that are the linked list in the file by the id.
	mLock->lock();
	sanityCheck();
	u32 pos = id - mStartId;
	aos_assert_rl(pos>=0 && pos<smHeaderNumEntries, mLock, false);
	u32 tail  = mHeaderBuff[pos][1];

	//2.Add entriy of the list.
	for (size_t i=0; i<transes->size(); i++)
	{
		mBlockCrtLen = mBlockBuff->dataLen();
		aos_assert_rl(mBlockCrtLen <= smBodyBlockSize, mLock, false);
		u32 newsize = mBlockCrtLen + 
//			(*transes.getPtr())[i]->getSerializeSize() + eNextPosSize;
			(int)(*transes.getPtr())[i]->getTransSize() + eNextPosSize;
		if (newsize > smBodyBlockSize || mNumTrans > smMaxTrans) 
		{
			// Buff is full or trans number is bigger than max. save the block.
			// it change the mBlockCrtLen to 0.
			saveBlock();
			getBlock(true);
		}

		mBlockBuff->setU32(0);
		(*transes.getPtr())[i]->serializeTo(mBlockBuff);
		//if((int)mBlockCrtLen + (*transes.getPtr())[i]->getSerializeSize() + 
		if((int)mBlockCrtLen + (int)(*transes.getPtr())[i]->getTransSize() + 
				eNextPosSize != mBlockBuff->dataLen())
		{
			AosIILTransPtr t = (AosIILTrans*)(*transes.getPtr())[i].getPtr();
			//OmnAlarm << "TransSsize: " << t->getSerializeSize() << ", "
			OmnAlarm << "TransSsize: " << (int)t->getTransSize() << ", "
				"BlockCLen: " << mBlockCrtLen << ", " << "BlockDL: " << mBlockBuff->dataLen() 
				<< " Transtype: " << t->getOpr() << enderr;
			mLock->unlock();
			return false;
		}
		//update the last list entry.
		if (tail)
		{
			if ((tail-mBlockOffset)>=smBodyBlockSize)
			{
				mFile->lock();
				mFile->setU32(tail, mBlockCrtLen+mBlockOffset, false);
				mFile->unlock();
			}
			else
			{
				u32 crtIdx = mBlockBuff->getCrtIdx();
				mBlockBuff->setCrtIdx(tail-mBlockOffset);
				mBlockBuff->setU32(mBlockCrtLen + mBlockOffset);
				mBlockBuff->setCrtIdx(crtIdx);
			}
		}
		else
		{
			mHeaderBuff[pos][0] = mBlockCrtLen + mBlockOffset;
			aos_assert_rl(mHeaderBuff[pos][0] <= smFreeBlockStart, mLock, false);
		}
		tail = mBlockCrtLen + mBlockOffset;
	}
	
	//set header tail
	aos_assert_rl(tail <= smFreeBlockStart, mLock, false);
	mHeaderBuff[pos][1] = tail;
	sanityCheck();
	mLock->unlock();
	return true;
}


bool
AosTransBucket::procTrans(
		const u64 &id,
		const AosIDTransVectorPtr &transes,
		const AosRundataPtr &rdata)
{
	//1.Get the header and tail posision 
	//  that are the linked list in the file by the id.
	mLock->lock();
	sanityCheck();
	u32 pos = id - mStartId;
	aos_assert_rl(pos>=0 && pos<smHeaderNumEntries, mLock, false);
	u32 header = mHeaderBuff[pos][0];

	// The iil has not cached the trans. 
	if (header == 0)
	{
		mLock->unlock();
		return true;
	}

	// Get all the enties which are store by linked list in the file form file.
	AosBuffPtr buff = OmnNew AosBuff(smBodyBlockSize, 0 AosMemoryCheckerArgs);
	u32 next = header;
	bool finished = false;
	do
	{
		AosBuffPtr tmpBuff = buff;
		u32 blockid = (next-smMaxBuckets*smHeaderSize)/smBodyBlockSize;
		u32 blockOffset = smMaxBuckets*smHeaderSize + blockid*smBodyBlockSize;
		if (blockOffset == mBlockOffset)
		{
			//the block in memery
			tmpBuff = OmnNew AosBuff(mBlockBuff->data(), smBodyBlockSize, 
					mBlockBuff->dataLen(), 0 AosMemoryCheckerArgs);
		}
		else
		{
			mFile->lock();
			int size = mFile->readToBuff(blockOffset, smBodyBlockSize, tmpBuff->data());	
			mFile->unlock();
			aos_assert_rl((u32)size == smBodyBlockSize, mLock, false);
			tmpBuff->setDataLen(smBodyBlockSize);
		}

		while(1)
		{
//OmnScreen << "proctrans 1:" << id << ", " << next << endl;
			u32 pos = next-blockOffset;
			aos_assert_rl(pos < smBodyBlockSize, mLock, false);
			tmpBuff->setCrtIdx(pos);
			next = tmpBuff->getU32(-1);
			aos_assert_rl(next != 0xffffffff, mLock, false);

			// Ketty 2013/02/21
			//AosIILFuncType::E opr = (AosIILFuncType::E)tmpBuff->getChar(0);
			//aos_assert_rl(AosIILFuncType::isValid(opr), mLock, false);
			//AosIILTransPtr iiltrans = AosIILTrans::getTrans(opr, tmpBuff);
			//aos_assert_rl(iiltrans, mLock, false);
			
			//AosTinyTransPtr tiny_trans = (AosTinyTrans *)iiltrans.getPtr();
			//transes->push_back(tiny_trans);
			AosTransPtr trans = AosTrans::serializeFromStatic(tmpBuff);	
			aos_assert_rl(trans, mLock, false);
			AosIILTransPtr iiltrans = (AosIILTrans *)trans.getPtr();
			transes->push_back(iiltrans);
			// Ketty end.

			if (next == 0)
			{
//OmnScreen << "proctrans 2:tail" << endl;
				finished = true;
				break; 
			}
			if (next-blockOffset >= smBodyBlockSize)
				break;
		}
	}
	while(!finished);

	mHeaderBuff[pos][0] = 0;
	mHeaderBuff[pos][1] = 0;
	sanityCheck();
	mLock->unlock();
	
	return true;
}


bool
AosTransBucket::saveBlock()
{
	mFile->lock();
	bool rslt =	mFile->put(mBlockOffset, mBlockBuff->data(),smBodyBlockSize, true);
	mFile->unlock();
	return rslt;
}


bool
AosTransBucket::getBlock(bool needNew)
{
	if (needNew)
	{
		smLock->lock();
		mBlockOffset = smFreeBlockStart;
		smFreeBlockStart += smBodyBlockSize;
		if (mBlockOffset > smFreeBlockStart)
		{
			OmnAlarm << "Cache file have full!" << enderr;
		}
		smLock->unlock();
		mBlockCrtLen = 0;
		mBlockBuff->reset();
		mBlockBuff->setDataLen(0);
	}
	else
	{
		mFile->lock();
		int size = mFile->readToBuff(mBlockOffset, smBodyBlockSize, mBlockBuff->data());
		mFile->unlock();

		aos_assert_r((u32)size == smBodyBlockSize, false);
		mBlockBuff->setDataLen(mBlockCrtLen);
	}
	return true;
}


bool
AosTransBucket::procAllTrans(const u32 siteid, const AosRundataPtr &rdata)
{
	int total = 0;
	int totaltrans = 0;
	AosIDTransVectorPtr trans = OmnNew AosIDTransVector();
	for (size_t i=0; i<smHeaderNumEntries; i++)
	{
		procTrans(mStartId+i, trans, rdata);
		if (!trans->empty())
		{
			total++;
			totaltrans += trans->size();
			AosIILMgrObj::getIILMgr()->procTrans(mStartId+i, siteid, trans, rdata);
			trans->clear();
		}
	}
	return true;
}

bool
AosTransBucket::procOneTrans(bool &trans_processed,const u32 siteid, const AosRundataPtr &rdata)
{
	trans_processed = false;
	int total = 0;
	int totaltrans = 0;
	AosIDTransVectorPtr trans = OmnNew AosIDTransVector();
	for (size_t i=0; i<smHeaderNumEntries; i++)
	{
		procTrans(mStartId+i, trans, rdata);
		if (!trans->empty())
		{
			trans_processed = true;
			total++;
			totaltrans += trans->size();
			AosIILMgrObj::getIILMgr()->procTrans(mStartId+i, siteid, trans, rdata);
			trans->clear();
			return true;
		}
	}
	return true;
}

bool
AosTransBucket::sanityCheck()
{
	return true;
	for (u32 i=0 ; i<smHeaderNumEntries; i++)	
	{
		aos_assert_r(checkOneTrans(mHeaderBuff[i][0], mHeaderBuff[i][1]),
				false);
	}
	return true;
}


bool
AosTransBucket::checkOneTrans(
		const u32 header,
		const u32 tail)
{
	if (header == 0)
	{
		aos_assert_r(tail==0, false);
		return true;
	}

	// Get all the enties which are store by linked list in the file form file.
	AosBuffPtr buff = OmnNew AosBuff(smBodyBlockSize, 0 AosMemoryCheckerArgs);
	u32 next = header;
	bool finished = false;
	do
	{
		AosBuffPtr tmpBuff = buff;
		u32 blockid = (next-smMaxBuckets*smHeaderSize)/smBodyBlockSize;
		u32 blockOffset = smMaxBuckets*smHeaderSize + blockid*smBodyBlockSize;
		if (blockOffset == mBlockOffset)
		{
			//the block in memery
			tmpBuff = OmnNew AosBuff(mBlockBuff->data(), smBodyBlockSize, 
					mBlockBuff->dataLen(), 0 AosMemoryCheckerArgs);
		}
		else
		{
			mFile->lock();
			int size = mFile->readToBuff(blockOffset, smBodyBlockSize, tmpBuff->data());	
			mFile->unlock();
			aos_assert_r((u32)size == smBodyBlockSize, false);
			tmpBuff->setDataLen(smBodyBlockSize);
		}

		while(1)
		{
			u32 pos = next-blockOffset;
			aos_assert_r(pos < smBodyBlockSize, false);
			tmpBuff->setCrtIdx(pos);
			u32 next1 = tmpBuff->getU32(-1);
			aos_assert_r(next1 != 0xffffffff, false);
			if (next1 == 0)
			{
				aos_assert_r(tail == next, false);
				finished = true;
				break; 
			}
			next = next1;

			if (next-blockOffset >= smBodyBlockSize)
				break;
		}
	}
	while(!finished);
	return true;
}

*/
