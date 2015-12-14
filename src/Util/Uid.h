////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Uid.h
// Description:
//	This class defines Universal ID, or UID. A UID consists of three parts: 
//		1. Major Component ID (2 bytes)
//		2. Minor Component ID (2 bytes)
//		3. Sequence Number (8 bytes)
// 
//	Major Component ID can be manually assigned. 
//	Minor Component ID is normally the process ID.
//	Sequence Number is dynamically generated sequencially within the process.
//
//  When an application starts, it obtains a unique Major Component ID. This
//  component ID will not change until the application exits. The next
//  time it runs, it may get another Major Component ID.
//
//  If we can guarantee the uniqueness of Major Component ID, it is 
//  very easy to obtain a unique ID, called Universal (universal to a
//  network) ID. This is the purpose of this class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_UID_h
#define Omn_Util_UId_h

#include "Util/BasicTypes.h"
#include "Util/String.h"



class OmnUID
{
private:
	uint			mCompId;

public:
	/*
	OmnUid();
	OmnUid(const unsigned char *id);
	OmnUid(const int compId, 
		   const int major,
		   const int minor);
	OmnUid(const OmnUid &rhs);
	~OmnUid() {}

	void getCompId(const uint compId) const {return mCompId;}
	void getMajor(const uint major) const {return mMajor;}
	void getMinor(const uint minor) const {return mMinor;}

	void setCompId(const uint compId) {mCompId = compId;}
	void setMajor(const uint major) {mMajor = major;}
	void setMinor(const uint minor) {mMinor = minor;}

	bool operator == (const OmnUid &rhs)
	{
		return (mCompId == rhs.mCompId && 
				mMajor == rhs.mMajor &&
				mMinor == rhs.mMinor);
	}

	bool operator != (const OmnUniId &rhs)
	{
		return (mCompId != rhs.mCompId ||
				mMajor != rhs.mMajor ||
				mMinor != rhs.mMinor);
	}

	OmnUid & operator = (const OmnUid &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		mCompId = rhs.mComId;
		mMajor = rhs.mMajor;
		mMinor = rhs.mMinor;
		return *this;
	}

	OmnString		toString() const;
	const unsigned char	* const getBuf() const {return mId;}
	static int		getIdLength() {return eIdLength;}

  */
};
#endif
