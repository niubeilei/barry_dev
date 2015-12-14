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
// 	This file simulates a fixed length array:
// 		[entry]
// 		[entry]
// 		...
// 		[entry]
//
// Modification History:
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/VectorFileSimple.h"

#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"



AosVectorFileSimple::AosVectorFileSimple(
		AosRundata *rdata, 
		const u64 cube_id,
		const u64 file_id, 
		const u32 row_size, 
		const AosRlbFileType::E file_type)
:
mIsGood(false),
mCubeId(cube_id),
mFileId(file_id),
mFileSize(0),
mRowSize(row_size),
mFileType(file_type),
mReliableFileRaw(0)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);
	mReliableFile = vfsMgr->openRlbFile(mFileId, mFileType, rdata);
	aos_assert(mReliableFile);
	mReliableFileRaw = mReliableFile.getPtr();

	mFileSize = mReliableFileRaw->getFileCrtSize();
	mFileId = mReliableFile->getFileId();
	mIsGood = init(rdata);
}


// This constructor creates a new vector file. 
AosVectorFileSimple::AosVectorFileSimple(
		AosRundata *rdata, 
		const u64 cube_id,
		const u32 row_size, 
		const AosRlbFileType::E file_type)
:
mIsGood(false),
mCubeId(cube_id),
mFileId(0),
mFileSize(0),
mRowSize(row_size),
mFileType(file_type),
mReliableFileRaw(0)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);
	mReliableFile = vfsMgr->createRlbFile(
			rdata, mCubeId, "vctfile", 0, mFileType, false);
	aos_assert(mReliableFile);
	mFileId = mReliableFile->getFileId();

	mIsGood = true;
}


AosVectorFileSimple::~AosVectorFileSimple()
{
}


bool
AosVectorFileSimple::readCellBlock(
		AosRundata *rdata, 
		const u64 sdocid, 
		const int num_rows,
		AosBuffPtr &buff)
{
	// This function reads a cell block. If the cell block does not
	// exist, 'buff' is set to 0 and true is returned. 
	buff = 0;
	int read_size = num_rows * mRowSize;
	buff = OmnNew AosBuff(read_size);
	i64 start_pos = sdocid * mRowSize;
	char *data = buff->data();

	bool rslt = mReliableFileRaw->readToBuff(
			start_pos, read_size, data, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosVectorFileSimple::writeCellBlock(
		AosRundata *rdata, 
		const u64 sdocid, 
		const int num_rows,
		AosBuff *buff)
{
	// This function writes the cell 'buff' into the vector file. 
	// 1. Check whether the cell block exists. If not, it needs to 
	//    create it.
	// 2. It overrites the cell block.
	i64 start_pos = sdocid * mRowSize;
	int size = num_rows * mRowSize;
	bool rslt = mReliableFile->write(start_pos, buff->data(), size, true);
	aos_assert_rr(rslt, rdata, false);
	mFileSize += buff->dataLen();
}

