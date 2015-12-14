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
#ifndef Aos_BlobSE_BlobHeader_H_
#define Aos_BlobSE_BlobHeader_H_
#include <sstream>

#include "BlobSE/Ptrs.h"
#include "BlobSE/RawfidRange.h"
#include "BlobSE/SanityCheck.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosBlobHeader : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum OperationType
	{
		eInvalid		= 0,
		eDocOperation	= 1,
		eCreateSnapshot	= 2,
		eCommitSnapshot	= 3,
		eCancelSnapshot	= 4,
		eNoOp			= 5,
	};

	enum HeaderStatus
	{
		eGood		= 1,
		eDamaged	= 2,
		eRecoverd	= 3,
		eRecreated	= 4,
	};

#pragma pack(1)
	struct HeaderPointer
	{
		u32	ulSeqno;
		u32	ulOffset;

		HeaderPointer& operator = (const HeaderPointer& r)
		{
			ulSeqno = r.ulSeqno;
			ulOffset = r.ulOffset;
			return *this;
		}

		bool operator == (const HeaderPointer& r) const
		{
			return ulSeqno == r.ulSeqno && ulOffset == r.ulOffset;
		}

		bool operator != (const HeaderPointer& r) const
		{
			return ! (*this == r);
		}

		void dump() const
		{
			if (AosBlobHeader::unresolvedNextHeader == *this)
			{
				OmnScreen << " NextHeader@UnresolvedNextHeader" << endl;
			}
			else if (AosBlobHeader::noNextHeader == *this)
			{
				OmnScreen << " NextHeader@NoNextHeader" << endl;
			}
			else if (AosBlobHeader::noPrevHeader == *this)
			{
				OmnScreen << " NextHeader@noPrevHeader" << endl;
			}
			else
			{
				OmnScreen << " NextHeader@seqno:" << this->ulSeqno << " offset:" << this->ulOffset << endl;
			}
		}

		string toStr() const
		{
			std::stringstream ss;
			ss.str("");
			ss.clear();
			if (AosBlobHeader::unresolvedNextHeader == *this)
			{
				return "UnresolvedNextHeader";
			}
			else if (AosBlobHeader::noNextHeader == *this)
			{
				return "NoNextHeader";
			}
			else if (AosBlobHeader::noPrevHeader == *this)
			{
				return "noPrevHeader";
			}
			else
			{
				ss << "seqno:" << this->ulSeqno << " offset:" << this->ulOffset;
			}
			return ss.str();
		}

//		friend ostream& operator << (ostream &s, const NextHeader &p)
//		{
//			if (AosBlobHeader::unresolvedNextHeader == p)
//			{
//				s << " NextHeader@UnresolvedNextHeader";
//			}
//			else if (AosBlobHeader::noNextHeader == p)
//			{
//				s << " NextHeader@NoNextHeader";
//			}
//			else
//			{
//				s << " NextHeader@seqno:" << p.ulSeqno << " offset:" << p.ulOffset;
//			}
//			return s;
//		}
	};

	struct HeaderInHeaderFileAndCache
	{
		u64				ullDocid;
		u64				ullBodyRawfid;
		u64				ullBodyOffset;
		u64				ullTimestamp;
		u64				ullSnapshotID;
		u64				ullBodyLen;
		HeaderPointer	sPrevHeaderInFile;
		HeaderPointer	sNextHeaderInFile;
		char			pCustomData[];
	};

	struct HeaderInHeaderBucket
	{
		u64				ullDocid;
		u64				ullBodyRawfid;
		u64				ullBodyOffset;
		u64				ullTimestamp;
		u64				ullSnapshotID;
		u64				ullBodyLen;
		HeaderPointer	sSelfPosInFile;
		HeaderPointer	sPrevHeaderInFile;
		HeaderPointer	sNextHeaderInFile;
		u8				ucStatus;
		char			pCustomData[];
	};

	struct HeaderInACLAndHCL
	{
		u64		ullTimestamp;
		u64		ullBodyLen;
		char	pCustomData[];
	};
#pragma pack(0)

	static const HeaderPointer unresolvedNextHeader;
	static const HeaderPointer noNextHeader;
	static const HeaderPointer noPrevHeader;

public:
	AosBlobHeaderPtr	mPrev;		//for aging, should only be used by RawSE
	AosBlobHeaderPtr	mNext;		//for aging, should only be used by RawSE

private:
	u64				mDocid;
	u64				mTimestamp;
	u64				mSnapshotID;
	u64				mBodyRawfid;
	u64				mBodyOffset;
	u64				mBodyLen;
	u64				mOprID;
	u64				mPrevOprID;
	u64				mNextOprID;
	u32				mCustomDataSize;
	AosBuffPtr		mCustomData;
	HeaderPointer	mSelfPos;
	HeaderPointer	mPrevHeader;
	HeaderPointer	mNextHeader;
	OperationType	mOprType;
	HeaderStatus	mStatus;

public:
    AosBlobHeader(
    		const u64	docid,
    		const u32	customDataSize);
    AosBlobHeader(const AosBlobHeader* header);
    virtual ~AosBlobHeader();

	u64 			getBodyRawfid()		const {return mBodyRawfid;}
	u32 			getBodyOffset()		const {return mBodyOffset;}
	u32 			getBodyLen()		const {return mBodyLen;}
	u64 			getOprID()			const {return mOprID;}
	u64 			getPrevOprID()		const {return mPrevOprID;}
	u64 			getNextOprID()		const {return mNextOprID;}
	u64				getTimestamp()		const {return mTimestamp;}
	u64				getSnapshotID()		const {return mSnapshotID;}
	u32				getCustomDataSize()	const {return mCustomDataSize;}
	u64				getDocid()			const {return mDocid;}
	AosBuffPtr 		getCustomData()		const {return mCustomData;}
	HeaderPointer	getSelfPos()		const {return mSelfPos;}
	HeaderPointer	getPrevHeaderPtr()	const {return mPrevHeader;}
	HeaderPointer	getNextHeaderPtr()	const {return mNextHeader;}
	OperationType	getOprType()		const {return mOprType;}
	HeaderStatus	getStatus()			const {return mStatus;}

	void setDocid(const u64 docid)						{mDocid = docid;}
	void setBodyRawfid(const u64 rawfid)				{mBodyRawfid = rawfid;}
	void setBodyOffset(const u32 offset)				{mBodyOffset = offset;}
	void setBodyLen(const u32 length)					{mBodyLen = length;}
	void setOprID(const u64 oprid)						{mOprID = oprid;}
	void setPrevOprID(const u64 oprid)					{mPrevOprID = oprid;}
	void setNextOprID(const u64 oprid)					{mNextOprID = oprid;}
	void setTimestamp(const u64 timestamp)				{mTimestamp = timestamp;}
	void setSnapshotID(const u64 snapshot)				{mSnapshotID = snapshot;}
	void setCustomDataSize(const u32 size)				{mCustomDataSize = size;}
	void setCustomData(const AosBuffPtr buff)			{memcpy(mCustomData->data(), buff->data(), mCustomDataSize);}
	void setOprType(const OperationType type)			{mOprType = type;}
	void setStatus(const HeaderStatus s)				{mStatus = s;}

	void setPrevHeader(const HeaderPointer& sPrevHeader)
	{
		mPrevHeader = sPrevHeader;
		sanity_check_no_ret((noPrevHeader == sPrevHeader || sPrevHeader.ulOffset % getHeaderInCacheAndHeaderFileLength() == 0)
				&& noNextHeader != sPrevHeader && unresolvedNextHeader != sPrevHeader,
				"sPrevHeader.ulOffset:%u mod getHeaderInCacheAndHeaderFileLength():%u != 0 && noPrevHeader == sPrevHeader:%s",
				sPrevHeader.ulOffset, getHeaderInCacheAndHeaderFileLength(), sPrevHeader.toStr().c_str());
	}

	void setNextHeader(const HeaderPointer& sNextHeader)
	{
		sanity_check_no_ret(!(noNextHeader == mNextHeader && sNextHeader == unresolvedNextHeader),
				"noNextHeader == mNextHeader && sNextHeader == unresolvedNextHeader");
		mNextHeader = sNextHeader;
		sanity_check_no_ret((noNextHeader == sNextHeader || unresolvedNextHeader == sNextHeader
				|| sNextHeader.ulOffset % getHeaderInCacheAndHeaderFileLength() == 0) && noPrevHeader != sNextHeader,
				"sNextHeader.ulOffset:%u mod getHeaderInCacheAndHeaderFileLength():%u != 0 && noNextHeader == sNextHeader:%s",
				sNextHeader.ulOffset, getHeaderInCacheAndHeaderFileLength(), sNextHeader.toStr().c_str());
	}

	void setSelfPos(const HeaderPointer& sNextHeader)
	{
		mSelfPos = sNextHeader;
		sanity_check_no_ret((noNextHeader == sNextHeader || unresolvedNextHeader == sNextHeader
				|| sNextHeader.ulOffset % getHeaderInCacheAndHeaderFileLength() == 0) && noPrevHeader != sNextHeader,
				"sNextHeader.ulOffset:%u mod getHeaderInCacheAndHeaderFileLength():%u != 0 && noNextHeader == sNextHeader:%s",
				sNextHeader.ulOffset, getHeaderInCacheAndHeaderFileLength(), sNextHeader.toStr().c_str());
	}

	u32	getHeaderInCacheAndHeaderFileLength() const
	{
		return sizeof(HeaderInHeaderFileAndCache) + mCustomDataSize;
	}

	u32	getHeaderInACLAndHCLLen() const
	{
		return sizeof(HeaderInACLAndHCL) + mCustomDataSize;
	}

	void setNextHeader(
			const u32	seqno,
			const u32	offset)
	{
		mNextHeader.ulSeqno = seqno;
		mNextHeader.ulOffset = offset;
	}

	void clone(const AosBlobHeader* header);

	int serializeToCacheAndHeaderFileBuff(char* buff);

	int unserializeFromCacheAndHeaderFileBuff(const char* buff);

	int serializeToACLandHCLBuff(const char* buff);

	int unserializeFromACLandHCLBuff(const char* buff);

	int serializeToHeaderBucketFileBuff(char* buff);

	int unserializeFromHeaderBucketFileBuff(const char* buff);

	static bool compareTimestamp(
			AosBlobHeaderPtr l,
			AosBlobHeaderPtr r);

	void dump();

};


#endif /* Aos_BlobSE_BlobHeader_H_ */
