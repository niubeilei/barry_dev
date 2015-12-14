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
// 2015-05-07	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BatchInsertCommonDocTrans_h
#define Aos_DocTrans_BatchInsertCommonDocTrans_h

#include "TransUtil/DocTrans.h"
#include "StorageEngine/Ptrs.h"


class AosBatchInsertCommonDocTrans : public AosDocTrans 
{
	int						mVid;
	AosSengineDocInfoPtr	mRespCaller;
	u32						mSizeId;
	u64						mNumDocs;
	int						mRecordSize;
	u64						mDataLen;
	AosBuffPtr 				mBuff;
	u64						mTaskDocid;

public:
	AosBatchInsertCommonDocTrans(const bool regflag);
	AosBatchInsertCommonDocTrans(
			const AosSengineDocInfoPtr &resp_caller,
			const int vid,
			const u32 sizeid,
			const u64 &num_docs,
			const int record_size,
			const u64 &data_len,
			const AosBuffPtr &buff,
			const u64 &snap_id,
			const u64 &task_docid);
	~AosBatchInsertCommonDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack();

};

#endif
