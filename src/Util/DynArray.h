////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DynArray.h
// Description:
//	This is a dynamic array. 
// 
//  Since most arrays are used to store smart pointers. When 
//  deleting an elemnet from such an array, it is important to
//  delete the smart pointer, too. To help doing this, this 
//  array has a mDefaultValue. If it is a smart pointer array,
//  the user of this array do not bother at all. It is handled
//  properly.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_DynArray_h
#define Snt_Util_DynArray_h

#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Debug/ErrId.h"
#include "Util/OmnNew.h"

template <class C, int initsize, int incsize, int maxsize>
class OmnDynArray
{
private:
	C	   *mArray;
	u32		mCrtSize;		// This will grow/thrink as needed
	u32		mArraySize;		// This is the maxium size of the array
	C		mDefaultValue;
	u32		mMaxSize;
	int		mInitSize;
	int 	mIncSize;
	bool	mDeleted;

public:
	OmnDynArray()
		:
	mArray(0),
	mCrtSize(0),
	mArraySize(initsize),
	mMaxSize(maxsize),
	mInitSize(initsize),
	mIncSize(incsize),
	mDeleted(false)
	{
	}

	~OmnDynArray()
	{
		OmnDelete [] mArray;
		mArray = 0;
		mDeleted = true;
	}

	void	setMaxSize(const u32 size) {mMaxSize = size;}
	void	setNullValue(const C c) {mDefaultValue = c;}
	int		getCapacity() const {return mArraySize;}
	int		entries() const {return mCrtSize;}
	bool	hasMoreSpace() const {return mCrtSize < mArraySize;}
	bool	reachedCapacity() const {return mCrtSize >= mMaxSize;}

	// 
	// Chen Ding, 02/21/2007, Testing purpose only
	//
	void *	getArrayPointer() const {return mArray;}

	//
	// Note: this function will return mDefaultValue if it goes out of bound
	//
	C &		operator [] (const u32 index) 
			{
				if (index < mCrtSize) return mArray[index];

				if (changeArraySize(index+1))
				{
					mCrtSize = index+1;
					return mArray[index];
				}
				OmnAlarm << "OmnDynArray failed to allocate memory!" << enderr;
				return mDefaultValue;
			}

	bool	get(const u32 index, C &c) const
			{
				if (!mArray) return mDefaultValue;
				if (index >= 0 && index < mCrtSize) 
				{
					c = mArray[index];
					return true;
				}

				OmnAlarm << "OmnDynArray out of bound: " << index 
					<< ":" << mCrtSize << enderr;
				return false;
			}

	//
	// Note: this function will return mDefaultValue if it goes out of bound
	//
    const C&  operator [] (const u32 index) const
            {
				if (!mArray) return mDefaultValue;
                if (index < mCrtSize) 
				{
					return mArray[index];
				}

                OmnAlarm << "OmnDynArray out of bound: " << index 
					<< ":" << mCrtSize << enderr;
                return mDefaultValue;
            }

	OmnDynArray &append(const OmnDynArray &ar)
			{
				for (int i=0; i<ar.entries(); i++)
					append(ar[i]);
				return *this;
			}

	int		append(const C &c) 
			{
				// Append the object into the end of the array and
				// return the index. If failed, return -1.
				if (mArray && mCrtSize < mArraySize)
				{
					mArray[mCrtSize] = c;
					return mCrtSize++;
				}

				// it is out of bound. Grow the array.
				if (!changeArraySize(mArraySize + mIncSize))
				{
					// 
					// Failed to grow the array.
					//
					OmnAlarm << "Failed to grow dynamic array: "
						<< mArraySize << enderr;
					return -1;
				}

				mArray[mCrtSize] = c;
				return mCrtSize++;
			}

	bool 	remove(const u32 index, const bool shift = false)
			{
				// 
				// Removes the object at position: 'index'. If not a valid
				// index, return false. Otherwise the object is removed.
				// If the removed is not the last element, or not the 
				// element in the last position, it checks whether 'shift'
				// is true. If yes, the array is shifted. Otherwise, 
				// the position is filled
				// with the element in the last position
				//
				if (index >= mCrtSize)
				{
					OmnProgAlarm << "OmnDynArray remove: out of bound: " 
						<< index << enderr;
					return false;
				}

				if (!mArray) return false;

				if (mCrtSize <= 1)
				{
					//
					// The deleted is the first element
					//
					mArray[0] = mDefaultValue;
					mCrtSize = 0;
					return true;
				}

				if (index == mCrtSize-1)
				{
					// 
					// it is the last element
					//
					mArray[index] = mDefaultValue;
					mCrtSize--;
					return true;
				}

				//
				// Otherwise, the array is not empty now and the deleted
				// element is not the last one. 
				//
				if (shift)
				{
					// 
					// Need to shift the array. 
					//
					for (u32 i=index; i<mCrtSize-1; i++)
					{
						mArray[i] = mArray[i+1];
					}
				}
				else
				{
					// 
					// Just swap it with the last one
					//
					mArray[index] = mArray[mCrtSize-1];
				}

				mArray[mCrtSize-1] = mDefaultValue;
				mCrtSize--;
				return true;
			}

	// 
	// Frank Chen, 2007-05-18, Patch this remove function
	//
	bool 	remove(const C &c, const bool shift = false)
			{
				// 
				// Removes the object: 'c'. If not a valid
				// c, return false. Otherwise the object is removed.
				// If the removed is not the last element, or not the 
				// element in the last position, it checks whether 'shift'
				// is true. If yes, the array is shifted. Otherwise, 
				// the position is filled
				// with the element in the last position
				//
				if (!mArray) return false;
				for(u32 i = 0; i < mCrtSize; i ++)
				{
					if(mArray[i] == c)
					{
						return remove(i, shift);
					}
				}
				return false;
			}

	void	clear()
			{
				if (!mArray) return;
				for (u32 i=0; i<mCrtSize; i++)
				{
					mArray[i] = mDefaultValue;
				}

				mCrtSize = 0;
			}	

	bool	changeArraySize(const u32 newSize)
			{
//if (newSize > 5000)
//{
//cout << __FILE__ << ":" << __LINE__ << " found it: " << newSize << endl;
//}
				if (mArray && newSize <= mArraySize) return true;

				// If it is too big, don't do it.
				if (newSize > mMaxSize)
				{
					OmnAlarm << "Array too big: " << newSize << enderr;
					return false;
				}

				C *newArray = OmnNew C[newSize];
				if (!newArray)
				{
					OmnAlarm << "Failed to allocate mmeory: " 
						<< newSize << enderr;
					return false;
				}

				if (mArray)
				{
					for (u32 i=0; i<mCrtSize; i++)
					{
						newArray[i] = mArray[i];
					}
				}
				mArraySize = newSize;
				OmnDelete [] mArray;
				mArray = newArray;
				return true;
			}
};

#endif

