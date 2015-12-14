////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2015/05/07 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageEngine_SengineCommonDocReq_h
#define AOS_StorageEngine_SengineCommonDocReq_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/SengineDocReqObj.h"


class AosSengineCommonDocReq : public AosSengineDocReqObj 
{

public:
	u64			mDataLen;
	AosBuffPtr	mBuff;

public:
	AosSengineCommonDocReq(
			const req_type type,
			const AosTransPtr &trans,
			const int vid,
			const u32 sizeid,
			const u64 &num_docs,
			const int record_size,
			const u64 &data_len,
			const AosBuffPtr &buff,
			const u64 &snap_id)
	:
	AosSengineDocReqObj(type, trans,
		vid, sizeid, num_docs, record_size, snap_id),
	mDataLen(data_len),
	mBuff(buff)
	{
	}


	~AosSengineCommonDocReq()
	{
	}
};

#endif

