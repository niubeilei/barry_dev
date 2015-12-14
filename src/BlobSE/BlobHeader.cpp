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
// 2015-3-25 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/BlobHeader.h"

#include "alarm_c/alarm.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/RawfidRange.h"
#include "BlobSE/SanityCheck.h"
#include "RawSE/Ptrs.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"

const AosBlobHeader::HeaderPointer AosBlobHeader::unresolvedNextHeader = {0xFFFFFFFF, 0x7FFFF};	//prime number
const AosBlobHeader::HeaderPointer AosBlobHeader::noNextHeader = {0xFFFFFFFF, 0x663D81};	//prime number
const AosBlobHeader::HeaderPointer AosBlobHeader::noPrevHeader = {0xEEEEEEEE, 0x7FFFFFFF};	//prime number

AosBlobHeader::AosBlobHeader(
		const u64	docid,
		const u32	customDataSize)
:
mPrev(0),
mNext(0),
mDocid(docid),
mTimestamp(0),
mBodyRawfid(0),
mBodyOffset(0),
mBodyLen(0),
mCustomDataSize(customDataSize)
{
	mCustomData = OmnNew AosBuff(customDataSize AosMemoryCheckerArgs);
	if (mCustomData.isNull())
	{
		OmnAlarm << "mCustomData.isNull()" << enderr;
		return;
	}
	memset(mCustomData->data(), 0, mCustomData->buffLen());
	mCustomData->setDataLen(customDataSize);
	mNextHeader = unresolvedNextHeader;
}


AosBlobHeader::~AosBlobHeader()
{
    // TODO Auto-generated destructor stub
}


AosBlobHeader::AosBlobHeader(const AosBlobHeader *header)
{
	this->clone(header);
}


void
AosBlobHeader::clone(const AosBlobHeader* header)
{

}


int
AosBlobHeader::serializeToCacheAndHeaderFileBuff(char* buff)
{
	aos_assert_r(buff, -1);
	HeaderInHeaderFileAndCache* p = (HeaderInHeaderFileAndCache*)buff;
	p->ullDocid = mDocid;
	p->ullBodyRawfid = mBodyRawfid;
	p->ullBodyOffset = mBodyOffset;
	p->ullTimestamp = mTimestamp;
	p->ullSnapshotID = mSnapshotID;
	p->ullBodyLen = mBodyLen;
	p->sPrevHeaderInFile = mPrevHeader;
	p->sNextHeaderInFile = mNextHeader;
	memcpy(p->pCustomData, mCustomData->data(), mCustomDataSize);
	return 0;
}


int
AosBlobHeader::unserializeFromCacheAndHeaderFileBuff(const char* buff)
{
	aos_assert_r(buff, -1);
	HeaderInHeaderFileAndCache* p = (HeaderInHeaderFileAndCache*)buff;
	mDocid = p->ullDocid;
	mBodyRawfid = p->ullBodyRawfid;
	sanity_check((eActiveLogFileID_start <= mBodyRawfid && eActiveLogFileID_end >= mBodyRawfid) || 0 == mBodyRawfid, -99,
			"mBodyRawfid:%llu is out of legal ranged:[%llu,%llu]", mBodyRawfid, eActiveLogFileID_start, eActiveLogFileID_end);
	mBodyOffset = p->ullBodyOffset;
	mTimestamp = p->ullTimestamp;
	mSnapshotID = p->ullSnapshotID;
	mBodyLen = p->ullBodyLen;
	if (AosHeaderFileMgr::eHeaderFileSeqnoGuard == p->sNextHeaderInFile.ulSeqno)
	{
		return -2;
	}
	mPrevHeader = p->sPrevHeaderInFile;
	sanity_check(mPrevHeader.ulSeqno <= eOldHeaderFileIDCount || noPrevHeader == mPrevHeader, -99,
			"mPrevHeader.ulSeqno:%u > eOldHeaderFileIDCount:%u", mPrevHeader.ulSeqno, eOldHeaderFileIDCount);
	mNextHeader = p->sNextHeaderInFile;
	sanity_check(mNextHeader.ulSeqno <= eOldHeaderFileIDCount || noNextHeader == mNextHeader
			|| unresolvedNextHeader == mNextHeader, -99, "mNextHeader.ulSeqno:%u > eOldHeaderFileIDCount:%u",
			mNextHeader.ulSeqno, eOldHeaderFileIDCount);
	if (mCustomData->buffLen() < mCustomDataSize)
	{
		bool rslt = mCustomData->expandMemory1(mCustomDataSize);
		aos_assert_r(rslt, -3);
	}
	memcpy(mCustomData->data(), p->pCustomData, mCustomDataSize);
	mCustomData->setDataLen(mCustomDataSize);
	return 0;
}


int
AosBlobHeader::serializeToACLandHCLBuff(const char* buff)
{
	aos_assert_r(buff, -1);
	HeaderInACLAndHCL* p = (HeaderInACLAndHCL*)buff;
	p->ullTimestamp = mTimestamp;
	p->ullBodyLen = mBodyLen;
	memcpy(p->pCustomData, mCustomData->data(), mCustomDataSize);
	return 0;
}


int
AosBlobHeader::unserializeFromACLandHCLBuff(const char* buff)
{
	aos_assert_r(buff, -1);
	HeaderInACLAndHCL* p = (HeaderInACLAndHCL*)buff;
	mTimestamp = p->ullTimestamp;
	mBodyLen = p->ullBodyLen;
	if (mCustomData->buffLen() < mCustomDataSize)
	{
		bool rslt = mCustomData->expandMemory1(mCustomDataSize);
		aos_assert_r(rslt, -2);
	}
	memcpy(mCustomData->data(), p->pCustomData, mCustomDataSize);
	mCustomData->setDataLen(mCustomDataSize);
	return 0;
}


int
AosBlobHeader::serializeToHeaderBucketFileBuff(char* buff)
{
	aos_assert_r(buff, -1);
	HeaderInHeaderBucket* p = (HeaderInHeaderBucket*)buff;
	p->ullDocid = mDocid;
	p->ullBodyRawfid = mBodyRawfid;
	p->ullBodyOffset = mBodyOffset;
	p->ullTimestamp = mTimestamp;
	p->ullSnapshotID = mSnapshotID;
	p->ullBodyLen = mBodyLen;
	p->sPrevHeaderInFile = mPrevHeader;
	p->sNextHeaderInFile = mNextHeader;
	p->ucStatus = mStatus;
	memcpy(p->pCustomData, mCustomData->data(), mCustomDataSize);
	return 0;
}


int
AosBlobHeader::unserializeFromHeaderBucketFileBuff(const char* buff)
{
	aos_assert_r(buff, -1);
	HeaderInHeaderBucket* p = (HeaderInHeaderBucket*)buff;
	mDocid = p->ullDocid;
	mBodyRawfid = p->ullBodyRawfid;
	sanity_check((eActiveLogFileID_start <= mBodyRawfid && eActiveLogFileID_end >= mBodyRawfid) || 0 == mBodyRawfid, -99,
			"mBodyRawfid:%llu is out of legal ranged:[%llu,%llu]", mBodyRawfid, eActiveLogFileID_start, eActiveLogFileID_end);
	mBodyOffset = p->ullBodyOffset;
	mTimestamp = p->ullTimestamp;
	mSnapshotID = p->ullSnapshotID;
	mBodyLen = p->ullBodyLen;
	if (AosHeaderFileMgr::eHeaderFileSeqnoGuard == p->sNextHeaderInFile.ulSeqno)
	{
		return -2;
	}
	mPrevHeader = p->sPrevHeaderInFile;
	sanity_check(mPrevHeader.ulSeqno <= eOldHeaderFileIDCount || noPrevHeader == mPrevHeader, -99,
			"mPrevHeader.ulSeqno:%u > eOldHeaderFileIDCount:%u", mPrevHeader.ulSeqno, eOldHeaderFileIDCount);
	mNextHeader = p->sNextHeaderInFile;
	sanity_check(mNextHeader.ulSeqno <= eOldHeaderFileIDCount || noNextHeader == mNextHeader
			|| unresolvedNextHeader == mNextHeader, -99, "mNextHeader.ulSeqno:%u > eOldHeaderFileIDCount:%u",
			mNextHeader.ulSeqno, eOldHeaderFileIDCount);
	mStatus = (HeaderStatus)p->ucStatus;
	if (mCustomData->buffLen() < mCustomDataSize)
	{
		bool rslt = mCustomData->expandMemory1(mCustomDataSize);
		aos_assert_r(rslt, -3);
	}
	memcpy(mCustomData->data(), p->pCustomData, mCustomDataSize);
	mCustomData->setDataLen(mCustomDataSize);
	return 0;
}

bool
AosBlobHeader::compareTimestamp(
			AosBlobHeaderPtr l,
			AosBlobHeaderPtr r)
{
	return l->getTimestamp() > r->getTimestamp();
}


void
AosBlobHeader::dump()
{
	OmnScreen << "header:" << endl
			<< "           doicd: " << mDocid << endl
			<< "       timestamp: " << mTimestamp << endl
			<< "     snapshot id: " << mSnapshotID << endl
			<< "     body rawfid: " << mBodyRawfid << endl
			<< "     body offset: " << mBodyOffset << endl
			<< "        body len: " << mBodyLen << endl
			<< "     prev header: " << mPrevHeader.toStr() << endl
			<< "     next header: " << mNextHeader.toStr() << endl
			<< "custom data size: " << mCustomDataSize << endl;
	mCustomData->dumpData(true, "custom data:'");	//dumpData will change line and flush
}
