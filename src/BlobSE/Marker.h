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
#ifndef Aos_BlobSE_Marker_H_
#define Aos_BlobSE_Marker_H_
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosMarker : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum MarkerType
	{
		eInvalid				= 0,
		eHCLDeltaStartToSave	= 3,
		eHCLDeltaSaved			= 4,
		eHeaderStartToSave		= 5,
		eHeaderSaved			= 6,
		eCompactionStart		= 7,
		eCompactionFinished		= 8,
		eStartToProcessHeader	= 9,
		eProcessHeaderFinished	= 10,
		eStartToProcessBody		= 11,
		eProcessBodyFinished	= 12,
	};

	enum DefaultValues
	{
		eInvalidRawfid			= 0,
	};

#pragma pack(1)
	struct MarkerEntryHead
	{
		u32	ulLength;
		u8	ucType;
		u64 ullOprID;
		u64	ullRawfid;
		u64	ullOffset;
	};

	struct OperationEntry	//operation info, including header saving operations and snapshot operations, etc.
	{
		MarkerEntryHead	sHead;
		u32				ulLengthTail;
	};

	struct CompactionHeaderEntry
	{
		MarkerEntryHead	sHead;
		u32				ulSeqnoBeingCompacted;
		u32				ulSeqnoCompacted;
		u32				ulLengthTail;
	};

	struct CompactionBodyEntry
	{
		MarkerEntryHead	sHead;
		u64				ullBodyRawfidBeingCompacted;
		u64				ullBodyRawfidCompacted;
		u32				ulLengthTail;
	};

	struct MarkerFile
	{
		OperationEntry			sHeaderStartToSave;
		OperationEntry			sHeaderSaved;
		OperationEntry			sCompactionStart;
		OperationEntry			sCompactionFinished;
		CompactionHeaderEntry	sCompactionHeader;
		CompactionBodyEntry		sCompactionBody;
	};
#pragma pack(0)

private:
	MarkerType			mType;
	u32					mLength;
	u32					mSeqnoBeingCompacted;
	u32					mSeqnoCompacted;
	u64					mBodyRawfidBeingCompacted;
	u64					mBodyRawfidCompacted;
	u64					mOprID;		//the oprid right before the marker
	u64					mRawfid;
	u64					mOffset;

public:
    AosMarker(MarkerType eType);
    virtual ~AosMarker();

    MarkerType	getType()						const {return mType;}
    u32			getLength()						const {return mLength;}
    u32			getSeqnoBeingCompacted()		const {return mSeqnoBeingCompacted;}
    u32			getSeqnoCompacted()				const {return mSeqnoCompacted;}
    u64			getBodyRawfidBeingCompacted()	const {return mSeqnoBeingCompacted;}
    u64			getBodyRawfidCompacted()		const {return mSeqnoCompacted;}
    u64			getOprID()						const {return mOprID;}
    u64			getRawfid()						const {return mRawfid;}
    u64			getOffset()						const {return mOffset;}

    void	setType(MarkerType eType)						{mType = eType;}
    void	setLength(const u32 len)						{mLength = len;}
    void	setSeqnoBeingCompacted(const u32 seqno)			{mSeqnoBeingCompacted = seqno;}
    void	setSeqnoCompacted(const u32 seqno)				{mSeqnoCompacted = seqno;}
    void	setBodyRawfidBeingCompacted(const u64 rawfid)	{mBodyRawfidBeingCompacted = rawfid;}
    void	setBodyRawfidCompacted(const u32 rawfid)		{mBodyRawfidCompacted = rawfid;}
    void	setOprID(const u64 oprid)						{mOprID = oprid;}
    void	setRawfid(const u64 rawfid)						{mRawfid = rawfid;}
    void	setOffset(const u64 offset)						{mOffset = offset;}

    u32	calculateEntryLen();

	int unserializeFromBuff(const AosBuffPtr	&pBuff);

	int serializeToBuff(const AosBuffPtr	&pBuff);
};

#endif /* Aos_BlobSE_Marker_H_ */
