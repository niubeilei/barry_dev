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
#ifndef Aos_DataStructs_Vector2DConn_h
#define Aos_DataStructs_Vector2DConn_h

#include "DataStructs/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/RlbFileType.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

class AosVectorFile;

class AosVector2DConn : public OmnRCObject
{
	OmnDefineRCObject;

	typedef hash_map<const u64, AosVectorFilePtr, u64_hash, u64_cmp> map1_t;
	typedef hash_map<const u64, AosVectorFilePtr, u64_hash, u64_cmp>::iterator itr1_t;
	typedef hash_map<const u64, AosVectorFile*, u64_hash, u32_cmp> map2_t;
	typedef hash_map<const u64, AosVectorFile*, u64_hash, u32_cmp>::iterator itr2_t;

	typedef hash_map<const u32, u64, u32_hash, u32_cmp> map3_t;
	typedef hash_map<const u32, u64, u32_hash, u32_cmp>::iterator itr3_t;

private:
	u64			mStatDocid;
	u64			mCubeId;
	u32			mRowsPerCell;
	u32			mColumnsPerCell;
	u64			mTimeIDsFileID;
	u64			mFileIDsFileID;
	AosRlbFileType::E mFileType;
	AosBuffPtr 	mTimeIDsBuff;
	AosBuffPtr 	mFileIDsBuff;
	u32	*		mTimeIDsArray;
	u64	*		mFileIDsArray;
	int 		mNumTimeIDs;
	i64			mTimeIDsFileSize;
	i64			mFileIDsFileSize;
	u32			mCellBlockSize;
	AosReliableFilePtr	mTimeIDsFile;
	AosReliableFilePtr	mFileIDsFile;
	map1_t		mFileMap;
	map2_t		mFileMapRaw;
	map3_t		mTimeId2FileIdMap;

public:
	AosVector2DConn(
			const u64 stad_docid,
			const u64 cube_id, 
			const u32 rows_per_cell,
			const u32 columns_per_cell);
	~AosVector2DConn();

	bool readCellBlock(	AosRundata *rdata,
						const u64 sdocid, 
						const u64 timeid,
						AosBuffPtr &buff);
	
	bool writeCell( 	AosRundata *rdata, 
						const u64 sdocid, 
						const u64 timeid,
						AosBuff *buff);

private:
	inline u32 getSdocidBlockId(const u64 sdocid) const
	{
		return sdocid / mRowsPerCell;
	}

	inline u32 getTimeBlockId(const u64 timeid)
	{
		return timeid / mColumnsPerCell;
	}

	AosVectorFile * getVectorFile(
						AosRundata *rdata,
						const u64 timeid, 
						const bool create_flag);

	AosVectorFile * createVectorFile(
						AosRundata *rdata, 
						const u32 time_block_id);

	bool readMetadata(AosRundata *rdata);
	bool readTimeIDsMetaFile(AosRundata *rdata);
	bool readFileIDsMetaFile(AosRundata *rdata);
	AosReliableFilePtr openFile(AosRundata *rdata, const u64 file_id);
};
#endif



