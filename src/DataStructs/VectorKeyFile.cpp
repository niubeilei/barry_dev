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
// Vector Key Files are used to save keys. The file structure is:
// 		number of keys	u32
// 		[key offset]	u32	
// 		[key offset]	u32
// 		...
// 		[key offset]	u32		// These define the index block
// 		key				variable	
// 		key				variable	
// 		...
//
// The index block size is configurable and defaults to 1MB. Keys
// are immutable. 
//
// There can be multiple key files. 
//
// Modification History:
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/VectorKeyFile.h"

#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"



AosVectorKeyFile::AosVectorKeyFile(
		AosRundata *rdata, 
		const u64 cube_id,
		const u64 file_id, 
		const u32 num_rows_per_file, 
		const AosRlbFileType::E file_type)
:
mIsGood(false),
mCubeId(cube_id),
mFileId(file_id),
mFileSize(-1),
mNumSdocidsPerFile(num_rows_per_file),
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
AosVectorKeyFile::AosVectorKeyFile(
		AosRundata *rdata, 
		const u64 cube_id,
		const u32 num_rows_per_file, 
		const AosRlbFileType::E file_type)
:
mIsGood(false),
mCubeId(cube_id),
mFileId(0),
mFileSize(-1),
mNumSdocidsPerFile(num_rows_per_file),
mFileType(file_type),
mReliableFileRaw(0)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);
	mReliableFile = vfsMgr->createRlbFile(rdata, mCubeId, "vctfile", 0, mFileType, false);
	mReliableFileRaw = mReliableFile.getPtr();
	aos_assert(mReliableFile);
	mFileId = mReliableFileRaw->getFileId();

	initializeVectorKeyFile(rdata);

	mIsGood = true;
}


AosVectorKeyFile::~AosVectorKeyFile()
{
}


bool
AosVectorKeyFile::initializeVectorKeyFile(AosRundata *rdata)
{
	// It initializes a vector key file. It creates the index
	// block, initializes it, and saves it.
	//
	// mFileSize must be set to the proper file size.
	
	aos_assert_rr(mReliableFileRaw, rdata, false);
	mFileSize = (mNumSdocidsPerFile + 1) *sizeof(u32);
	mIndexBuff = OmnNew AosBuff(mFileSize);

	// Note: the first u32 is the 'number of keys' field (u32).
	// Need to skip it.
	mIndexArray = (u32 *)&mIndexBuff->data()[4];
	char *data = (char *)mIndexBuff->data();
	memset(data, 0, mFileSize);
	bool rslt = mReliableFileRaw->write(0, data, mFileSize, rdata);
	aos_assert_rr(rslt, rdata, false);

	mNumKeys = 0;

	return true;
}


bool
AosVectorKeyFile::init(AosRundata *rdata)
{
	// 1. Read in the index block
	aos_assert_rr(mReliableFileRaw, rdata, false);
	mIndexBuff = OmnNew AosBuff((mNumSdocidsPerFile + 1) * sizeof(u32));
	mIndexArray = (u32 *)&mIndexBuff->data()[4];
	char *data = (char *)mIndexArray;
	bool rslt = mReliableFileRaw->readToBuff(0, mNumSdocidsPerFile * sizeof(u32), data, rdata);
	aos_assert_rr(rslt, rdata, false);

	mNumKeys = *(u32*)mIndexBuff->data();

	return true;
}


bool
AosVectorKeyFile::readKeys(
		AosRundata *rdata, 
		const u64 sdocid, 
		const int num_keys, 
		AosBuffPtr &buff)
{
	// This function reads 'num_keys' starting from 'sdocid'. 
	// It assumes these are valid keys. Otherwise, it is an 
	// error. 
	//
	// Keys are encoded as [int (for length), contents (variable)]
	aos_assert_rr(num_keys > 0, rdata, false);	
	aos_assert_rr(sdocid + num_keys < mNumKeys, rdata, false);
	u32 start_pos = mIndexArray[sdocid];
	u32 end_pos = 0;
	if (sdocid+num_keys == mNumKeys)
	{
		end_pos = mFileSize;
	}
	else
	{
		end_pos = mIndexArray[sdocid+num_keys];
	}

	u32 size = end_pos - start_pos;
	buff = OmnNew AosBuff(size);
	char *data = buff->data();

	return mReliableFileRaw->readToBuff(start_pos, size, data, rdata);
}


bool
AosVectorKeyFile::appendEntries(
		AosRundata *rdata, 
		const u64 sdocid,
		const int num_keys,
		AosBuff *buff)
{
	// This function appends the entries to the key file.
	// 'buff' is in the following format:
	// 		[contents]
	// 		[contents]
	// 		...
	// 		[contents]
	// The contents are constructed by using AosBuff::setCharStr(data, len, false);
	buff->reset();
	AosBuffPtr bb = OmnNew AosBuff();
	aos_assert_rr(sdocid >= mStartSdocid, rdata, false);
	u64 sd = sdocid;
	aos_assert_rr(sd < mNumSdocidsPerFile, rdata, false);
	i64 pos = mFileSize;
	int key_len;
	for (int i=0; i<num_keys; i++)
	{
		aos_assert_rr(mIndexArray[sd+i] == 0, rdata, false);
		mIndexArray[sd+i] = pos;
		buff->getCharStr(key_len);
		pos = mFileSize + buff->getCrtIdx();
	}
	
	// Save the indexes
	i64 start_pos = eIndexStartPos + sd*sizeof(u32);
	u32 index_size = sizeof(u32) * num_keys;
	bool rslt = mReliableFileRaw->write(start_pos, 
			(char *)&mIndexArray[sd], index_size, rdata);
	aos_assert_rr(rslt, rdata, false);

	// Save the body
	rslt = mReliableFileRaw->append(buff->data(), buff->dataLen(), true);
	aos_assert_rr(rslt, rdata, false);

	mFileSize += buff->dataLen();

	return true;
}

