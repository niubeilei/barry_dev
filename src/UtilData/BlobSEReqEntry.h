////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 26 May 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UtilData_BlobSEReqEntry_H_
#define Aos_UtilData_BlobSEReqEntry_H_

#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"

class AosBlobSEReqEntry : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum ReqTypeAndResult
	{
		eInvalid		= 0,
		//request types for doc operations
		eSave			= 1,
		eDelete			= 2,
		eRead			= 3,
		//request types for snapshot operations
		eCreateSnapshot	= 11,
		eCommitSnapshot	= 12,
		eCancelSnapshot	= 13,
		//result of operations
		eDeleted		= 101,	//for telling whether the result of reading is a deleted doc
		eNotExist		= 102,	//for generating no-op request
	};

#pragma pack(1)
	struct BlobSEReqEntry
	{
		u64		ullDocid;
		u64		ullSnapshotID;
		u64		ullOprID;
		u64		ullTimestamp;
		u64		ullBodyDataLen;
		u8		ucReqTypeAndResult;
		char	pData[];	//header custom data + body
	};
#pragma pack(0)

private:
	u64			mDocid;
	u64			mSnapshotID;
	u64			mOprID;
	u64			mTimestamp;
	u32			mHeaderCustomDataSize;
	u8			mReqTypeAndResult;
	AosBuffPtr	mHeaderCustomData;
	AosBuffPtr	mBodyData;

public:
    AosBlobSEReqEntry();
    AosBlobSEReqEntry(const u32 ulHeaderCumstomDataSize);
    virtual ~AosBlobSEReqEntry();

	u64	getDocid() const {return mDocid;}
	u64	getOprID() const {return mOprID;}
	u64	getSnapshotID() const {return mSnapshotID;}
	u64	getTimestamp() const {return mTimestamp;}
	u32 getHeaderCustomDataSize() const {return mHeaderCustomDataSize;}
	ReqTypeAndResult getReqTypeAndResult() const {return (ReqTypeAndResult)mReqTypeAndResult;}
	AosBuffPtr getHeaderCustomDataBuff() const {return mHeaderCustomData;}
	AosBuffPtr getBodyBuff() const {return mBodyData;}

	void setDocid(const u64 docid)
	{
		mDocid = docid;
	}

	void setOprID(const u64 oprid)
	{
		mOprID = oprid;
	}

	void setSnapshotID(const u64 snapshotID)
	{
		mSnapshotID = snapshotID;
	}

	void setTimestamp(const u64 timestamp)
	{
		mTimestamp = timestamp;
	}

	void setHeaderCustomDataSize(const u32 size)
	{
		mHeaderCustomDataSize = size;
	}

	void setReqTypeAndResult(const ReqTypeAndResult eReqTypeAndResult)
	{
		mReqTypeAndResult = eReqTypeAndResult;
	}

	void setBodyBuff(const AosBuffPtr pBuff)
	{
		mBodyData = pBuff;
	}

	void setHeaderCustomDataBuff(const AosBuffPtr pBuff)
	{
		mHeaderCustomData = pBuff;
		if (pBuff->dataLen() != mHeaderCustomDataSize)
		{
			OmnAlarm << "pBuff->dataLen():" << pBuff->dataLen()
					<< " != mHeaderCustomDataSize:" << mHeaderCustomDataSize << enderr;
		}
	}

	int unserializeFromBuff(const AosBuffPtr	&pBuff);

	int serializeToBuff(const AosBuffPtr	&pBuff);
};

#endif /* Aos_BlobSE_BlobSEReqEntry_H_ */
