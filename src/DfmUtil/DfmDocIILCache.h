////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmUtil_DfmDocIILCache_h
#define AOS_DfmUtil_DfmDocIILCache_h

#include "DfmUtil/DfmDoc.h"


class AosDfmDocIILCache : public AosDfmDoc 
{
public:
	enum
	{
		eHeaderSize = 32 
	};

	enum Type
	{
		eInvalidType,

		eIILDocids,
		eQueriedData,

		eMaxType
	};

private:
	u64			mIILID;
	u64			mTimestamp;
	Type		mType;

public:
	AosDfmDocIILCache(const u64 &docid, const Type type);
	~AosDfmDocIILCache();
	
	// AosDfmDoc Interface.
	virtual AosDfmDocPtr clone();
	virtual u32 getHeaderSize(){ return eHeaderSize; };

	virtual bool serializeFrom(const AosBuffPtr &header_buff);
	virtual bool serializeTo(const AosBuffPtr &header_buff);
	virtual bool getBodySeqOffFromBuff(
				const AosBuffPtr &header_buff,
				u32 &body_seq,
				u64 &body_off);
	virtual bool setBodySeqOffToBuff(
				const AosBuffPtr &header_buff,
				const u32 body_seq,
				const u64 body_off);
	// AosDfmDoc Interface End.

	
	void setIILID(const u64 &iilid) {mIILID = iilid;}
	void setTimestamp(const u64 &t) {mTimestamp = t;}
	u64 getIILID() const {return mIILID;}
	Type getType() const {return mType;}
	u64 getTimestamp() const {return mTimestamp;}

};

#endif
