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
#include "BlobSE/BodyEntry.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobHeader.h"
#include "Util/Buff.h"
AosBodyEntry::AosBodyEntry(const u32 ulHeaderCustomDataSize)
:
mLength(0),
mType(eInvalid),
mOprID(0),
mDocid(0),
mSnapshotID(0),
mTimestamp(0),
mHeader(OmnNew AosBlobHeader(0, ulHeaderCustomDataSize)),
mBodyDataSize(0),
mBodyBuff(0)
{
}

AosBodyEntry::~AosBodyEntry()
{
}


u32
AosBodyEntry::calculateEntryLen()
{
	switch (mType)
	{
	case eSave:
		if (mHeader.isNull())
		{
			return 0;
		}
		if (mBodyBuff.isNull())
		{
			mLength = mHeader->getHeaderInACLAndHCLLen() + mBodyDataSize + sizeof(SaveEntry) + sizeof(u32);
		}
		else
		{
			mLength = mHeader->getHeaderInACLAndHCLLen() + mBodyBuff->dataLen() + sizeof(SaveEntry) + sizeof(u32);
		}
		break;
	case eDelete:
		mLength = sizeof(DeleteEntry);
		break;
	case eCreateSnapshot:
	case eCommitSnapshot:
	case eCancelSnapshot:
		mLength = sizeof(OperationEntry);
		break;
	default:
		return mLength;
	}
	return mLength;
}


int
AosBodyEntry::unserializeFromBuff(const AosBuffPtr	&pBuff)
{
	if (pBuff.isNull())
	{
		return -1;
	}
	if ((i64)sizeof(OperationEntry) > pBuff->dataLen())		//the smallest one
	{
		OmnAlarm << "sizeof(OperationEntry):" << sizeof(OperationEntry)
				<< " > pBuff->dataLen():" << pBuff->dataLen() << enderr;
		return -2;
	}
	BodyEntryGeneralHead* pGeneralHead = (BodyEntryGeneralHead*)pBuff->data();
	if ((u32)pBuff->dataLen() < pGeneralHead->ulLength)
	{
		OmnAlarm << "(u32)pBuff->dataLen():" << (u32)pBuff->dataLen()
				<< " < pGeneralHead->ulLength:" << pGeneralHead->ulLength << enderr;
	}
	mLength = pGeneralHead->ulLength;
	mType = (EntryType)pGeneralHead->ucType;
	mTermID = pGeneralHead->ulTermID;
	mOprID = pGeneralHead->ullOprID;
	SaveEntry* pSaveEntry = (SaveEntry*)pBuff->data();
	DeleteEntry* pDeleteEntry = (DeleteEntry*)pBuff->data();
	OperationEntry* pOperationEntry = (OperationEntry*)pBuff->data();
	int iRet = 0;
	switch (pGeneralHead->ucType)
	{
	case eSave:
		mDocid = pSaveEntry->sBodyEntryHead.ullDocid;
		mSnapshotID = pSaveEntry->sBodyEntryHead.ullSnapshotID;
		iRet = mHeader->unserializeFromACLandHCLBuff(pSaveEntry->pData);
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->unserializeFromACLandHCLBuff failed, iRet=" << iRet << enderr;
			return -3;
		}
		mHeader->setSnapshotID(mSnapshotID);
		mHeader->setDocid(mDocid);
		mHeader->setOprID(mOprID);
		mHeader->setOprType(AosBlobHeader::eDocOperation);
		mTimestamp = mHeader->getTimestamp();
		if (mHeader->getBodyLen() + mHeader->getHeaderInACLAndHCLLen()
				!= pSaveEntry->sBodyEntryHead.sGeneralHead.ulLength - sizeof(SaveEntry) - sizeof(u32))
		{
			OmnAlarm << "mHeader->getBodyLen():" << mHeader->getBodyLen()
					<< " + mHeader->getHeaderInACLAndHCLLen():" << mHeader->getHeaderInACLAndHCLLen()
					<< "!= pSaveEntry->sBodyEntryHead.sGeneralHead.ulLength:" << pSaveEntry->sBodyEntryHead.sGeneralHead.ulLength
					<< " - sizeof(SaveEntry):" << sizeof(SaveEntry) << " - sizeof(u32):" << sizeof(u32) << enderr;
			return -4;
		}
		if (mBodyBuff.isNull())
		{
			mBodyBuff = OmnNew AosBuff(mHeader->getBodyLen() AosMemoryCheckerArgs);
		}
		if (!mBodyBuff->setBuff(pSaveEntry->pData + mHeader->getHeaderInACLAndHCLLen(), mHeader->getBodyLen()))
		{
			OmnAlarm << "mBodyBuff->setBuff failed, len:" << mHeader->getBodyLen() << enderr;
			return -5;
		}
		mBodyBuff->setDataLen(mHeader->getBodyLen());
		break;
	case eDelete:
		mDocid = pDeleteEntry->sBodyEntryHead.ullDocid;
		mSnapshotID = pDeleteEntry->sBodyEntryHead.ullSnapshotID;
		mTimestamp = pDeleteEntry->ullTimestamp;
		mHeader->setBodyRawfid(0);
		mHeader->setBodyOffset(0);
		mHeader->setTimestamp(mTimestamp);
		mHeader->setSnapshotID(mSnapshotID);
		mHeader->setDocid(mDocid);
		mHeader->setOprID(mOprID);
		mHeader->setOprType(AosBlobHeader::eDocOperation);
		break;
	case eCreateSnapshot:
	case eCommitSnapshot:
	case eCancelSnapshot:
		mSnapshotID = pOperationEntry->ullSanpshotID;
		break;
	default:
		OmnAlarm << "unsupported entry type:" << pGeneralHead->ucType << enderr;
		return -99;
		break;
	}
	return 0;
}


int
AosBodyEntry::serializeToBuff(const AosBuffPtr	&pBuff)
{
	if (pBuff.isNull())
	{
		return -1;
	}
	if (pBuff->buffLen() < mLength)
	{
		if (!pBuff->expandMemory1(mLength))
		{
			OmnAlarm << "pBuff->expandMemory1 failed, trying to expand memory to len:" << mLength << enderr;
			return -2;
		}
	}
	pBuff->setDataLen(mLength);
	BodyEntryGeneralHead* pBodyEntryGeneralHead = (BodyEntryGeneralHead*)pBuff->data();
	pBodyEntryGeneralHead->ulLength = mLength;
	pBodyEntryGeneralHead->ucType = mType;
	pBodyEntryGeneralHead->ulTermID = mTermID;
	pBodyEntryGeneralHead->ullOprID = mOprID;
	SaveEntry* pSaveEntry = (SaveEntry*)pBuff->data();
	DeleteEntry* pDeleteEntry = (DeleteEntry*)pBuff->data();
	OperationEntry* pOperationEntry = (OperationEntry*)pBuff->data();
	int iRet = 0;
	switch (mType)
	{
	case eSave:
		pSaveEntry->sBodyEntryHead.ullDocid = mDocid;
		pSaveEntry->sBodyEntryHead.ullSnapshotID = mSnapshotID;
		iRet = mHeader->serializeToACLandHCLBuff(pSaveEntry->pData);
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->serializeToACLandHCLBuff failed, iRet=" << iRet << enderr;
			return -3;
		}
		if (mBodyBuff.isNull())
		{
			OmnAlarm << "mBodyBuff should be set properly." << enderr;
			return -4;
		}
		memcpy(pSaveEntry->pData + mHeader->getHeaderInACLAndHCLLen(), mBodyBuff->data(), mBodyBuff->dataLen());
		*(u32*)(pSaveEntry->pData + mHeader->getHeaderInACLAndHCLLen() + mBodyBuff->dataLen()) = mLength;
		break;
	case eDelete:
		pDeleteEntry->sBodyEntryHead.ullDocid = mDocid;
		pDeleteEntry->sBodyEntryHead.ullSnapshotID = mSnapshotID;
		pDeleteEntry->ullTimestamp = mTimestamp;
		pDeleteEntry->ulLengthTail = mLength;
		break;
	case eCreateSnapshot:
	case eCommitSnapshot:
	case eCancelSnapshot:
		pOperationEntry->ullSanpshotID = mSnapshotID;
		pOperationEntry->ulLengthTail = mLength;
		break;
	default:
		OmnAlarm << "unsupported entry type:" << mType << enderr;
		return -99;
		break;
	}
	return 0;
}
