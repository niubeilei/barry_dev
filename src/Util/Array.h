////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array.h
// Description:
//	This is a fixed size array. It never grows. It is designed
//  for efficiency. 
// 
//  Since most arrays are used to store smart pointers. When 
//  deleting an elemnet from such an array, it is important to
//  delete the smart pointer, too. To help doing this, this 
//  array has a mNullValue. If it is a smart pointer array,
//  the user of this array do not bother at all. It is handled
//  properly.
//
//  This array differs from the conventional array in the following
//  ways:
//	1. Index checking:
//	   If it is out of range, we will raise alarms, but program
//	   never dumps.
//	2. Deleting elements:
//	   When deleting an element, it will swap the last element with
//	   the one being deleted so that it avoids shifting the remaining
//	   array. This is for efficiency considerations.   
//
// Modification History:
//  	Added index checking 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_Array_h
#define Snt_Util_Array_h

#include "Alarm/Alarm.h"
#include "Debug/ErrId.h"

template <class C, int eArraySize>
class OmnArray
{
private:
	C		mArray[eArraySize];
	int		mCrtSize;		// This will grow/thrink as needed
	int		mArraySize;		// This is the maxium size of the array
	C		mNullValue;

public:
	OmnArray()
		:
	mCrtSize(0)
	{
	}

	OmnArray(const OmnArray &rhs)
	:
	mCrtSize(rhs.mCrtSize)
	{
		for (int i=0; i<eArraySize; i++)
		{
			mArray[i] = rhs.mArray[i];
		}
	}

	OmnArray & operator = (const OmnArray &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		mCrtSize = rhs.mCrtSize;
		for (int i=0; i<mCrtSize; i++)
		{
			mArray[i] = rhs.mArray[i];
		}
		mNullValue = rhs.mNullValue;

		return *this;
	}


	~OmnArray()
	{
	}

	void	setNullValue(const C c) {mNullValue = c;}
	int		getCapacity() const {return eArraySize;}
	int		entries() const {return mCrtSize;}
	bool	hasMoreSpace() const {return mCrtSize < eArraySize;}

	//
	// Note: this function will return mNullValue if it goes out of bound
	//
	C &		operator [] (const int index) 
			{
				if (index >= 0 && index < mCrtSize) 
				{
					return mArray[index];
				}

				OmnAlarm << "OmnArray out of bound: " << index 
					<< ":" << mCrtSize << enderr;
				return mNullValue;
			}

	//
	// Note: this function will return mNullValue if it goes out of bound
	//
    const C&  operator [] (const int index) const
            {
                if (index >= 0 && index < mCrtSize) 
				{
					return mArray[index];
				}

                OmnAlarm << "OmnArray out of bound: " << index 
					<< ":" << mCrtSize << enderr;
                return mNullValue;
            }

	// 
	// Note: if the array is already full, it will return false 
	// and the element is not added.
	//
	bool	append(C c) 
			{
				if (mCrtSize < eArraySize)
				{
					mArray[mCrtSize++] = c;
					return true;
				}

				//
				// it is out of bound
				//
				OmnAlarm << "OmnArray out of bound in inserting" << enderr;
				return false;
			}

	bool 	remove(const int index)
			{
				if (index < 0 || index >= mCrtSize)
				{
					OmnAlarm << "OmnArray remove: out of bound: " 
						<< index << enderr;
					return false;
				}

				if (mCrtSize <= 1)
				{
					//
					// The deleted is the first element
					//
					mArray[0] = mNullValue;
					mCrtSize = 0;
					return true;
				}

				if (index == mCrtSize-1)
				{
					// 
					// it is the last element
					//
					mArray[index] = mNullValue;
					mCrtSize--;
					return true;
				}

				//
				// Otherwise, swap it with the last element
				//
				mArray[index] = mArray[mCrtSize-1];
				mArray[mCrtSize-1] = mNullValue;
				mCrtSize--;
				return true;
			}

	void	clear()
			{
				for (int i=0; i<mCrtSize; i++)
				{
					mArray[i] = mNullValue;
				}

				mCrtSize = 0;
			}	
};

#endif

