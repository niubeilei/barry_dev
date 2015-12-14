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
#include "BlobSE/BodyCache.h"
#include "RawSE/RawSE.h"

AosBodyCache::AosBodyCache(
		AosRundata	*rdata,
		AosRawSE*	rawse,
		const u32	siteid,
		const u32	cubeid,
		const u64	aseid)
:
mSiteID(siteid),
mCubeID(cubeid),
mAseID(aseid)
{
	if (!rawse)
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mRawSE = rawse;
}


AosBodyCache::~AosBodyCache()
{
}


int
AosBodyCache::readFile(
		AosRundata	*rdata,
		const u64	rawfid,
		const u64	offset,
		const u64	length,
		AosBuffPtr	&buff)
{

	return mRawSE->readRawFile(rdata, mSiteID, mCubeID, mAseID,
			rawfid, offset, length, buff, AosRawFile::eReadWriteCreate);
}


int
AosBodyCache::saveEntryToPos(
		AosRundata	*rdata,
		const u32	ulSeqno,
		const u32	ulOffset,
		AosBuffPtr	&pBuff)
{
	return 0;
}


int
AosBodyCache::readEntry(
		AosRundata	*rdata,
		const u32	ulSeqno,
		const u32	ulOffset,
		const u32	ulLength,
		AosBuffPtr	&pBuff)
{
	return 0;
}
