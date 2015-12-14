////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ulong.h
// Description:
//	We want to use unsigned long, which is supposed to be 8 bytes long.
//  But Microsoft implements the type as 4 bytes long (most Unix is 8
//  bytes long). This class implements unsigned long as 8 bytes.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_OmnUlong_h
#define Omn_Porting_OmnUlong_h

#include "Util/BasicTypes.h"

#ifdef OMN_PLATFORM_UNIX
typedef unsigned long long	OmnUlong;

#elif OMN_PLATFORM_MICROSOFT
class OmnUlong
{
private:
	uint	mUpper;			// Most significant bytes
	uint	mLower;			// Least significant byates

public:
	OmnUlong() 
		:
	mUpper(0),
	mLower(0)
	{
	}

	OmnUlong(const long v)
		:
	mLower(v),
	mUpper(0)
	{
	}

	OmnUlong(const OmnUlong &rhs)
		:
	mUpper(rhs.mUpper),
	mLower(rhs.mLower)
	{
	}

	OmnUlong & operator = (const OmnUlong &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}
		
		mUpper = rhs.mUpper;
		mLower = rhs.mLower;
		return *this;
	}

	bool operator == (const OmnUlong &rhs)
	{
		return (mUpper == rhs.mUpper && mLower == rhs.mLower);
	}

	bool operator > (const OmnUlong &rhs)
	{
		if (mUpper < rhs.mUpper)
		{
			return false;
		}

		if (mUpper > rhs.mUpper)
		{
			return true;
		}

		return mLower > rhs.mLower;
	}

	bool operator >= (const OmnUlong &rhs)
	{
		if (mUpper < rhs.mUpper)
		{
			return false;
		}

		if (mUpper > rhs.mUpper)
		{
			return true;
		}

		return mLower >= rhs.mLower;
	}

	bool operator < (const OmnUlong &rhs)
	{
		if (mUpper > rhs.mUpper)
		{
			return false;
		}

		if (mUpper < rhs.mUpper)
		{
			return true;
		}

		return mLower < rhs.mLower;
	}

	bool operator <= (const OmnUlong &rhs)
	{
		if (mUpper > rhs.mUpper)
		{
			return false;
		}

		if (mUpper < rhs.mUpper)
		{
			return true;
		}

		return mLower <= rhs.mLower;
	}

	bool operator != (const OmnUlong &rhs)
	{
		return (mUpper != rhs.mUpper || mLower != rhs.mLower);
	}

	OmnUlong & operator + (const OmnUlong &rhs)
	{
		if (mLower + rhs.mLower > (unsigned int)0xffffffff)
		{
			mUpper++;
		}
		mLower += rhs.mLower;
		return *this;
	}

	OmnUlong & operator - (const OmnUlong &rhs)
	{
		if (mLower < rhs.mLower)
		{
			mUpper--;
		}

		mLower -= rhs.mLower;
		return *this;
	}

	OmnUlong & operator ++ (int)
	{
		if (mLower == (unsigned int)0xffffffff)
		{
			mLower = 0;
			mUpper++;
		}
		else
		{
			mLower++;
		}
		return *this;
	}

	OmnUlong & operator -- ()
	{
		if (mLower == 0)
		{
			mUpper--;
			mLower = (unsigned int)0xffffffff;
		}
		else
		{
			mLower--;
		}
		return *this;
	}
};

#endif

#endif

