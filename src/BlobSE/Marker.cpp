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
// 11 Aug 2015 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/Marker.h"

#include "Alarm/Alarm.h"
#include "Util/Buff.h"

AosMarker::AosMarker(MarkerType eType)
:
mType(eType)
{

}


AosMarker::~AosMarker()
{
}


u32
AosMarker::calculateEntryLen()
{
	switch (mType)
	{
	case eHeaderStartToSave:
	case eHeaderSaved:
	case eCompactionStart:
	case eCompactionFinished:
		mLength = sizeof(OperationEntry);
		break;
	case eStartToProcessHeader:
	case eProcessHeaderFinished:
		mLength = sizeof(CompactionHeaderEntry);
		break;
	case eStartToProcessBody:
	case eProcessBodyFinished:
		mLength = sizeof(CompactionBodyEntry);
		break;
	default:
		return mLength;
	}
	return mLength;
}


int
AosMarker::unserializeFromBuff(const AosBuffPtr	&pBuff)
{
	if (pBuff.isNull())
	{
		return -1;
	}
	if ((i64)sizeof(OperationEntry) != pBuff->dataLen() && (i64)sizeof(CompactionHeaderEntry) != pBuff->dataLen()
			&& (i64)sizeof(CompactionBodyEntry) != pBuff->dataLen())		//the smallest one
	{
		OmnAlarm << "illegal pBuff->dataLen():" << pBuff->dataLen() << " it should be one of the followings\n"
				<< " sizeof(OperationEntry):" << sizeof(OperationEntry)
				<< " sizeof(CompactionHeaderEntry):" << sizeof(CompactionHeaderEntry)
				<< " sizeof(CompactionBodyEntry):" << sizeof(CompactionBodyEntry) << enderr;
		return -2;
	}
	MarkerEntryHead* pGeneralHead = (MarkerEntryHead*)pBuff->data();
	CompactionHeaderEntry* pCompactionHeaderEntry = (CompactionHeaderEntry*)pBuff->data();
	CompactionBodyEntry* pCompactionBodyEntry = (CompactionBodyEntry*)pBuff->data();
	mLength = pGeneralHead->ulLength;
	mOprID = pGeneralHead->ullOprID;
	mType = (MarkerType)pGeneralHead->ucType;
	mRawfid = pGeneralHead->ullRawfid;
	mOffset = pGeneralHead->ullOffset;
	switch (pGeneralHead->ucType)
	{
	case eHeaderStartToSave:
	case eHeaderSaved:
	case eCompactionStart:
	case eCompactionFinished:
		break;
	case eStartToProcessHeader:
	case eProcessHeaderFinished:
		mSeqnoBeingCompacted = pCompactionHeaderEntry->ulSeqnoBeingCompacted;
		mSeqnoCompacted = pCompactionHeaderEntry->ulSeqnoCompacted;
		break;
	case eStartToProcessBody:
	case eProcessBodyFinished:
		mBodyRawfidBeingCompacted = pCompactionBodyEntry->ullBodyRawfidBeingCompacted;
		mBodyRawfidCompacted = pCompactionBodyEntry->ullBodyRawfidCompacted;
		break;
	default:
		OmnAlarm << "unsupported entry type:" << pGeneralHead->ucType << enderr;
		return -99;
		break;
	}
	return 0;
}


int
AosMarker::serializeToBuff(const AosBuffPtr	&pBuff)
{
	if (pBuff.isNull())
	{
		return -1;
	}
	calculateEntryLen();
	if (pBuff->buffLen() < mLength)
	{
		if (!pBuff->expandMemory1(mLength))
		{
			OmnAlarm << "pBuff->expandMemory1 failed, trying to expand memory to len:" << mLength << enderr;
			return -2;
		}
	}
	pBuff->setDataLen(mLength);
	MarkerEntryHead* pGeneralHead = (MarkerEntryHead*)pBuff->data();
	OperationEntry* pOperationEntry = (OperationEntry*)pBuff->data();
	CompactionHeaderEntry* pCompactionHeaderEntry = (CompactionHeaderEntry*)pBuff->data();
	CompactionBodyEntry* pCompactionBodyEntry = (CompactionBodyEntry*)pBuff->data();
	pGeneralHead->ulLength = mLength;
	pGeneralHead->ucType = mType;
	pGeneralHead->ullOprID = mOprID;
	pGeneralHead->ullRawfid = mRawfid;
	pGeneralHead->ullOffset = mOffset;
	switch (mType)
	{
	case eHeaderStartToSave:
	case eHeaderSaved:
	case eCompactionStart:
	case eCompactionFinished:
		pOperationEntry->ulLengthTail = mLength;
		break;
	case eStartToProcessHeader:
	case eProcessHeaderFinished:
		pCompactionHeaderEntry->ulSeqnoBeingCompacted = mSeqnoBeingCompacted;
		pCompactionHeaderEntry->ulSeqnoCompacted = mSeqnoCompacted;
		pCompactionHeaderEntry->ulLengthTail = mLength;
		break;
	case eStartToProcessBody:
	case eProcessBodyFinished:
		pCompactionBodyEntry->ullBodyRawfidBeingCompacted = mBodyRawfidBeingCompacted;
		pCompactionBodyEntry->ullBodyRawfidCompacted = mBodyRawfidCompacted;
		pCompactionBodyEntry->ulLengthTail = mLength;
		break;
	default:
		OmnAlarm << "unsupported entry type:" << mType << enderr;
		return -99;
		break;
	}
	return 0;
}
