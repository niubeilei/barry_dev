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
#ifndef Aos_DataStructs_VectorFile_h
#define Aos_DataStructs_VectorFile_h

#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/RlbFileType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"


class AosVectorFile : public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef hash_map<const u64, AosBuffPtr, u64_hash, u32_cmp> map1_t;
	typedef hash_map<const u64, AosBuffPtr, u64_hash, u32_cmp>::iterator itr1_t;
	typedef hash_map<const u64, i64 *, u64_hash, u32_cmp> map2_t;
	typedef hash_map<const u64, i64 *, u64_hash, u32_cmp>::iterator itr2_t;

	enum
	{
		eNumEntriesPerIndexBlock = 10*1000,
		eIndexBlockSize = eNumEntriesPerIndexBlock * sizeof(i64),
		eMaxIndexBlocks = 10000,
		eControlBlockSize
	};

	bool		mIsGood;
	u64			mCubeId;
	u64			mFileId;
	i64			mFileSize;
	int			mCellBlockSize;
	u32			mNumRowsPerCellBlock;
	int			mNumIndexBlocks;
	AosBuffPtr	mControlBlockBuff;
	i64 *		mControlBlockArray;
	i64			mLastIndexUnusedEntry;	// It points to the first unused entry
	i64			mLastIndexBlockPos;
	AosBuffPtr	mLastIndexBlockBuff;
	i64 *		mLastIndexBlockArray;
	map1_t		mIndexBlockMap;
	map2_t		mIndexBlockMapRaw;
	AosRlbFileType::E mFileType;
	AosReliableFile *  mReliableFileRaw;
	AosReliableFilePtr mReliableFile;

public:
	AosVectorFile(AosRundata *rdata, 
				const u64 cube_id,
				const u64 file_id, 
				const u32 cell_block_size, 
				const u32 num_rows_per_cell_block, 
				const AosRlbFileType::E file_type);
	AosVectorFile(AosRundata *rdata, 
				const u64 cube_id,
				const u32 cell_block_size, 
				const u32 num_rows_per_cell_block, 
				const AosRlbFileType::E file_type);
	~AosVectorFile();

	inline bool isGood() const {return mIsGood;}

	bool readCellBlock(AosRundata *rdata, 
						const u64 sdocid, 
						AosBuffPtr &buff);

	bool writeCell(AosRundata *rdata, 
						const u64 sdocid, 
						AosBuff *buff);

	u64 getFileId() const;

private:
	i64 getCellBlockPos(AosRundata *rdata, 
						const u64 sdocid, 
						const bool create_flag);

	i64 createCellBlock(AosRundata *rdata, const u64 sdocid);
	bool init(AosRundata *rdata);
	bool initializeVectorFile(AosRundata *rdata);
};
#endif



