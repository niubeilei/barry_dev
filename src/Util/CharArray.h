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
// 11/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_CharArray_h
#define Snt_Util_CharArray_h

#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "Alarm/Alarm.h"
#include "Porting/LongTypes.h"
#include "Util/BasicTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#define AOSCHARARRAY_SANITYCHECK(x) x->sanityCheck()

class AosCharArray : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum 
	{
		eExtraBytes = 20,
		eByte1 = 0x26,
		eByte2 = 0x64,
		eByte3 = 0xa7,
		eByte4 = 0xc9

	};

	char *		mData;
	int			mDataLen;

public:
	AosCharArray(const int len)
		:
	mData(0),
	mDataLen(len)
	{
		allocateMemory(len);
	}

	bool allocateMemory(int len)
	{
		if (len < 0)
		{
			OmnAlarm << "Length is negative: " << len  << enderr;
			len = 0;
		}

		if (len == 0)
		{
			if (mData) deleteMemory();
			mData = 0;
			mDataLen = 0;
		}
		else
		{
			char *data = 0;
			if (len > mDataLen)
			{
				if (mData) deleteMemory();
				char *data = new char[mDataLen+eExtraBytes];
				if (!data)
				{
					OmnAlarm << "Failed allocating the memory: " << mDataLen << enderr;
					mData = 0;
					mDataLen = 0;
					return false;
				}
			}
			else
			{
				data = &mData[-4];
			}

			// Set the poisons
			data[0] = eByte1;
			data[1] = eByte2;
			data[2] = eByte3;
			data[3] = eByte4;
			data[4 + mDataLen + 1] = eByte1;
			data[4 + mDataLen + 2] = eByte2;
			data[4 + mDataLen + 3] = eByte3;
			data[4 + mDataLen + 4] = eByte4;
			data[4 + mDataLen + 5] = 0;
			mData = &data[4];
		}
	}

	AosCharArray(const AosCharArray &rhs);
	~AosCharArray();

	bool deleteMemory()
	{
		if (!mData) return true;
		char *data = &mData[-4];
		delete data;
		mData = 0;
		return true;
	}

	char operator [](const int idx) const 
	{
		AOSCHARARRAY_SANITYCHECK(this);
		aos_assert_r(idx >= 0 && idx < mDataLen, 0);
		aos_assert_r(mData, 0);
		return mData[idx];
	}

	char & operator [](const int idx)
	{
		static char lsChar = 0;

		AOSCHARARRAY_SANITYCHECK(this);
		aos_assert_r(mData, lsChar);
		aos_assert_r(idx >= 0 && idx < mDataLen, lsChar);
		return mData[idx];
	}

	int length() const 
	{
		if (!mData) return 0;
		return strlen(mData);
	}

	bool sanityCheck() const
	{
		if (!mData) return true;
		const char * const data = mData;
		aos_assert_r(data[0] == eByte1 && 
				data[1] == eByte2 &&
				data[2] == eByte3 &&
				data[3] == eByte4 &&
				data[4 + mDataLen + 1] == eByte1 &&
				data[4 + mDataLen + 2] == eByte2 &&
				data[4 + mDataLen + 3] == eByte3 &&
				data[4 + mDataLen + 4] == eByte4 &&
				data[4 + mDataLen + 5] == 0, false);
		return true;
	}

	char * getBuff() {return mData;}
};
#endif

