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
// 04/28/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_AioRequest_h
#define AOS_StorageMgr_AioRequest_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "StorageMgr/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/AioCaller.h"

class AosAioRequest : virtual public OmnRCObject   
{
	OmnDefineRCObject;

public:
	enum Type
	{
		eAioReqRead,
		eAioReqWrite
	};

public:
	u64 					mReqId;
	AosAioCallerPtr			mCaller;
	const OmnLocalFilePtr	mFile;
	int64_t					mOffset;
	int64_t					mSize;
	Type					mType;
	u64						mTimestamp;

	AosAioRequest() {}

	AosAioRequest(
			const u64 &reqid, 
			const AosAioCallerPtr &caller,
			const OmnLocalFilePtr &file,
			const int64_t &offset,
			const int64_t &size)
	:
	mReqId(reqid),
	mCaller(caller),
	mFile(file),
	mOffset(offset),
	mSize(size),
	mType(eAioReqRead),
	mTimestamp(OmnGetTimestamp())	
	{
	}

	AosAioRequest(
			const u64 &reqid,
			const AosAioCallerPtr &caller,
			const OmnLocalFilePtr &file,
			const int64_t &offset,
			const int64_t &size,
			const Type type)
	:
	mReqId(reqid),
	mCaller(caller),
	mFile(file),
	mOffset(offset),
	mSize(size),
	mType(type),
	mTimestamp(OmnGetTimestamp())	
	{
	}

	~AosAioRequest()
	{
		mCaller = 0;
	}

	friend bool operator < (const AosAioRequestPtr &p1, const AosAioRequestPtr &p2)
	{
		//return p1->mTimestamp < p2->mTimestamp;
		int vv = p1->mTimestamp - p2->mTimestamp;
		if (p1->mType == p2->mType)
		{
			if (abs(vv) < 500000)
			{
				if (p1->mFile == p2->mFile)
				{
					return p1->mOffset > p2->mOffset;
				}                                     
				else
				{
					return p1->mFile < p2->mFile;
				}
			}
		}
		return  p1->mTimestamp > p2->mTimestamp;
	}

};

#endif
