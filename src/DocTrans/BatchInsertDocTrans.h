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
#ifndef Aos_DocTrans_BatchInsertDocTrans_h
#define Aos_DocTrans_BatchInsertDocTrans_h

#include "TransUtil/DocTrans.h"
#include "StorageEngine/Ptrs.h"
#include "SEInterfaces/DataProcObj.h"


class AosBatchInsertDocTrans : public AosDocTrans 
{
	enum
	{
		eDftCompressSize = 500000
	};

	AosDataProcObjPtr		mRespCaller;
	int						mVid;
	u64						mGroupId;
	u64						mSnapshotId;
	u64						mTaskDocid;
	AosBuffPtr 				mBuff;
	bool					mCompressFlag;

public:
	AosBatchInsertDocTrans(const bool regflag);
	AosBatchInsertDocTrans(
		const AosDataProcObjPtr &resp_caller,
		const int vid,
		const u64 group_id, 
		const u64 &snap_id,
		const u64 &task_docid,
		const AosBuffPtr &buff);
	~AosBatchInsertDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack();

};

#endif
