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
#ifndef AOS_SEInterfaces_SengineSaveDocReqObj_h
#define AOS_SEInterfaces_SengineSaveDocReqObj_h

#include "TransBasic/Trans.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"


class AosSengineSaveDocReqObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum req_type
	{
		eFixedDoc,
		eCSVDoc
	};

	req_type	mType;
	AosTransPtr	mTrans;	
	int			mVid;
	u32			mSizeid;
	int			mNumDocs;
	int			mRecordSize;
	u64			mSnapId;

	AosSengineSaveDocReqObj(
			const req_type type,
			const AosTransPtr &trans,
			const int vid,
			const u64 &snap_id)
	:
	mType(type),
	mTrans(trans),
	mVid(vid),
	mSnapId(snap_id)
	{


	AosSengineSaveDocReqObj(
			const req_type type,
			const AosTransPtr &trans,
			const int vid,
			const u32 sizeid,
			const int num_docs,
			const int record_size,
			const u64 &snap_id)
	:
	mType(type),
	mTrans(trans),
	mVid(vid),
	mSizeid(sizeid),
	mNumDocs(num_docs),
	mRecordSize(record_size),
	mSnapId(snap_id)
	{
	}

	~AosSengineSaveDocReqObj()
	{
	}
};

#endif

