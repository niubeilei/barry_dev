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
#ifndef Aos_DataStructs_VectorFileSimple_h
#define Aos_DataStructs_VectorFileSimple_h

#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/RlbFileType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"


class AosVectorFileSimple : public OmnRCObject
{
	OmnDefineRCObject;

private:
	bool		mIsGood;
	u64			mCubeId;
	u64			mFileId;
	i64			mFileSize;
	int			mRowSize;
	AosRlbFileType::E mFileType;
	AosReliableFile *  mReliableFileRaw;
	AosReliableFilePtr mReliableFile;

public:
	AosVectorFileSimple(AosRundata *rdata, 
				const u64 cube_id,
				const u64 file_id, 
				const u32 row_size, 
				const AosRlbFileType::E file_type);
	AosVectorFileSimple(AosRundata *rdata, 
				const u64 cube_id,
				const u32 row_size, 
				const AosRlbFileType::E file_type);
	~AosVectorFileSimple();

	inline bool isGood() const {return mIsGood;}

	bool readCellBlock( AosRundata *rdata, 
						const u64 sdocid, 
						const int num_rows,
						AosBuffPtr &buff);

	bool writeCellBlock(AosRundata *rdata, 
						const u64 sdocid, 
						const int num_rows,
						AosBuff *buff);

	u64 getFileId() const;

private:
	i64 getCellBlockPos(AosRundata *rdata, 
						const u64 sdocid, 
						const bool create_flag);

	i64 createCellBlock(AosRundata *rdata, const u64 sdocid);
	bool init(AosRundata *rdata);
	bool initializeVectorFileSimple(AosRundata *rdata);
};
#endif



