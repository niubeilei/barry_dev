////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjId.h
// Description:
//	An object ID consists of 8 bytes:
//		Byte 7 (highest):	Major
//		Byte 6:				Minor
//		Byte 5-0:			Sequence Number
//
//	First eStart sequence numbers are reserved for special purposes.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_ObjId_h
#define Omn_Util_ObjId_h

#include "Util/String.h"



class OmnObjId
{
private:
	enum
	{
		eStart = 1000,
		eInvalidObjId = 0
	};

	unsigned int	mHigh;
	unsigned int	mLow;

public:
	OmnObjId(const unsigned int major, 
			 const unsigned int minor)
			 :
	mHigh(0),
	mLow(eStart)
	{
		mHigh = (major << 24) + ((minor & 0xff) << 16);
	}

	OmnObjId(const OmnObjId &rhs)
		:
	mHigh(rhs.mHigh),
	mLow(rhs.mLow)
	{
	}

	OmnObjId()
		:
	mHigh(0),
	mLow(eStart)
	{
	}

	~OmnObjId() {}

	OmnObjId operator ++();
	OmnObjId & operator = (const OmnObjId &rhs)
	{
		mHigh = rhs.mHigh;
		mLow = rhs.mLow;
		return *this;
	}

	bool operator == (const OmnObjId &rhs) const
	{
		return (mHigh == rhs.mHigh && mLow == rhs.mLow);
	}

	OmnObjId operator + (const int rhs);

	void	reset() {mHigh &= 0xffff0000; mLow = eStart;}
	static OmnObjId	getInvalidObjId() {return OmnObjId(0, eInvalidObjId);}


	unsigned int	getHashKey() const
	{
		// 
		// It packs the 8 bytes into 2 bytes.
		//
		return ((mLow & 0xffff) + 
				((mLow >> 16) & 0xffff) +
				(mHigh & 0xffff) +
				((mHigh >> 16) & 0xffff));
	}

	OmnString toString() const
	{
		OmnString str;
		str << mHigh << ":" << mLow;
		return str;
	}
};
#endif

