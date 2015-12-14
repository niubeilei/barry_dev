////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BatchFixedTrans_h
#define Aos_DocTrans_BatchFixedTrans_h

#include "TransUtil/DocTrans.h"
#include "StorageEngine/Ptrs.h"


class AosBatchFixedTrans : public AosDocTrans 
{
	int						mVid;
	AosSengineDocInfoPtr	mRespCaller;
	u32						mSizeId;
	u64						mNumDocs;
	int						mRecordSize;
	AosBuffPtr 				mDocids;
	AosBuffPtr				mRawData;
	u64						mTaskDocid;

public:
	AosBatchFixedTrans(const bool regflag);
	AosBatchFixedTrans(
			const AosSengineDocInfoPtr &resp_caller,
			const int vid,
			const u32 sizeid,
			const u64 &num_docs,
			const int record_size,
			const AosBuffPtr &docid_buff,
			const AosBuffPtr &buff,
			const u64 &snap_id,
			const u64 &task_docid);
	~AosBatchFixedTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack();

};

#endif
