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
// 10/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_FileReadReq_h
#define AOS_SEInterfaces_FileReadReq_h

#include "Debug/Except.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/FileReadListener.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

struct AosFileReadRequest :  public OmnRCObject
{
	OmnDefineRCObject;

	AosNetFileObjPtr		mFile;
	AosFileReadListenerPtr	mCaller;
	u64						mOffset;
	u64						mReadId;
	u64						mReadSize;

	AosFileReadRequest(
			const u64 read_id,
			const AosNetFileObjPtr &file,
			const u64 offset,
			const u64 read_size,
			const AosFileReadListenerPtr &caller)
	:
	mFile(file),
	mCaller(caller),
	mOffset(offset),
	mReadId(read_id),
	mReadSize(read_size)
	{
		if (!mFile)
		{
			OmnThrowException("File is empty");
		}

		if (!mCaller)
		{
			OmnThrowException("Caller is empty");
		}

		if (mReadSize < 0)
		{
			OmnThrowException("Read size is less than 0");
		}
	}
	~AosFileReadRequest()
	{
	}
};
#endif
