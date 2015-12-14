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


class AosVectorKeyFile : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eIndexStartPos = 100
	};

	typedef hash_map<const u64, AosBuffPtr, u64_hash, u32_cmp> map1_t;
	typedef hash_map<const u64, AosBuffPtr, u64_hash, u32_cmp>::iterator itr1_t;
	typedef hash_map<const u64, i64 *, u64_hash, u32_cmp> map2_t;
	typedef hash_map<const u64, i64 *, u64_hash, u32_cmp>::iterator itr2_t;

	bool		mIsGood;
	u64			mCubeId;
	u64			mFileId;
	i64			mFileSize;
	i64			mCrtIndexPos;	// It points to the first unused entry
	u32			mNumKeys;
	u32			mNumSdocidsPerFile;
	u64			mStartSdocid;
	AosBuffPtr	mIndexBuff;
	u32 *		mIndexArray;
	AosRlbFileType::E mFileType;
	AosReliableFile *  mReliableFileRaw;
	AosReliableFilePtr mReliableFile;

public:
	AosVectorKeyFile(AosRundata *rdata, 
				const u64 cube_id,
				const u64 file_id, 
				const u32 num_rows_per_file, 
				const AosRlbFileType::E file_type);
	AosVectorKeyFile(AosRundata *rdata, 
				const u64 cube_id,
				const u32 num_rows_per_file, 
				const AosRlbFileType::E file_type);
	~AosVectorKeyFile();

	inline bool isGood() const {return mIsGood;}

	bool readCellBlock(AosRundata *rdata, 
						const u64 sdocid, 
						AosBuffPtr &buff);

	bool writeCell(AosRundata *rdata, 
						const u64 sdocid, 
						AosBuff *buff);

	u64 getFileId() const;

	bool readKeys(AosRundata *rdata, 
						const u64 sdocid, 
						const int num_keys, 
						AosBuffPtr &buff);

	bool appendEntries(AosRundata *rdata, 
						const u64 sdocid,
						const int num_keys,
						AosBuff *buff);

private:
	bool init(AosRundata *rdata);
	bool initializeVectorKeyFile(AosRundata *rdata);
};
#endif



