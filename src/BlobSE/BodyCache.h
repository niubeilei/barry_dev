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
// 2014-12-22 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_BodyCache_H_
#define Aos_BlobSE_BodyCache_H_
#include "RawSE/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"

class AosBodyCache : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	u32				mSiteID;
	u32				mCubeID;
	u64				mAseID;
	AosRawSE*		mRawSE;

public:
    AosBodyCache(
    		AosRundata	*rdata,
    		AosRawSE*	rawse,
    		const u32	siteid,
    		const u32	cubeid,
    		const u64	aseid);
    ~AosBodyCache();

    int readFile(
    		AosRundata	*rdata,
    		const u64	rawfid,
    		const u64	offset,
    		const u64	length,
    		AosBuffPtr	&buff);

    int saveEntryToPos(
    		AosRundata	*rdata,
    		const u32	ulSeqno,
			const u32	ulOffset,
			AosBuffPtr	&pBuff);

    int readEntry(
    		AosRundata	*rdata,
    		const u32	ulSeqno,
    		const u32	ulOffset,
    		const u32	ulLength,
    		AosBuffPtr	&pBuff);
};


#endif /* Aos_BlobSE_BodyCache_H_ */
