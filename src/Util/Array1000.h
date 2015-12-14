////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array1000.h
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
// Modification History:
//		Added index checking 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_Array1000_h
#define Snt_Util_Array1000_h

#include "Alarm/Alarm.h"
#include "Debug/ErrId.h"

template <class C>
class OmnArray1000
{
private:
	enum
	{
		eArraySize = 1000
	};

	C		mArray[eArraySize];
	int		mCrtSize;
	C		mNullValue;

public:
	OmnArray1000()
		:
	mCrtSize(0)
	{
	}

	OmnArray1000(const OmnArray1000 &rhs)
	:
	mCrtSize(rhs.mCrtSize)
	{
		for (int i=0; i<eArraySize; i++)
		{
			mArray[i] = rhs.mArray[i];
		}
	}

	OmnArray1000 & operator = (const OmnArray1000 &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		mCrtSize = rhs.mCrtSize;
		for (int i=0; i<eArraySize; i++)
		{
			mArray[i] = rhs.mArray[i];
		}

		return *this;
	}


	~OmnArray1000()
	{
	}

	void	setNullValue(const C c) {mNullValue = c;}
	int		getCapacity() const {return eArraySize;}
	int		entries() const {return mCrtSize;}
	bool	hasMoreSpace() const {return mCrtSize < eArraySize;}

	//
	// Note: this function will return mArray[0] if it goes out of bound
	//
	C &		operator [] (const int index) 
			{
				// 
				// Chen Ding, 05/21/2003, Change-0001
				//
				// if (index < mCrtSize) 
				if (index >= 0 && index < mCrtSize) 
				{
					return mArray[index];
				}

				OmnAlarm << "OmnArray1000 out of bound: " << index 
					<< ":" << mCrtSize << enderr;
				return mArray[0];
			}

    const C&  operator [] (const int index) const
            {
				// 
				// Chen Ding, 05/21/2003, Change-0003
				//
                // if (index < mCrtSize) 
                if (index >= 0 && index < mCrtSize) 
				{
					return mArray[index];
				}

                OmnAlarm << "OmnArray1000 out of bound: " << index 
					<< ":" << mCrtSize << enderr;
                return mArray[0];
            }

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
				OmnAlarm << "OmnArray1000 out of bound in inserting" << enderr;
				return false;
			}

	bool 	remove(const int index)
			{
				// 
				// Chen Ding, 05/21/2003, Change-0001
				//
				// if (index >= mCrtSize)
				if (index < 0 || index >= mCrtSize)
				{
					OmnAlarm << "OmnArray1000 remove: out of bound: " 
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

