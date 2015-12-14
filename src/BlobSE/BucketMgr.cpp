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
// 23 Jul 2015 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/BucketMgr.h"

#include "BlobSE/BlobHeader.h"
#include "BlobSE/HeaderBuffAndFileMgr.h"
#include "BlobSE/HeaderCache.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/Bucket.h"
#if 0
AosBucketMgr::AosBucketMgr(
		AosBlobSE					*blobSE,
		AosRawSE					*rawSE,
		AosHeaderFileMgrPtr			&pHeaderFileMgr,
		AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
		AosHeaderCachePtr			&pHeaderCache)
:
mHeaderFileMgr(pHeaderFileMgr),
mHeaderBuffAndFileMgr(pHeaderBuffAndFileMgr),
mNormalHeaderCache(pHeaderCache)
{

}


AosBucketMgr::~AosBucketMgr()
{
}


int
AosBucketMgr::putOprIDEntryIntoBucket(
		AosRundata						*rdata,
		const AosOprIDCache::OprIDEntry	&sOprIDEntry)
{
	AosBucketPtr pBucket = getBucketByOprID(rdata, sOprIDEntry.ullOprID);
	if (pBucket.isNull())
	{
		OmnAlarm << "getBucketByOprID failed," << sOprIDEntry.toStr() << enderr;
		return -1;
	}
	return pBucket->putOprIDEntryIntoBucket(rdata, sOprIDEntry);
}


int
AosBucketMgr::putHeaderIntoBucket(
		AosRundata				*rdata,
		const AosBlobHeaderPtr	&pHeader)
{
	AosBucketPtr pBucket = getBucketByOprID(rdata, pHeader->getOprID());
	if (pBucket.isNull())
	{
		OmnAlarm << "getBucketByOprID failed, oprid:" << pHeader->getOprID() << enderr;
		return -1;
	}
	return pBucket->putHeaderIntoBucket(rdata, pHeader);
}


AosBucketPtr
AosBucketMgr::getBucketByOprID(
		AosRundata	*rdata,
		const u64	ullOprID)
{
	u32 ulBucketID = getBucketIDByOprID(ullOprID);
	if (0 == ulBucketID)
	{
		OmnAlarm << "getBucketIDByOprID failed, oprid:" << ullOprID << enderr;
		return NULL;
	}
	bucket_map_t::iterator it = mBucketMap.find(ulBucketID);
	if (mBucketMap.end() == it)
	{
		OmnScreen << "can not find bucket for bucket id:" << ulBucketID << " oprid:" << ullOprID
				<< ", just create one" << endl;
		AosBucketPtr pBucket = OmnNew AosBucket(rdata, ulBucketID, mBlobSE, mRawSE, mHeaderFileMgr,
				mHeaderBuffAndFileMgr, mNormalHeaderCache);
		mBucketMap[ulBucketID] = pBucket;
		return pBucket;
	}
	return it->second;
}


u32
AosBucketMgr::getBucketIDByOprID(const u64 ullOprID)
{
	if (0 == ullOprID)
	{
		return 0;
	}
	return (ullOprID - 1) / mOprIDsPerBucket + 1;
}


int
AosBucketMgr::restoreHeaders(AosRundata *rdata)
{
	int iRet = 0;
	for (bucket_map_t::iterator it = mBucketMap.begin(); it != mBucketMap.end(); it++)
	{
		iRet = it->second->restoreHeaders(rdata);
		if (0 != iRet)
		{
			OmnAlarm << "restoreHeaders failed, iRet=" << iRet << " bucket id:" << it->second->getBucketID() << enderr;
			return -1;
		}
	}
	return 0;
}
#endif
