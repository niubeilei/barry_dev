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
#ifndef Aos_BlobSE_BodyEntry_H_
#define Aos_BlobSE_BodyEntry_H_
#include "BlobSE/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosBodyEntry : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
#pragma pack(1)
	enum EntryType
	{
		eInvalid				= 0,
		eSave					= 1,
		eDelete					= 2,
		eCreateSnapshot			= 13,
		eCommitSnapshot			= 14,
		eCancelSnapshot			= 15,
	};

	struct BodyEntryGeneralHead
	{
		u32	ulLength;
		u8	ucType;
		u32	ulTermID;
		u64	ullOprID;
	};

	struct BodyEntryHead
	{
		BodyEntryGeneralHead	sGeneralHead;
		u64						ullDocid;
		u64						ullSnapshotID;
	};

	struct SaveEntry
	{
		BodyEntryHead	sBodyEntryHead;
		char			pData[];	//header + body
	};

	struct DeleteEntry
	{
		BodyEntryHead	sBodyEntryHead;
		u64				ullTimestamp;
		u32				ulLengthTail;
	};

	struct OperationEntry	//operation info, including header saving operations and snapshot operations, etc.
	{
		BodyEntryGeneralHead	sGeneralHead;
		u64						ullSanpshotID;
		u32						ulLengthTail;
	};

#pragma pack(0)

private:
	u32					mLength;
	EntryType			mType;
	u32					mTermID;
	u64					mOprID;
	u64					mDocid;
	u64					mSnapshotID;
	u64					mTimestamp;
	AosBlobHeaderPtr	mHeader;
	u32					mBodyDataSize;
	AosBuffPtr			mBodyBuff;

public:
    AosBodyEntry(const u32 ulHeaderCustomDataSize);
    virtual ~AosBodyEntry();

    u32					getLength()			const {return mLength;}
    EntryType			getEntryType()		const {return mType;}
    u32					getTermID()			const {return mTermID;}
    u64					getOprID()			const {return mOprID;}
    u64					getDocid()			const {return mDocid;}
    u64					getSnapshotID()		const {return mSnapshotID;}
    u64					getTimestamp()		const {return mTimestamp;}
    u32					getBodyDataSize()	const {return mBodyDataSize;}
    AosBlobHeaderPtr	getHeader()			const {return mHeader;}
    AosBuffPtr			getBodyBuff()		const {return mBodyBuff;}

    void	setLength(const u32 len)			{mLength = len;}
    void	setEntryType(const EntryType type)	{mType = type;}
    void	setTermID(const u32 termid)			{mTermID = termid;}
    void	setOprID(const u64 oprid)			{mOprID = oprid;}
    void	setDocid(const u64 docid)			{mDocid = docid;}
    void	setSnapshotID(const u64 snapid)		{mSnapshotID = snapid;}
    void	setTimestamp(const u64 t)			{mTimestamp = t;}
    void	setHeader(AosBlobHeaderPtr header)	{mHeader = header;}
    void	setBodyBuff(AosBuffPtr pBodyBuff)	{mBodyBuff = pBodyBuff;}
    void	setBodyDataSize(const u32 size)		{mBodyDataSize = size;}

    u32	calculateEntryLen();

	int unserializeFromBuff(const AosBuffPtr	&pBuff);

	int serializeToBuff(const AosBuffPtr	&pBuff);
};

#endif /* Aos_BlobSE_BodyEntry_H_ */
