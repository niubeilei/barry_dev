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
// 2014/01/13 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageEngine_SengineFixedDocReq_h
#define AOS_StorageEngine_SengineFixedDocReq_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/SengineDocReqObj.h"


class AosSengineFixedDocReq : public AosSengineDocReqObj 
{

public:
	AosBuffPtr	mDocids;
	AosBuffPtr	mRawData;

public:
	AosSengineFixedDocReq(
			const AosTransPtr &trans,
			const int vid,
			const u32 sizeid,
			const int num_docs,
			const int record_size,
			const AosBuffPtr &docid_buff,
			const AosBuffPtr &rawdata_buff,
			const u64 &snap_id)
	:
	AosSengineDocReqObj(eFixedDoc, trans,
		vid, sizeid, num_docs, record_size, snap_id),
	mDocids(docid_buff),
	mRawData(rawdata_buff)
	{
	}

	~AosSengineFixedDocReq()
	{
	}
};

#endif

