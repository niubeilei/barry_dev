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
// 11 Jun 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_BlobSERestore_H_
#define Aos_BlobSE_BlobSERestore_H_
#include "aosUtil/Types.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/HeaderCache.h"
#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosBlobSERestore : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	typedef std::stack<AosBlobHeaderPtr> stack_header_t;

private:
	AosBlobSE*			mBlobSE;
	AosHeaderCache*		mHeaderCache;
	AosHeaderFileMgr*	mHeaderFileMgr;

public:
    AosBlobSERestore();
    virtual ~AosBlobSERestore();

    int restoreToTimestamp(
    		AosRundata	*rdata,
    		const u64	ullTimestamp);

private:
    int prepareForRestoration(
    		AosRundata	*rdata,
			u32			&ullBoundaryOldHeaderFileSeqno);

    int restoreHeadersToTimestamp(
    		AosRundata	*rdata,
    		const u64	ullTimestamp,
			const u32	ullBoundaryOldHeaderFileSeqno,
			const u64	ullBoundaryRawfid,
			const u64	ullBoundaryOffset);

    int getBodyFileBoundaryByTimestamp(
    		AosRundata	*rdata,
    		const u64	ullTimestamp,
			u64			&ullBoundaryRawfid,
			u64			&ullBoundaryOffset);

    int removeBodyFilesThatBeyondBoundary(
    		AosRundata	*rdata,
			const u64	ullBoundaryRawfid,
			const u64	ullBoundaryOffset);

    int removeUnusedOldHeaderFiles(
    		AosRundata	*rdata,
			const u32	ullBoundaryOldHeaderFileSeqno);
};

#endif /* Aos_BlobSE_BlobSERestore_H_ */
