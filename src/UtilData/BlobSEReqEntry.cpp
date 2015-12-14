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
#include "UtilData/BlobSEReqEntry.h"

#include "Alarm/Alarm.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"

AosBlobSEReqEntry::AosBlobSEReqEntry()
:
mDocid(0),
mSnapshotID(0),
mOprID(0),
mTimestamp(0),
mHeaderCustomDataSize(0),
mReqTypeAndResult(eInvalid),
mHeaderCustomData(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mBodyData(OmnNew AosBuff(AosMemoryCheckerArgsBegin))
{
}


AosBlobSEReqEntry::AosBlobSEReqEntry(const u32 ulHeaderCumstomDataSize)
:
mDocid(0),
mSnapshotID(0),
mOprID(0),
mTimestamp(0),
mHeaderCustomDataSize(ulHeaderCumstomDataSize),
mReqTypeAndResult(eInvalid),
mHeaderCustomData(OmnNew AosBuff(ulHeaderCumstomDataSize AosMemoryCheckerArgs)),
mBodyData(OmnNew AosBuff(AosMemoryCheckerArgsBegin))
{
	memset(mHeaderCustomData->data(), 0, ulHeaderCumstomDataSize);
	mBodyData->setDataLen(0);
}


AosBlobSEReqEntry::~AosBlobSEReqEntry()
{
    // TODO Auto-generated destructor stub
}


int
AosBlobSEReqEntry::unserializeFromBuff(const AosBuffPtr	&pBuff)
{
	if (pBuff.isNull())
	{
		return -1;
	}
	if ((i64)(sizeof(BlobSEReqEntry)) > pBuff->dataLen())
	{
		OmnAlarm << "sizeof(BlobSEReqEntry):" << sizeof(BlobSEReqEntry)
				<< " > pBuff->dataLen():" << pBuff->dataLen() << enderr;
		return -2;
	}
	BlobSEReqEntry* pBlobSEReqEntry = (BlobSEReqEntry*)pBuff->data();
	mDocid				= pBlobSEReqEntry->ullDocid;
	mSnapshotID			= pBlobSEReqEntry->ullSnapshotID;
	mOprID				= pBlobSEReqEntry->ullOprID;
	mTimestamp			= pBlobSEReqEntry->ullTimestamp;
	mReqTypeAndResult	= pBlobSEReqEntry->ucReqTypeAndResult;
	if (eNotExist == mReqTypeAndResult)
	{
		return 0;
	}
	if ((i64)(sizeof(BlobSEReqEntry) + pBlobSEReqEntry->ullBodyDataLen + mHeaderCustomDataSize) != pBuff->dataLen())
	{
		OmnAlarm << "sizeof(BlobSEReqEntry):" << sizeof(BlobSEReqEntry)
				<< " + pBlobSEReqEntry->ullBodyDataLen:" << pBlobSEReqEntry->ullBodyDataLen
				<< " + mHeaderCustomDataSize:" << mHeaderCustomDataSize
				<< " != pBuff->dataLen():" << pBuff->dataLen() << enderr;
		return -3;
	}
	if (0 < mHeaderCustomDataSize)
	{
		if (!mHeaderCustomData->setBuff(pBlobSEReqEntry->pData, mHeaderCustomDataSize))
		{
			OmnAlarm << "mHeaderCumstomData->setBuff failed, len:" << mHeaderCustomDataSize << enderr;
			return -4;
		}
		mHeaderCustomData->setDataLen(mHeaderCustomDataSize);
	}
	if (0 < pBlobSEReqEntry->ullBodyDataLen)
	{
		if (!mBodyData->setBuff(pBlobSEReqEntry->pData + mHeaderCustomDataSize, pBlobSEReqEntry->ullBodyDataLen))
		{
			OmnAlarm << "mBodyData->setBuff failed, len:" << pBlobSEReqEntry->ullBodyDataLen << enderr;
			return -5;
		}
		mBodyData->setDataLen(pBlobSEReqEntry->ullBodyDataLen);
	}
	return 0;
}


int
AosBlobSEReqEntry::serializeToBuff(const AosBuffPtr	&pBuff)
{
	if (pBuff.isNull())
	{
		return -1;
	}
	if (eNotExist == mReqTypeAndResult)
	{
		if ((i64)sizeof(BlobSEReqEntry) > pBuff->buffLen())
		{
			if (!pBuff->expandMemory1(sizeof(BlobSEReqEntry)))
			{
				OmnAlarm << "pBuff->expandMemory1 failed, trying to expand memory to len:"
						<< sizeof(BlobSEReqEntry) << enderr;
				return -2;
			}
		}
	}
	else if ((i64)(sizeof(BlobSEReqEntry) + mHeaderCustomDataSize +
			(mBodyData.notNull() ? mBodyData->dataLen() : 0) > pBuff->buffLen()))
	{
		if (!pBuff->expandMemory1(sizeof(BlobSEReqEntry) + mHeaderCustomDataSize +
				(mBodyData.notNull() ? mBodyData->dataLen() : 0)))
		{
			OmnAlarm << "pBuff->expandMemory1 failed, trying to expand memory to len:"
					<< sizeof(BlobSEReqEntry) + mHeaderCustomDataSize + mBodyData->dataLen() << enderr;
			return -3;
		}
	}
	BlobSEReqEntry* pBlobSEReqEntry = (BlobSEReqEntry*)pBuff->data();
	pBlobSEReqEntry->ullDocid				= mDocid;
	pBlobSEReqEntry->ullSnapshotID			= mSnapshotID;
	pBlobSEReqEntry->ullOprID				= mOprID;
	pBlobSEReqEntry->ullTimestamp			= mTimestamp;
	pBlobSEReqEntry->ucReqTypeAndResult		= mReqTypeAndResult;
	pBlobSEReqEntry->ullBodyDataLen			= mBodyData.notNull() ? mBodyData->dataLen() : 0;
	if (eNotExist == mReqTypeAndResult)
	{
		pBuff->setDataLen(sizeof(BlobSEReqEntry));
		return 0;
	}
	if (0 < mHeaderCustomDataSize)
	{
		if (!mHeaderCustomData->data()
				&& eRead != mReqTypeAndResult && eDelete != mReqTypeAndResult && eDeleted != mReqTypeAndResult)
		{
			OmnAlarm << "mHeaderCustomData->data() is NULL, req type:" << mReqTypeAndResult << enderr;
			return -4;
		}
		memcpy(pBlobSEReqEntry->pData, mHeaderCustomData->data(), mHeaderCustomDataSize);
	}
	if (0 < (mBodyData.notNull() ? mBodyData->dataLen() : 0))
	{
		memcpy(pBlobSEReqEntry->pData + mHeaderCustomDataSize, mBodyData->data(), mBodyData->dataLen());
	}
	pBuff->setDataLen(sizeof(BlobSEReqEntry) + mHeaderCustomDataSize + (mBodyData.notNull() ? mBodyData->dataLen() : 0));
	return 0;
}
