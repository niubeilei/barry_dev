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
// 6 May 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_OprIDCache_H_
#define Aos_BlobSE_OprIDCache_H_
#include "aosUtil/Types.h"
#include "RawSE/RawSE.h"
#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosOprIDCache : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eStartOprID		= 1,
		eOprIDsPerFile	= 1000000,
	};

	enum OprType
	{
		eDocOperation	= 1,
		eSnapShot		= 2,
	};

#pragma pack(1)
	struct OprIDEntry
	{
		u64	ullDocid;
		u64	ullOprID;
		u64	ullRawfid;
		u64	ullOffset;
		u64	ullLen;
		u8	ucType;

		OprIDEntry& operator = (const OprIDEntry& r)
		{
			ullDocid = r.ullDocid;
			ullOprID = r.ullOprID;
			ullRawfid = r.ullRawfid;
			ullOffset = r.ullOffset;
			ullLen = r.ullLen;
			ucType = r.ucType;
			return *this;
		}

		const char* toStr() const
		{
			std::stringstream ss;
			ss.str("");
			ss.clear();
			ss << " docid:" << ullDocid << " oprid:" << ullOprID
					<< " seqno:" << ullRawfid << " offset:" << ullOffset
					<< " len:" << ullLen << "\t";
			return ss.str().c_str();
		}
	};
#pragma pack(0)

private:
	u32					mSiteID;
	u32					mCubeID;
	u64					mAseID;
	OmnMutexPtr			mLock;
	OmnMutex*			mLockRaw;
	AosRundata*			mRdata;
	AosRawSE*			mRawSE;
	u32					mOprIDsPerFile;
	AosRawFilePtr		mActiveOprIDFile;
	AosBuffPtr			mActiveOprIDFileBuffer;

public:
    AosOprIDCache(
    		AosRundata			*rdata,
    		AosRawSE			*rawse,
    		const AosXmlTagPtr	&conf);

    virtual ~AosOprIDCache();

    int saveOprID(
    		AosRundata	*rdata,
			const u64	ullOprId,
			const u64	ullRawfid,
			const u64	ullOffset,
			const u64	ullLen);

    int removeOprID(
    		AosRundata	*rdata,
			const u64	ullOprId);

    int getRawfidAndOffsetAndLenByOprID(
    		const u64	ullOprId,
			u64			&ullRawfid,
			u64			&ullOffset,
			u64			&ullLen);

    int shutdown();

    int getNextOprIDEntry(
    		AosRundata	*rdata,
			OprIDEntry	&sOprIDEntry);

    bool oprHasHeader(OprIDEntry &sOprIDEntry);

private:
    u32 getOprIDFileSeqnoByOprID(const u64 ullOprID);

    int overwriteOprIDFile();

    int switchOprIDFileAndBuff(const u32 ulSeqno);

    u32 getOprIDFileSeqnoByRawfid(const u64 ullRawfid);

    u32 getOffsetByOprID(const u64 ullOprID);

    u64 getOprIDFileRawfidBySeqno(const u32 ulSeqno);

    u32 getOprIDFileSize();
};

#endif /* Aos_BlobSE_OprIDCache_H_ */
