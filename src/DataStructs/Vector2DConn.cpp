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
#include "DataStructs/Vector2DConn.h"

#include "API/AosApi.h"
#include "DataStructs/Ptrs.h"
#include "DataStructs/VectorFile.h"
#include "ReliableFile/ReliableFile.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"


AosVector2DConn::AosVector2DConn(
		const u64 stat_docid,
		const u64 cube_id, 
		const u32 rows_per_cell,
		const u32 columns_per_cell)
:
mStatDocid(stat_docid),
mCubeId(cube_id),
mRowsPerCell(rows_per_cell),
mColumnsPerCell(columns_per_cell)
{
}


AosVector2DConn::~AosVector2DConn()
{
}


bool
AosVector2DConn::readCellBlock(
	AosRundata *rdata,
	const u64 sdocid, 
	const u64 timeid,
	AosBuffPtr &buff)
{
	// This function reads in the cell block in which the entry
	// [sdocid, timeid] is stored.
	// If the cell block has not been created yet, it returns null.
	AosVectorFile *vector_file = getVectorFile(rdata, timeid, false);
	buff = 0;
	if (!vector_file)
	{
		// The cell has not been created yet. 
		return true;
	}

	return vector_file->readCellBlock(rdata, sdocid, buff);
}


bool
AosVector2DConn::writeCell(
		AosRundata *rdata, 
		const u64 sdocid, 
		const u64 timeid,
		AosBuff *buff)
{
	// This function writes the cell 'buff' to the file. If the
	// cell has not been created yet, it will create it. 
	AosVectorFile *vector_file = getVectorFile(rdata, timeid, true);
	aos_assert_rr(vector_file, rdata, false);

	return vector_file->writeCell(rdata, sdocid, buff);
}


#if 0
u32
AosVector2DConn::getCellIdx(
		AosRundata *rdata,
		const u64 sdocid, 
		const u64 timeid, 
		const bool create_flag)
{
	// Given [sdocid, timeid], this function retrieves the cell idx.
	// If the cell does not exist, it returns -1. Otherwise, it
	// returns the cell idx. 
	// If any error occurred, it returns false. Otherwise, it 
	// returns true.
	// 
	// The class maintains a map:
	// 	[sdocid_block, timeid] to [cell_id]
	u32 sdocid_block_id = getSdocidBlockId(sdocid);
	u32 time_block_id = getTimeBlockId(timeid);
	u64 entry_id = (sdocid_block_id << 32) + time_block_id;

	// It binary searches mEntryIdArray to see whether
	// 'entry_id' exists in the array. If not, the cell 
	// does not exist. Otherwise, it retrieves the cell
	// id and file id.
	int idx = AosBinarySearchOnU64Array(
			rdata, mEntryIDsArray, mNumCells, entry_id);
	if (idx == -1) return -1;
	return mEntryIDs2CellIdxArray[idx];

	/*
	aos_assert_rr(idx >= 0 && idx < mNumCells, rdata, false);
	u64 file_id = mEntryIDs2FileIDsArray[idx];

	mapitr_t itr = mFileMapRaw.find(file_id);
	if (itr != mFileMapRaw.end()) 
	{
		if (itr->second->isGood()) return itr->second;
		return 0;
	}

	// It is not in the map yet. Open it.
	AosVectorFilePtr vector_file = OmnNew AosVectorFile(rdata, file);
	mFileMap[file_id] = vector_file;
	mFileMapRaw[file_id] = vector_file.getPtr();
	if (vector_file->isGood()) return vector_file;
	return 0;
	*/
}
#endif


AosVectorFile *
AosVector2DConn::getVectorFile(
		AosRundata *rdata,
		const u64 timeid, 
		const bool create_flag) 
{
	// Given [timeid], this function retrieves the vector file
	// for the time id. If the file does not exist yet, it will
	// create it.
	 
	u32 time_block_id = getTimeBlockId(timeid);
	itr3_t tt = mTimeId2FileIdMap.find(time_block_id);
	u64 file_id = 0;
	if (tt != mTimeId2FileIdMap.end())
	{
		file_id = tt->second;
	}

	if (file_id == 0)
	{
		// Did not find it. 
		if (!create_flag) return 0;
		return createVectorFile(rdata, time_block_id);
	}

	itr2_t itr = mFileMapRaw.find(file_id);
	if (itr != mFileMapRaw.end()) 
	{
		if (itr->second->isGood()) return itr->second;
		return 0;
	}

	// It is not in the map yet. Open it.
	AosVectorFilePtr vector_file = OmnNew AosVectorFile(
			rdata, mCubeId, file_id, mCellBlockSize, 
			mRowsPerCell, AosRlbFileType::eNormal);
	if (vector_file->isGood()) return vector_file.getPtr();
	return 0;
}


AosVectorFile *
AosVector2DConn::createVectorFile(
		AosRundata *rdata, 
		const u32 time_block_id)
{
	// This function creates a vector file for the time block id. 
	// 1. Create the file
	// 2. Modify the map [mTimeIDsArray, mFileIDsArray].
	AosVectorFilePtr vector_file = OmnNew AosVectorFile(
			rdata, mCubeId, mCellBlockSize, 
			mRowsPerCell, AosRlbFileType::eNormal);
	aos_assert_rr(vector_file, rdata, false);
	u64 file_id = vector_file->getFileId();

	mFileMap[file_id] = vector_file;
	mFileMapRaw[file_id] = vector_file.getPtr();

	// Add the entry [fime_block_id, file_id] to 
	// [mTimeIDsArray, mFileIDsArray]
	// if ((int)(sizeof(u32)*(mNumTimeIDs+1)) >= mTimeIDsBuff->buffLen())
	// {
	// 	mTimeIDsBuff->expandMemory1(100000);
	// 	mTimeIDsArray = (u32 *)mTimeIDsBuff->data();
	// }
	mTimeIDsArray[mNumTimeIDs] = time_block_id;

	// if ((int)(sizeof(u64)*(mNumTimeIDs+1)) >= mFileIDsBuff->buffLen())
	// {
	// 	mFileIDsBuff->expandMemory1(100000);
	// 	mFileIDsArray = (u64 *)mFileIDsBuff->data();
	// }
	mFileIDsArray[mNumTimeIDs] = file_id;
	mNumTimeIDs++;
	mTimeIDsFile->setU32(mTimeIDsFileSize, time_block_id, true, rdata);
	mFileIDsFile->setU64(mFileIDsFileSize, file_id, true, rdata);
	mTimeIDsFileSize += sizeof(u32);
	mFileIDsFileSize += sizeof(u64);

	return vector_file.getPtr();
}


bool
AosVector2DConn::readMetadata(AosRundata *rdata)
{
	// Each Vector2D has two metafiles:
	// 	TimeID meta file: It is an array of u32
	// 	FileID meta file: it is an array of u64.
	// These two files define a map that maps time block ids
	// to file ids. 
	bool rslt = readTimeIDsMetaFile(rdata);
	aos_assert_rr(rslt, rdata, false);

	rslt = readFileIDsMetaFile(rdata);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosVector2DConn::readTimeIDsMetaFile(AosRundata *rdata)
{
	// TimeID meta file is an array of u32.
	mTimeIDsFile = openFile(rdata, mTimeIDsFileID);
	aos_assert_rr(mTimeIDsFile, rdata, false);
	mTimeIDsFileSize = mTimeIDsFile->getLength();
	mNumTimeIDs = mTimeIDsFileSize / sizeof(u32);

	mTimeIDsBuff = OmnNew AosBuff(mTimeIDsFileSize);
	char *data = (char *)mTimeIDsBuff->data();
	bool rslt = mTimeIDsFile->readToBuff(0, mTimeIDsFileSize, data, rdata);
	aos_assert_rr(rslt, rdata, false);
	mTimeIDsArray = (u32 *)data;

	return true;
}


bool
AosVector2DConn::readFileIDsMetaFile(AosRundata *rdata)
{
	// FileID meta file is an array of u64
	mFileIDsFile = openFile(rdata, mFileIDsFileID);
	aos_assert_rr(mFileIDsFile, rdata, false);
	mFileIDsFileSize = mFileIDsFile->getLength();

	mFileIDsBuff = OmnNew AosBuff(mFileIDsFileSize);
	char *data = (char *)mFileIDsBuff->data();
	bool rslt = mFileIDsFile->readToBuff(0, mFileIDsFileSize, data, rdata);
	aos_assert_rr(rslt, rdata, false);
	mFileIDsArray = (u64 *)data;

	return true;
}


AosReliableFilePtr
AosVector2DConn::openFile(
		AosRundata *rdata, 
		const u64 file_id)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_rr(vfsMgr, rdata, 0);
	return vfsMgr->openRlbFile(file_id, mFileType, rdata);
}

