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
// Each block saves eMaxRecords (2500) number of records. Records are 
// packed one after another, which means once one record size is changed, 
// all the subsequent records need to be moved accordingly.
//
// Records are identified by:
// 		Block Start Offset + Record ID
// Block Start Offset is a multiplication of 'Block Size'. As an example,
// if the block size is 1000, blocks can start at 1000, 2000, ..., only.
// Note that block size cannot be smaller than eMaxRecords.
//
// Modification History:
// 07/18/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DiskBlock.h"

#include "ReliableFile/ReliableFile.h"
#include "Thread/Mutex.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"

static u64 sgNumReadsTotal = 0;
static u64 sgNumReadsLast = 0;
static u64 sgBytesReadTotal = 0;
static u64 sgBytesReadLast = 0;
static u64 sgNumWritesTotal = 0;
static u64 sgNumWritesLast = 0;
static u64 sgBytesWrittenTotal = 0;
static u64 sgBytesWrittenLast = 0;
static u32 sgNextLogSec = 0;
static u32 sgLogFreq = 5;
static bool sgMonitorFiles = false;
static void sgReadStat(const u32 size)
{
	sgNumReadsTotal++;
	sgNumReadsLast++;
	sgBytesReadTotal += size;
	sgBytesReadLast += size;
	if (OmnGetSecond() > sgNextLogSec)
	{
		OmnScreen << "File Stat: " << endl
			<< "Reading: " << sgNumReadsTotal << ":" << sgNumReadsLast
			<< ":" << sgBytesReadTotal/1000 << "K:" << sgBytesReadLast/1000 << "K" << endl
			<< "Writing: " << sgNumWritesTotal << ":" << sgNumWritesLast
			<< ":" << sgBytesWrittenTotal/1000 << "K:" << sgBytesWrittenLast/1000 << "K" << endl;
		sgNextLogSec = OmnGetSecond() + sgLogFreq;
		sgNumReadsLast = 0;
		sgBytesReadLast = 0;
		sgNumWritesLast = 0;
		sgBytesWrittenLast = 0;
	}
}

static void sgWriteStat(const u32 size)
{
	sgNumWritesTotal++;
	sgNumWritesLast++;
	sgBytesWrittenTotal += size;
	sgBytesWrittenLast += size;
	if (OmnGetSecond() > sgNextLogSec)
	{
		OmnScreen << "File Stat: " << endl
			<< "Reading: " << sgNumReadsTotal << ":" << sgNumReadsLast
			<< ":" << sgBytesReadTotal/1000 << "K:" << sgBytesReadLast/1000 << "K" << endl
			<< "Writing: " << sgNumWritesTotal << ":" << sgNumWritesLast
			<< ":" << sgBytesWrittenTotal/1000 << "K:" << sgBytesWrittenLast/1000 << "K" << endl;
		sgNextLogSec = OmnGetSecond() + sgLogFreq;
		sgNumReadsLast = 0;
		sgBytesReadLast = 0;
		sgNumWritesLast = 0;
		sgBytesWrittenLast = 0;
	}
}


//static int sgBlocks = 0;
bool AosDiskBlock2::smSanityCheck = false;

/*
AosDiskBlock2::AosDiskBlock2(
		const AosRundataPtr &rdata,
		const BlockId &block_id,
		//const u32 virtual_id,
		//const u32 docFileMgrId,
		//const OmnFilePtr &file,
		const AosReliableFilePtr &file,
		const u32 seqno, 
		const u64 &offset, 
		const u32 blocksize,
		const bool flag)
*/
AosDiskBlock2::AosDiskBlock2(
		const AosRundataPtr &rdata,
		const BlockId &block_id,
		const AosReliableFilePtr &file,
		const bool flag)
:
mBlockId(block_id),
//mVirtualId(virtual_id),
//mDocFileMgrId(docFileMgrId),
//mBlocksize(blocksize),
//mBlockStart(offset),
mData(0),
mFile(file),
mGroupIdx(0),
mIsGood(false),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRefCount(0)
{
	mData = OmnNew unsigned char[getBlockSize()+1];
	aos_assert(mData);

	// If 'flag' is true, it creates a block. Otherwise, it reads in 
	// an existing block.
	//OmnScreen << "New Block:" 
	//		<< "seqno: " << seqno << "; "
	//		<< "offset: " << offset << "; "
	//		<< endl;
	if (flag)
	{
		// Create a new block
		mIsGood = createNewBlock();
	}
	else
	{
		mIsGood = readBlock(rdata);
	}
}


AosDiskBlock2::~AosDiskBlock2()
{
	//OmnScreen << "Delete a block: " << sgBlocks-- << endl;
	OmnDelete [] mData;
	mData = 0;
	mGroupIdx = 0;
}


///////////////////////// Public Member Functions ////////////////////////////
bool
//AosDiskBlock2::readRcd(const u64 &offset, char *data, const u32 expected_size)
//AosDiskBlock2::readRcd(const u64 &offset, AosBuffPtr &data_buff)
AosDiskBlock2::readRcd(
		const u64 &offset,
		AosBuffPtr &data_buff,
		u32 &orig_len,
		u32 &compress_len)
{
	mLock->lock();
	aos_assert_rl(offset >= getBlockStart() &&
		offset - getBlockStart() < getBlockSize(), mLock, false);
	aos_assert_rl(mGroupIdx, mLock, false);
	u64 crtpos= convertToRcdStart(offset);
	aos_assert_rl(crtpos<=getBufferStart()-eRcdBodyOffset && crtpos>=eBodyStart, mLock, false);

	// Ketty 2013/01/26
	//u32 docsize = getDocsize(crtpos);
	orig_len = getRcdOrigLen(crtpos);
	compress_len = getRcdCompressLen(crtpos);
	u32 docsize = compress_len ? compress_len : orig_len;

	data_buff = OmnNew AosBuff(docsize, 0 AosMemoryCheckerArgs);
	/*
	if (docsize != expected_size)
	{
		mLock->unlock();
		OmnAlarm << "size mismatch: " << docsize << ":" << expected_size << enderr;
		return false;
	}
	*/
	memcpy(data_buff->data(), &mData[crtpos] + eRcdHeaderSize, docsize);
	mLock->unlock();
	data_buff->setDataLen(docsize);
	
	//OmnScreen << "kttt DiskBlock readRcd:"
	//		<< "; virtual_id:" << mBlockId.virtualId
	//		<< "; dfm_id:" << mBlockId.dfmId
	//		<< "; file_seqno:" << mBlockId.fileSeqno
	//		<< "; offset:" << offset
	//		<< "; block_size:" << mBlockId.blockSize
	//		<< "; block_start:" << mBlockId.blockStart
	//		<< "; file:" << mFile->getFileName()
	//		<< "; mData:" << mData
	//		<< "; num_rcd:" << getNumRcd()
	//		<< endl;

	return true;
}


u64
//AosDiskBlock2::addRcd(const AosRundataPtr &rdata, const u32 docsize, const char *data)
AosDiskBlock2::addRcd(
		const AosRundataPtr &rdata, 
		const char *data,
		const u32 orig_len,
		const u32 compress_len)
{
	// This function adds a record into the block. If failed, it 
	// returns 0. Otherwise, it returns the offset.
	// 1. Make sure the buffer is big enough for the record
	// 2. Get the buffer start
	// 3. Create the record
	// 4. Modify the buffer
	
	// Ketty 2013/01/26
	u32 docsize = compress_len ? compress_len : orig_len;

	aos_assert_r(data, 0);
	aos_assert_r(docsize > 0, 0);
	mLock->lock();
	AosDiskBlockCheck;
	aos_assert_rl(mGroupIdx, mLock, false);

	// 1. Set entry id. Make sure entryid less than Max Records
	u32 entryid = getNewEntryid();
	if (entryid >= eMaxRecords)
	{
		mLock->unlock();
		return 0;
	}

	//OmnScreen << "DiskBlock, addRcd:"
	//		<< "docFileMgrId:" << mDocFileMgrId << "; "
	//		<< "seqno: " << mSeqno << "; "
	//		<< "entryid:" << entryid << "; "
	//		<< endl;

	u32 start = getBufferStart();

	// 2. Make sure the buffer is big enough for the record
	u32 buffersize = getBufferSize();
	if (buffersize < docsize+eRcdHeaderSize)
	{
		// There is not enough room for the record.
		mLock->unlock();
	
		//OmnScreen << "kttt DiskBlock addRcd not enough room:"
		//	<< "; virtual_id:" << mBlockId.virtualId
		//	<< "; dfm_id:" << mBlockId.dfmId
		//	<< "; file_seqno:" << mBlockId.fileSeqno
		//	<< "; block_size:" << mBlockId.blockSize
		//	<< "; block_start:" << mBlockId.blockStart
		//	<< "; file:" << mFile->getFileName()
		//	<< "; mData:" << mData
		//	<< "; num_rcd:" << getNumRcd()
		//	<< endl;

		return 0;
	}

	// 3. Modify the control information
	setNumRcd(entryid+1);
	setEntryId(start, entryid);
	decreaseBuffer(docsize + eRcdHeaderSize);
	setFlag(start);
	setReserved(start);
	aos_assert_rl(start + eRcdHeaderSize + docsize <= getBlockSize(), mLock, false);

	// Ketty 2013/01/26
	// 4. Set docsize
	// setRcdDocsize(start, docsize);
	// 4.1 set orig_len.
	setRcdOrigLen(start, orig_len);
	// 4.2 set compress_len
	setRcdCompressLen(start, compress_len);
	
	// 5. Set the data
	memcpy(&mData[start + eRcdHeaderSize], data, docsize);

	// 6. Set group index start
	u32 groupid = entryid / eGroupSize;

	//Set a new Group index start 'start'
	if (entryid % eGroupSize == 0)
	{
		mGroupIdx[groupid] = start;
	}

	u64 offset = getBlockStart() + entryid;
	
	saveToDiskPriv(rdata);	// Ketty 2012/09/26 will delete.
	AosDiskBlockCheck;
	if (smSanityCheck) 
	{
		aos_assert_r(checkSaving(rdata, offset, data, docsize), false);
		u64 len = mFile->getLength();
		aos_assert_rl(offset <= len, mLock, false);
	}
	mLock->unlock();
	
	//OmnScreen << "kttt DiskBlock addRcd:"
	//		<< "; virtual_id:" << mBlockId.virtualId
	//		<< "; dfm_id:" << mBlockId.dfmId
	//		<< "; file_seqno:" << mBlockId.fileSeqno
	//		<< "; offset:" << offset
	//		<< "; block_size:" << mBlockId.blockSize
	//		<< "; block_start:" << mBlockId.blockStart
	//		<< "; file:" << mFile->getFileName()
	//		<< "; mData:" << mData
	//		<< "; num_rcd:" << getNumRcd()
	//		<< endl;

	return offset;
}


bool
AosDiskBlock2::modifyRcd(
		const AosRundataPtr &rdata,
		u64 &offset, 
		//const u32 &newsize,
		const char *data,
		bool &kickout, 
		const u32 orig_len,
		const u32 compress_len)
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
	aos_assert_r(data, false);
	mLock->lock();
	AosDiskBlockCheck;

	u32 crtpos= convertToRcdStart(offset);
	aos_assert_rl(crtpos<=getBufferStart()-eRcdBodyOffset && crtpos>=eBodyStart, mLock, false);
	u32 oldsize = getDocsize(crtpos);
	
	// Ketty 2013/01/26
	u32 newsize = compress_len ? compress_len : orig_len;

	kickout = false;
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
		saveToDiskPriv(rdata);	// Ketty 2012/09/26 will delete. 
		AosDiskBlockCheck;
		mLock->unlock();
		
		//OmnScreen << "kttt DiskBlock modifyRcd kickout:"
		//		<< "; virtual_id:" << mBlockId.virtualId
		//		<< "; dfm_id:" << mBlockId.dfmId
		//		<< "; file_seqno:" << mBlockId.fileSeqno
		//		<< "; offset:" << offset
		//		<< "; block_size:" << mBlockId.blockSize
		//		<< "; block_start:" << mBlockId.blockStart
		//		<< "; file:" << mFile->getFileName()
		//		<< "; mData:" << mData
		//		<< "; num_rcd:" << getNumRcd()
		//		<< endl;

		return true;
	}

	// Ketty 2013/01/26
	//setU32(crtpos + eRcdDocsizeOffset, newsize);
	// 1 set orig_len.
	setRcdOrigLen(crtpos, orig_len);
	// 2 set compress_len
	setRcdCompressLen(crtpos, compress_len);
	
	memcpy(&mData[crtpos + eRcdBodyOffset], data, newsize);
	aos_assert_rl(rslt, mLock, 0);
	AosDiskBlockCheck;

	saveToDiskPriv(rdata);	// Ketty 2012/09/26 will delete.
	if (smSanityCheck) 
	{
		aos_assert_r(checkSaving(rdata, offset, data, newsize), false);
		u64 len = mFile->getLength();
		aos_assert_rl(offset < len, mLock, false);
	}
	mLock->unlock();
	
	//OmnScreen << "kttt DiskBlock modifyRcd:"
	//		<< "; virtual_id:" << mBlockId.virtualId
	//		<< "; dfm_id:" << mBlockId.dfmId
	//		<< "; file_seqno:" << mBlockId.fileSeqno
	//		<< "; offset:" << offset
	//		<< "; block_size:" << mBlockId.blockSize
	//		<< "; block_start:" << mBlockId.blockStart
	//		<< "; file:" << mFile->getFileName()
	//		<< "; mData:" << mData
	//		<< "; num_rcd:" << getNumRcd()
	//		<< endl;

	return true;
}


bool
AosDiskBlock2::deleteRcd(const AosRundataPtr &rdata, const u64 &offset)
{
	mLock->lock();
	AosDiskBlockCheck;
	u32 crtpos= convertToRcdStart(offset);
	aos_assert_rl(crtpos<=getBufferStart()-eRcdBodyOffset && crtpos>=eBodyStart, mLock, false);
	u32 oldsize = getDocsize(crtpos);
 	bool rslt = moveBackward(crtpos, oldsize);
	aos_assert_rl(rslt, mLock, 0);
	// Ketty 2013/01/26
	//setU32(crtpos + eRcdDocsizeOffset, 0);
	// 1 set orig_len.
	setRcdOrigLen(crtpos, 0);
	// 2 set compress_len
	setRcdCompressLen(crtpos, 0);

	AosDiskBlockCheck;
	saveToDiskPriv(rdata);	// Ketty 2012/09/26 will delete.
	
	//OmnScreen << "kttt DiskBlock deleteRcd:"
	//		<< "; virtual_id:" << mBlockId.virtualId
	//		<< "; dfm_id:" << mBlockId.dfmId
	//		<< "; file_seqno:" << mBlockId.fileSeqno
	//		<< "; offset:" << offset
	//		<< "; block_size:" << mBlockId.blockSize
	//		<< "; block_start:" << mBlockId.blockStart
	//		<< "; file:" << mFile->getFileName()
	//		<< "; mData:" << mData
	//		<< "; num_rcd:" << getNumRcd()
	//		<< endl;

	mLock->unlock();
	return true;
}

	
/*
bool
AosDiskBlock2::reload(
		const AosRundataPtr &rdata,
		const BlockId &block_id,
		//const u32 virtual_id,
		//const u32 docFileMgrId,
		//const OmnFilePtr &file,
		const AosReliableFilePtr &file,
		const u32 seqno,
		const u64 &offset, 
		const u32 blocksize,
		const bool createnew)
*/
bool
AosDiskBlock2::reload(
		const AosRundataPtr &rdata,
		const BlockId &block_id,
		const AosReliableFilePtr &file,
		const bool create_new)
{
	// modifyed by ketty 2011/07/26
	mLock->lock();
	aos_assert_rl(mFile && getBlockSize() == block_id.blockSize, mLock, false);
	
	mBlockId = block_id;
	mFile = file;
	mGroupIdx = 0;
	mIsGood = false;
	
	if (create_new)
	{
		mIsGood = createNewBlock();
	}
	else
	{
		mIsGood = readBlock(rdata);
	}
	
	AosDiskBlockCheck;
	mLock->unlock();
	return true;
}


///////////////////////// Private Member Functions  /////////////////////////////

bool
AosDiskBlock2::createNewBlock()
{
	// 2. Set block size 
	setBlocksize(getBlockSize());

	// Ketty 2012/09/10
	// 3. Set the flag bytes
	//mData[eBlockFlagOffset] = eBlockFlagByte0;
	//mData[eBlockFlagOffset+1] = eBlockFlagByte1;
	//mData[eBlockFlagOffset+2] = eBlockFlagByte2;
	//mData[eBlockFlagOffset+3] = eBlockFlagByte3;

	// 4. Set number of records
	setNumRcd(0);

	// 5. Set buffer 
	setBufferStart(eBodyStart);
	setBufferSize(getBlockSize() - eBodyStart - eRcdHeaderSize);

	// 6. Get the group index array
	mGroupIdx = (u32*)&mData[eIdxStart];
	memset(mGroupIdx, 0, sizeof(u32) * eMaxRecords/eGroupSize+1);
	mGroupIdx[0] = eBodyStart;
	// 7. Create the buffer record
	u32 start = getBufferStart();
	// Ketty 2013/01/26
	//setRcdDocsize(eBodyStart, 0);
	setRcdOrigLen(eBodyStart, 0);
	setRcdCompressLen(eBodyStart, 0);
	setFlag(start);
	setReserved(start);

 	setEntryId(start,0xffffffff);	
	AosDiskBlockCheck;
	return true;
}


bool
AosDiskBlock2::readBlock(const AosRundataPtr &rdata)
{
	aos_assert_r(mFile, false);

	// 1. Allocate the memory for the block
	aos_assert_r(mData, false);

	mFile->readlock();
	if (sgMonitorFiles) sgReadStat(getBlockSize());
	
	bool rslt  = mFile->readToBuff(getBlockStart(), getBlockSize(), (char *) mData, rdata.getPtr());
	mFile->unlock();
	aos_assert_r(rslt, false);
	
	//OmnScreen << "kttt DiskBlock readBlock:"
	//		<< "; virtual_id:" << mBlockId.virtualId
	//		<< "; dfm_id:" << mBlockId.dfmId
	//		<< "; file_seqno:" << mBlockId.fileSeqno
	//		<< "; block_size:" << mBlockId.blockSize
	//		<< "; block_start:" << mBlockId.blockStart
	//		<< "; file:" << mFile->getFileName()
	//		<< "; mData:" << mData
	//		<< "; num_rcd:" << getNumRcd()
	//		<< endl;

	// Ketty 2012/09/10
	bool data_good = checkSign();
	if(!data_good)
	{
		// This block is damaged.
		rslt = mFile->recover(rdata.getPtr());
		aos_assert_r(rslt, false);

		rslt = mFile->readToBuff(getBlockStart(), getBlockSize(), (char *) mData, rdata.getPtr());
		aos_assert_r(rslt, false);
		data_good = checkSign();
		if(!data_good)
		{
			OmnAlarm << "The diskblock file is damaged." << mFile->getFileId() << enderr;
			return false;
		}
	}

	mGroupIdx = (u32*)&mData[eIdxStart];
	AosDiskBlockCheck;
	return true;
}


bool 
AosDiskBlock2::moveBackward(
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
    // movetoPos -= mBlockStart;
    u32 movefromPos = getNextRcdStart(crtpos);
    u32 bufferpos = getBufferStart();
    aos_assert_r(movefromPos < getBlockSize(), false);
    aos_assert_r(movetoPos< getBlockSize(), false);
    memmove(&mData[movetoPos], &mData[movefromPos], bufferpos - movefromPos);
	updateIdx(crtpos, -(int)delta);
	increaseBuffer(delta);
	return true;
}


bool 
AosDiskBlock2::moveForward(
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
	
	kickedout = false;
	if (delta > buffersize)
	{
		// There is not enough space for the doc. 
		// Need to kick the doc out and move all 
		// the subsequent docs backward.
		kickedout = true;

		// Ice 2013/5/17
		//// Ketty 2013/01/26
		//u32 docsize = getDocsize(crtpos);

		//bool rslt = moveBackward(crtpos, docsize);
		//aos_assert_r(rslt,false);
		//// Ketty 2013/01/26
		////setRcdDocsize(crtpos, 0);
		//setRcdOrigLen(crtpos, 0);
		//setRcdCompressLen(crtpos, 0);
		return true;
	}

	// There is enough space. Need to move the subsequent docs
	u32 startpos = getNextRcdStart(crtpos);
	u32 bufferpos = getBufferStart();
	decreaseBuffer(delta);
	if (startpos != bufferpos &&bufferpos > startpos)
	{
		aos_assert_r(bufferpos-startpos>=0, false);
		memmove(&mData[startpos+delta], &mData[startpos], bufferpos - startpos);
		updateIdx(crtpos, delta);
	}
	return true;
}


bool 
AosDiskBlock2::updateIdx(
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


u64
AosDiskBlock2::convertToRcdStart(const u64 &offset)
{
	u32 entry_id = offset % eMaxRecords;
	u32 groupid = entry_id / eGroupSize;
	u32 group_offset = entry_id % eGroupSize;
	u32 pos = mGroupIdx[groupid];

	//OmnScreen << "entry_id:" << entry_id << "; "
	//		<< "groupid:" << groupid << "; "
	//		<< "group_offset:" << group_offset << "; "
	//		<< "pos:" << pos << endl;
	for (u32 i=0; i<group_offset; i++)
	{
		pos += getRcdDisksize(pos);
	}

	return pos;
}

	
bool
AosDiskBlock2::saveToDiskPriv(const AosRundataPtr &rdata)
{
	if (!(sanityCheck()))
	{
		OmnAlarm << "Failed to check!" << enderr;
	}

	if (!mFile)
	{
		OmnAlarm << "Failed to read the file!" << enderr;
	}
	mFile->writelock();

	if (sgMonitorFiles) sgWriteStat(getBlockSize());
	setSign();	// Ketty 2012/09/10
	mFile->put(getBlockStart(), (char *)mData, getBlockSize(), true,  rdata.getPtr());

	mFile->unlock();
	return true;
}


bool 
AosDiskBlock2::sanityCheck() const
{
	if(!smSanityCheck)
	{
		return true;
	}

	//Check Block
	aos_assert_r(mFile, false);
	aos_assert_r(getU32(eBlockSizeOffset) == getBlockSize(), false);
	//aos_assert_r(isBlockGood(), false);
	aos_assert_r(getBlockStart()% getBlockSize() == 0, false);
	aos_assert_r(mGroupIdx != 0, false);
	aos_assert_r(getBufferSize()+getBufferStart() + eRcdHeaderSize == getBlockSize(),false);
	//aos_assert_r(mData[eBlockFlagOffset]==eBlockFlagByte0 &&
	//		mData[eBlockFlagOffset+1]==eBlockFlagByte1 &&
	//		mData[eBlockFlagOffset+2]==eBlockFlagByte2 &&
	//		mData[eBlockFlagOffset+3]==eBlockFlagByte3, false);

	//Check Records
	u32 numrcds = getNumRcd();
	u32 pos = eBodyStart;
	for (u32 i=0; i<numrcds; i++)
	{
		if (i % eGroupSize == 0)
		{
			aos_assert_r(pos == mGroupIdx[i/eGroupSize], false);
		}
		aos_assert_r(isValidRcd(pos), false);
		pos += getRcdDisksize(pos);
	}
	return true;
}


bool
AosDiskBlock2::checkSaving(
		const AosRundataPtr &rdata,
		const u64 &offset, 
		const char *data, 
		const u32 docsize)
{
	// Ketty 2012/10/23
	return true;
	/*
	u32 blockStart = (offset/mBlocksize) * mBlocksize;
	//AosDiskBlock2 block(rdata, mVirtualId, mDocFileMgrId, mFile, mSeqno, blockStart, mBlocksize, false);
	AosDiskBlock2 block(rdata, mFile, mSeqno, blockStart, mBlocksize, false);
	char *buff = new char[docsize];
	bool rslt = block.readRcd(offset, buff, docsize);
	aos_assert_r(rslt, false);
	aos_assert_r(memcmp(buff, data, docsize) == 0, false);
	delete buff;
	return true;
	*/
}


// Ketty 2012/09/22
bool
AosDiskBlock2::isNeedKickOut(
		const AosRundataPtr &rdata, 
		const u64 offset,
		const u64 new_docsize, 
		bool &need_kickout)
{
	u32 crtpos= convertToRcdStart(offset);
	aos_assert_rl(crtpos<=getBufferStart()-eRcdBodyOffset && crtpos>=eBodyStart, mLock, false);
	u32 oldsize = getDocsize(crtpos);
	
	need_kickout = false;
	if (new_docsize < oldsize)
	{
		return true;
	}
	
	u32 buffersize = getBufferSize();
	u32 remain_size = buffersize + (new_docsize - oldsize);
	if (new_docsize > remain_size)
	{
		need_kickout = true;
	}
	return true;
}
	
// Ketty 2014/02/21
u64
AosDiskBlock2::getFileId()
{
	return mFile->getFileId();
}


