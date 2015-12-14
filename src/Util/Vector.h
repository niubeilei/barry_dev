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
// 2015/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_vector_h
#define Aos_Util_vector_h
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

template <class T>
class aos_vector
{
private:
	enum
	{
		eInitCapacity = 100,
		eSize1 = 5000,
		eIncSize = 1000
	};

	u32 	mSize;
	u32		mCapacity;
	T *		mData;	

public:
	aos_vector() 
	:
	mSize(0),
	mCapacity(0),
	mData(0)
	{
	}

	~aos_vector()
	{
		OmnDelete [] mData;
	}

	inline u32 size() const {return mSize;}
	inline T &operator [](const int idx)
	{
		aos_assert_r(idx >= 0 && idx < mSize, mData[0]);
		return mData[idx];
	}

	inline const T &operator [](const int idx) const
	{
		aos_assert_r(idx >= 0 && idx < mSize, mData[0]);
		return mData[idx];
	}

	inline bool push_back(const T &v)
	{
		if (mSize >= mCapacity)
		{
			expand_memory();
		}
		aos_assert_r(mSize < mCapacity, false);
		mData[mSize++] = v;
		return true;
	}

private:
	bool expand_memory()
	{
		u32 new_capacity = mCapacity;
		if (new_capacity == 0)
		{
			new_capacity = eInitCapacity;
		}
		else if (new_capacity >= eSize1)
		{
			new_capacity += eIncSize;
		}
		else
		{
			new_capacity += new_capacity;
		}
		T *data = OmnNew T[new_capacity];
		if (mData)
		{
			memcpy(data, mData, mSize*sizeof(T));
			OmnDelete [] mData;
		}
		mData = data;
		mCapacity = new_capacity;
		return true;
	}
};
#endif

