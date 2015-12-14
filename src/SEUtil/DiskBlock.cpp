////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Blocks are started with a Block Control Record that is
// in the following format:
// 		<blocksize>		4 bytes
// 		<blockflag>		4 bytes (used for error checking)
// 		<num_records>	4 bytes
// 		<buffsize>		4 bytes (the size of the buffer)
// 		<buffstart>		4 bytes (the start position for the buffer)
// 		ckSize
//		<reserved>		eBlockReservedSize
//		<idxstart>       
//
// Each record is stored in the following format:
// 		<docsize>		4 bytes (docsize)
// 		<entry_id>		4 bytes (entry_id)
// 		<flag>			4 bytes (for error checking)
// 		<reserved>		eReservedRcdSize
// 		<body>			variable
//
// Modification History:
// 07/18/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "SEUtil/DiskBlock.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"


#define AosDiskBlockCheck sanityCheck()

static int sgBlocks = 0;

AosDiskBlock::AosDiskBlock(
		const OmnFilePtr &file,
		const u32 seqno, 
		const u64 &offset, 
		const u32 blocksize,
		const bool flag)
:
mBlocksize(blocksize),
mBlockStart(offset),
mData(0),
mFile(file),
mGroupIdx(0),
mIsGood(false),
mSeqno(seqno),
mLock(OmnNew OmnMutex()),
mRefCount(0)
{
	// If 'flag' is true, it creates a block. Otherwise, it reads in 
	// an existing block.
	if (flag)
	{
		// Create a new block
		mIsGood = createNewBlock();
	}
	else
	{
		mIsGood = readBlock();
	}
}


AosDiskBlock::~AosDiskBlock()
{
	OmnScreen << "Delete a block: " << sgBlocks-- << endl;
	OmnDelete [] mData;
	mData = 0;
	mGroupIdx = 0;
}


bool
AosDiskBlock::createNewBlock()
{
	// 1. Allocate the memory for the block
	if (!mData)
	{
		mData = OmnNew unsigned char[eMaxDiskBlockSize+1];
		aos_assert_r(mData, false);
	}

	// 2. Set block size 
	setBlocksize(mBlocksize);

	// 3. Set the flag bytes
	mData[eBlockFlagOffset] = eBlockFlagByte0;
	mData[eBlockFlagOffset+1] = eBlockFlagByte1;
	mData[eBlockFlagOffset+2] = eBlockFlagByte2;
	mData[eBlockFlagOffset+3] = eBlockFlagByte3;

	// 4. Set number of records
	setNumRcd(0);

	// 5. Set buffer 
	setBufferStart(eBodyStart);
	setBufferSize(mBlocksize - eBodyStart - eRcdHeaderSize);

	// 6. Get the group index array
	mGroupIdx = (u32*)&mData[eIdxStart];
	memset(mGroupIdx, 0, sizeof(u32) * eMaxRecords/eGroupSize+1);
	mGroupIdx[0] = eBodyStart;

	// 7. Create the buffer record
	u32 start = getBufferStart();
	setRcdDocsize(eBodyStart, 0);
//OmnScreen <<"Save To Docsize  offset: "<<mBlockStart<<"  docsize:"<< 0 <<endl;
	setFlag(start);
	setReserved(start);

 	setEntryId(start,0xffffffff);	
	AosDiskBlockCheck;
	return true;
}


bool
AosDiskBlock::readBlock()
{
	aos_assert_r(mFile, false);

	// 1. Allocate the memory for the block
	if (!mData) mData = OmnNew unsigned char[eMaxDiskBlockSize+1];
	aos_assert_r(mData, false);

	mFile->lock();
	int  bytesread = mFile->readToBuff(mBlockStart, mBlocksize, (char *) mData);
	mFile->unlock();

	aos_assert_r(bytesread > 0 && (u32)bytesread == mBlocksize, false);
	
	mGroupIdx = (u32*)&mData[eIdxStart];

	AosDiskBlockCheck;
	return true;
}


u64
AosDiskBlock::addRcd(const u32 docsize, const char *data)
{
	// 1. Make sure the buffer is big enough for the record
	// 2. Get the buffer start
	// 3. Create the record
	// 4. Modify the buffer
	mLock->lock();
	AosDiskBlockCheck;

	// 1. Set entry id ,Make sure entryid less than Max Records number 
	u32 entryid = getNewEntryid();
	if (entryid >= eMaxRecords)
	{
		mLock->unlock();
		return 0;
	}
	u32 start = getBufferStart();

	// 2. Make sure the buffer is big enough for the record
	u32 buffersize = getBufferSize();
	if (buffersize < docsize+eRcdHeaderSize)
	{
		mLock->unlock();
		return 0;
	}

	setNumRcd(entryid+1);
	setEntryId(start, entryid);
	decreaseBuffer(docsize + eRcdHeaderSize);

	// 3. Set docsize

	setRcdDocsize(start, docsize);

	// 4. Set flag and reserved
	setFlag(start);
	setReserved(start);

	aos_assert_r(start + eRcdHeaderSize + docsize <= mBlocksize, false);
	// 5. Set the data
	if (data) memcpy(&mData[start + eRcdHeaderSize], data, docsize);

	// 6. Set group index start
	u32 groupid = entryid / eGroupSize;
	//Set a new Group index start 'start'
	if (entryid % eGroupSize == 0)   //enl
	{
		mGroupIdx[groupid] = start;
	}

	u64 offset = mBlockStart + entryid;
	AosDiskBlockCheck;
	saveToDisk(false);//save
	mLock->unlock();
	
	return offset;
}

bool 
AosDiskBlock::moveBackward(
			const u32 crtpos, 
			const u32 delta)
{
	// This function moves all the subsequent docs backward
	// 'delta' number of bytes. If 'overrideflag' is true, 
	// the current record ('crtpos') will be overridden. 
	// Otherwise, only the current record's subsequent
	// docs are moved (not the current record).
	//
	// To move docs, we need to calculate the starting 
	// position of the docs to be moved, and then the 
	// buffer's starting position. The memory to be
	// moved is determined by these two values. 
	
	aos_assert_r(isValidRcd(crtpos), false);
	u32 docsize = getDocsize(crtpos);
	aos_assert_r(docsize >= delta, false);

	u32 movetoPos;
    movetoPos = crtpos + eRcdHeaderSize + docsize - delta;
    //movetoPos -= mBlockStart;
    u32 movefromPos = getNextRcdStart(crtpos);
    u32 bufferpos = getBufferStart();
    aos_assert_r(movefromPos < mBlocksize, false);
    aos_assert_r(movetoPos< mBlocksize, false);
    memmove(&mData[movetoPos], &mData[movefromPos],
		         bufferpos - movefromPos);
	updateIdx(crtpos, -(int)delta);
	increaseBuffer(delta);
	return true;
}

bool 
AosDiskBlock::moveForward(
			const u64 &offset,
			const u32 crtpos, 
			const u32 delta,
			bool &kickedout)
{
	// This function moves all the subsequent docs forward
	// 'delta' number of bytes. If there is not enough 
	// space, however, the doc will be 'kicked' out and
	// all the subsequent docs are moved backward. 
	aos_assert_r(isValidRcd(crtpos), false);
	u32 buffersize = getBufferSize();
	
	if (delta > buffersize)
	{
		// There is not enough space for the doc. 
		// Need to kick the doc out and move all 
		// the subsequent docs backward.
		kickedout = true;
		u32 docsize = getDocsize(crtpos);
		bool rslt = moveBackward(crtpos, docsize);
		aos_assert_r(rslt,false);
		setRcdDocsize(crtpos, 0);
		return true;
	}

	// There is enough space. Need to move the subsequent docs
	u32 startpos = getNextRcdStart(crtpos);
	u32 bufferpos = getBufferStart();
	decreaseBuffer(delta);
	if (startpos != bufferpos &&bufferpos > startpos)
	{
		aos_assert_r(bufferpos-startpos>=0, false);
		memmove(&mData[startpos+delta], &mData[startpos], 
			bufferpos - startpos);
		updateIdx(crtpos, delta);
	}
	return true;
}


bool 
AosDiskBlock::updateIdx(
	const u32 start,
	const int delta)
{
	//Get current gruop id
	u32 entryid = getEntryId(start);
	u32 groupid = entryid / eGroupSize;

	//Set group index for the forword
	for (int i=groupid+1; mGroupIdx[i] && i<=eMaxRecords/eGroupSize; i++)
	{
		mGroupIdx[i] += delta;  
	}
	return true;
}

/*
bool
AosDiskBlock::readDoc(
		const OmnFilePtr file, 
		const u32 offset, 
		char *data, 
		const u32 expected_size)
{
	AosDiskBlockPtr block =  AosDiskBlockMgr::getSelf()->getBlock(file, offset, false);
	aos_assert_r(block, false);
	return block->readRcd(offset, data, expected_size);
}
*/



bool
AosDiskBlock::readRcd(const u64 &offset, char *data, const u32 expected_size)
{
	mLock->lock();
	AosDiskBlockCheck;
	aos_assert_rl(offset >= mBlockStart &&
		offset - mBlockStart < mBlocksize, mLock, false);
	aos_assert_rl(mGroupIdx, mLock, false);
	u64 crtpos= convertToRcdStart(offset);
	aos_assert_rl(crtpos<=getBufferStart()-eRcdBodyOffset && crtpos>=eBodyStart, mLock, false);
	u32 docsize = getDocsize(crtpos);

if (docsize != expected_size)
	OmnMark;
	aos_assert_rl(docsize == expected_size, mLock, false);
	memcpy(data, &mData[crtpos] + eRcdHeaderSize, docsize);
	AosDiskBlockCheck;
	mLock->unlock();
	return true;
}

bool
AosDiskBlock::modifyRcd(
		u64 &offset, 
		const u32 &newsize,
		const char *data,
		bool &kickout) 
{
	// This function is called when a doc [seqno, offset] size 
	// changes. Based on whether the size increased or descreased, 
	// it will either move back or move forward the subsequent
	// docs within the block. The start of the block is calculated:
	// 		block_start = offset / blocksize;
	//
	// IMPORTANT: This class should be locked when this function is called.
	//
	// 1. It reads in the entire block into memory. 
	// 2. Adjust the docs within the block.
	// 3. Returns the block
	
	// 1. Read in the block
	// Blocks are started with a Block Control Record that is
	// in the following format:
	// 		<blocksize>		4 bytes
	// 		<blockflag>		8 bytes (used for error checking)
	// 		<num_records>	4 bytes
	// 		<buffsize>		4 bytes (the size of the buffer)
	// 		<buffstart>		4 bytes (the start position for the buffer)
	//		<reserved>		eBlockReservedSize
	mLock->lock();
if (mBlockStart == 77500000 && offset == 77500063)
{
	OmnScreen << "modify: seq" << endl;
}
	AosDiskBlockCheck;

	u32 crtpos= convertToRcdStart(offset);
	
	aos_assert_rl(crtpos<=getBufferStart()-eRcdBodyOffset && crtpos>=eBodyStart, mLock, false);

	u32 oldsize = getDocsize(crtpos);

	bool rslt;
	if (newsize < oldsize)
	{
		rslt = moveBackward(crtpos, oldsize - newsize);
	}
	else if(newsize>oldsize)
	{
		rslt = moveForward(offset, crtpos, newsize - oldsize, kickout);
	}
	else
	{
		rslt = true;
	}

	if (kickout) 
	{
		AosDiskBlockCheck;

		saveToDisk(false);//save

		mLock->unlock();
		return true;
	}

//OmnScreen<<"Save To Docsize 11 offset:"<<offset<<"  newsize:"<<newsize<<endl;
	setU32(crtpos + eRcdDocsizeOffset, newsize);	
//OmnScreen<<"Save To Docsize 22 offset:"<<offset<<"  newsize:"<<getU32(crtpos + eRcdDocsizeOffset)<<endl;

	if (data) memcpy(&mData[crtpos + eRcdBodyOffset], data, newsize);
	aos_assert_rl(rslt, mLock, 0);
	AosDiskBlockCheck;

	saveToDisk(false);//save	

	mLock->unlock();
	return true;
}

u64
AosDiskBlock::convertToRcdStart(const u64 &offset)
{
	u32 entry_id = offset % eMaxRecords;
	u32 groupid = entry_id / eGroupSize;
	u32 group_offset = entry_id % eGroupSize;
	u32 pos = mGroupIdx[groupid];

	for (u32 i=0; i<group_offset; i++)
	{
		pos += getRcdDisksize(pos);
	}

	return pos;
}

bool
AosDiskBlock::reload(
		const OmnFilePtr &file,
		const u32 seqno,
		const u64 &offset, 
		const u32 blocksize,
		const bool createnew)
{
	mLock->lock();
	saveToDisk(false);
	mFile = file;
	mSeqno = seqno;
	

	mIsGood = false;

	mBlocksize = blocksize;
	mBlockStart = offset / mBlocksize * mBlocksize;

	aos_assert_rl(mFile, mLock, false);
    aos_assert_rl(mData, mLock, false);

	if (createnew)
	{

		createNewBlock();
	}
	else
	{
		mFile->lock();

		int bytesread = mFile->readToBuff(mBlockStart, mBlocksize, (char *) mData);
		mFile->unlock();
		aos_assert_rl(bytesread > 0 && (u32)bytesread == mBlocksize, mLock, false);
		mGroupIdx = (u32*)&mData[eIdxStart];
	}
	mIsGood = true;
	AosDiskBlockCheck;
	mLock->unlock();
	return true;
}


bool
AosDiskBlock::deleteRcd(const u64 &offset)
{
	mLock->lock();
	AosDiskBlockCheck;
	u32 crtpos= convertToRcdStart(offset);
	aos_assert_rl(crtpos<=getBufferStart()-eRcdBodyOffset && crtpos>=eBodyStart, mLock, false);
	u32 oldsize = getDocsize(crtpos);
 	bool rslt = moveBackward(crtpos, oldsize);
	setU32(crtpos + eRcdDocsizeOffset, 0);
	aos_assert_rl(rslt, mLock, 0);

	AosDiskBlockCheck;

	saveToDisk(false);//save


	mLock->unlock();
	return true;
}
