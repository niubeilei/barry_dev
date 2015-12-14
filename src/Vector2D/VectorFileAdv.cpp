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
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Vector2D/VectorFileAdv.h"

#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"



AosVectorFileAdv::AosVectorFileAdv(
		AosRundata *rdata, 
		const u64 cube_id,
		const u64 file_id, 
		const u32 cell_block_size, 
		const u32 num_rows_per_cell_block, 
		const AosRlbFileType::E file_type)
:
AosVectorFile(),
mCubeId(cube_id),
mFileId(file_id),
mFileSize(-1),
mCellBlockSize(cell_block_size),
mNumRowsPerCellBlock(num_rows_per_cell_block),
mNumIndexBlocks(0),
mControlBlockArray(0),
mLastIndexUnusedEntry(-1),
mLastIndexBlockPos(-1),
mLastIndexBlockArray(0),
mFileType(file_type),
mReliableFileRaw(0)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);

	mReliableFile = vfsMgr->openRlbFile(mFileId, mFileType, rdata);
	aos_assert(mReliableFile);
	mReliableFileRaw = mReliableFile.getPtr();
	mFileSize = mReliableFileRaw->getFileCrtSize();
	mIsGood = true;
}


// This constructor creates a new vector file. 
AosVectorFileAdv::AosVectorFileAdv(
		AosRundata *rdata, 
		const u64 cube_id,
		const u32 cell_block_size, 
		const u32 num_rows_per_cell_block, 
		const AosRlbFileType::E file_type)
:
AosVectorFile(),
mCubeId(cube_id),
mFileId(0),
mFileSize(-1),
mCellBlockSize(cell_block_size),
mNumRowsPerCellBlock(num_rows_per_cell_block),
mNumIndexBlocks(0),
mControlBlockArray(0),
mLastIndexUnusedEntry(-1),
mLastIndexBlockPos(-1),
mLastIndexBlockArray(0),
mFileType(file_type),
mReliableFileRaw(0)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);
	mReliableFile = vfsMgr->createRlbFile(
			rdata, mCubeId, "vctfile", 0, mFileType, false);
	aos_assert(mReliableFile);
	mFileId = mReliableFile->getFileId();

	initializeVectorFileAdv(rdata);

	mIsGood = true;
}


AosVectorFileAdv::~AosVectorFileAdv()
{
}


bool
AosVectorFileAdv::initializeVectorFileAdv(AosRundata *rdata)
{
	// It initializes a vector file. The first block in a vector
	// file is the control block. It initializes the control
	// block to be all 0's. 
	//
	// mFileSize must be set to the proper file size.
	
	aos_assert_rr(mReliableFile, rdata, false);
	mControlBlockBuff = OmnNew AosBuff(eControlBlockSize);
	mControlBlockArray = (i64 *)mControlBlockBuff->data();
	char *data = (char *)mControlBlockArray;
	memset(data, 0, eControlBlockSize);
	bool rslt = mReliableFileRaw->write(0, data, eControlBlockSize, rdata);
	aos_assert_rr(rslt, rdata, false);

	mFileSize = eControlBlockSize;

	return true;
}


bool
AosVectorFileAdv::init(AosRundata *rdata)
{
	// 1. Read in the control block
	aos_assert_rr(mReliableFile, rdata, false);
	mControlBlockBuff = OmnNew AosBuff(eControlBlockSize);
	mControlBlockArray = (i64 *)mControlBlockBuff->data();
	char *data = (char *)mControlBlockArray;
	bool rslt = mReliableFileRaw->readToBuff(0, eControlBlockSize, data, rdata);

	// Find the last index block position
	mNumIndexBlocks = 0;
	while (mControlBlockArray[mNumIndexBlocks] > 0 && 
		   mNumIndexBlocks < eMaxIndexBlocks) mNumIndexBlocks++;
	aos_assert_rr(mNumIndexBlocks >= 0 && mNumIndexBlocks < eMaxIndexBlocks, rdata, false);

	if (mNumIndexBlocks == 0)
	{
		mLastIndexBlockBuff = 0;
		mLastIndexBlockArray = 0;
		mLastIndexBlockPos = 0;
		mLastIndexUnusedEntry = 0;
	}
	else
	{
		// It is not an empty vector file. 
		mLastIndexBlockPos = mControlBlockArray[mNumIndexBlocks-1];
		aos_assert_rr(mLastIndexBlockPos < mFileSize, rdata, false);

		// Read the last index block
		mLastIndexBlockBuff = OmnNew AosBuff(eIndexBlockSize);
		mLastIndexBlockArray = (i64 *)mLastIndexBlockBuff->data();
		rslt = mReliableFile->readToBuff(mLastIndexBlockPos, 
			eIndexBlockSize, data, rdata);
		aos_assert_rr(rslt, rdata, false);

		// Scan the index block to the last one
		mLastIndexUnusedEntry = 0;
		while (mLastIndexUnusedEntry < eNumEntriesPerIndexBlock &&
			   mLastIndexBlockArray[mLastIndexUnusedEntry] > 0) mLastIndexUnusedEntry++;
	}

	return true;
}


i64 
AosVectorFileAdv::getCellBlockPos(
		AosRundata *rdata, 
		const u64 sdocid, 
		const bool create_flag)
{
	// Each vector file saves all the cell blocks for the same
	// time id. The file has an index block and zero or more 
	// cell blocks. 
	//
	// Indexes are arranged in Index Blocks. There can be more than
	// one index block. The size of index blocks is eIndexBlockSize.
	// Initially, there shall be only one index block. When this runs
	// out (because more keys are added), a new index block is appended 
	// to the end of the file. 
	//
	// There is a control block that is an index for index blocks. 
	// Control block size is fixed (eControlBlockSize).
	//
	// The control block structure is:
	// 		[index for index block]
	// 		[index for index block]
	// 		...
	// 		[index for index block]		
	// 
	// The structure of vector files is therefore:
	// 		[control block]
	// 		[index block or cell block]
	// 		[index block or cell block]
	// 		...
	//
	// Index block is defined as:
	// 		[offset for cell block]
	// 		[offset for cell block]
	// 		...
	// 		[offset for cell block]
	int sdocid_block_idx = sdocid / mNumRowsPerCellBlock;
	int sdocid_offset = sdocid % mNumRowsPerCellBlock;
	int index_block_idx = sdocid_block_idx / eNumEntriesPerIndexBlock;
	if (index_block_idx >= mNumIndexBlocks) 
	{
		// The cell block does not exist
		if (!create_flag) return -1;

		return createCellBlock(rdata, sdocid);
	}

	i64 index_block_pos = mControlBlockArray[index_block_idx];
	aos_assert_rr(index_block_pos >= 0, rdata, false);
	aos_assert_rr(index_block_pos + eIndexBlockSize <= mFileSize, rdata, false);

	// Retrieve the index block, either from the map or from the file.
	itr2_t itr = mIndexBlockMapRaw.find((u64)index_block_pos);
	i64 *index_block = 0;
	if (itr == mIndexBlockMapRaw.end())
	{
		// It is not read in memory yet. 
		AosBuffPtr bb = OmnNew AosBuff(eIndexBlockSize);
		char *dd = (char *)bb->data();
		bool rslt = mReliableFileRaw->readToBuff(
			index_block_pos, eIndexBlockSize, dd, rdata);
		aos_assert_rr(rslt, rdata, false);

		mIndexBlockMap[index_block_pos] = bb;
		mIndexBlockMapRaw[index_block_pos] = (i64*)bb->data();
		index_block = (i64*)bb->data();
	}
	else
	{
		index_block = itr->second;
	}

	i64 cell_block_pos = index_block[sdocid_offset];
	if (cell_block_pos > 0) return cell_block_pos;

	if (cell_block_pos == -1)
	{
		if (!create_flag) return -1;
	}

	aos_assert_rr(cell_block_pos > 0, rdata, false);
	return cell_block_pos;
}


bool
AosVectorFileAdv::readCellBlock(
		AosRundata *rdata, 
		const u64 sdocid, 
		AosBuffPtr &buff)
{
	// This function reads a cell block. If the cell block does not
	// exist, 'buff' is set to 0 and true is returned. 
	buff = 0;
	i64 cell_block_pos = getCellBlockPos(rdata, sdocid, false);
	if (cell_block_pos == -1) 
	{
		// The cell block does not exist.
		return true;
	}
	
	buff = OmnNew AosBuff(mCellBlockSize);
	char *data = buff->data();

	bool rslt = mReliableFileRaw->readToBuff(
			cell_block_pos, mCellBlockSize, data, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosVectorFileAdv::writeCell(
		AosRundata *rdata, 
		const u64 sdocid, 
		AosBuff *buff)
{
	// This function writes the cell 'buff' into the vector file. 
	// 1. Check whether the cell block exists. If not, it needs to 
	//    create it.
	// 2. It overrites the cell block.
	i64 cell_block_pos = getCellBlockPos(rdata, sdocid, true);
	aos_assert_rr(cell_block_pos, rdata, false);

	return mReliableFile->write(cell_block_pos, buff->data(), mCellBlockSize, true);
}


i64
AosVectorFileAdv::createCellBlock(
		AosRundata *rdata, 
		const u64 sdocid)
{
	// 1. It appends an empty cell block
	// 2. Update mFileSize
	// 3. Find the last index block. 
	// 4. If the last index block is full, create a new index block.
	// 5. Add the index entry to the last index block.

	// 1. Appends an empty cell block
	AosBuffPtr buff = OmnNew AosBuff(mCellBlockSize);
	char *data = buff->data();
	memset(data, 0, mCellBlockSize);

	bool rslt = mReliableFile->append(data, mCellBlockSize, false);
	aos_assert_rr(rslt, rdata, -1);

	// 2. Update mFileSize
	i64 cell_block_pos = mFileSize;
	mFileSize += mCellBlockSize;

	// 3. Find the last index block
	aos_assert_rr(mLastIndexBlockArray, rdata, -1);
	if (mLastIndexUnusedEntry >= eNumEntriesPerIndexBlock)
	{
		// Runs out. Need to create a new index block. 
		mLastIndexBlockBuff = OmnNew AosBuff(eIndexBlockSize);
		data = mLastIndexBlockBuff->data();
		memset(data, 0, eIndexBlockSize);
		mLastIndexBlockArray = (i64*)mLastIndexBlockBuff->data();
		mLastIndexBlockArray[0] = cell_block_pos;
		mLastIndexUnusedEntry = 1;

		// Save the newly created index block
		i64 index_block_pos = mFileSize;
		rslt = mReliableFile->append(data, eIndexBlockSize, false);
		aos_assert_rr(mNumIndexBlocks < eMaxIndexBlocks, rdata, -1);
		mControlBlockArray[mNumIndexBlocks++] = index_block_pos;

		// Save the modifications to the control block
		mReliableFile->write(0, (char *)mControlBlockArray, eControlBlockSize, true);
	}
	else
	{
		// Update the index block and save the changes
		mLastIndexBlockArray[mLastIndexUnusedEntry++] = cell_block_pos;
		mReliableFile->write(mLastIndexBlockPos, (char *)mLastIndexBlockArray, 
				eIndexBlockSize, true);
	}

	return cell_block_pos;
}
#endif
