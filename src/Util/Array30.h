////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array30.h
// Description:
//	This is a fixed size array. It never grows. It is designed
//  for efficiency.    
//
// Modification History:
// 		Added index range checking 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_Array30_h
#define Snt_Util_Array30_h

#include "Alarm/Alarm.h"
#include "Debug/ErrId.h"
#include "Util/Obj.h"

template <class C>
class OmnArray30  OmnDeriveFromObj
{
private:
	enum
	{
		eArraySize = 30
	};

	C		mArray[eArraySize];
	int		mCrtSize;
	C		mNullValue;

public:
	OmnArray30()
		:
	mCrtSize(0)
	{
	}

	OmnArray30(const OmnArray30 &rhs)
	:
	mCrtSize(rhs.mCrtSize)
	{
		for (int i=0; i<eArraySize; i++)
		{
			mArray[i] = rhs.mArray[i];
		}
	}

	OmnArray30 & operator = (const OmnArray30 &rhs)
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


	~OmnArray30()
	{
	}

	void	setNullValue(const C c) {mNullValue = c;}
	int		getCapacity() const {return eArraySize;}
	int		size() const {return mCrtSize;}
	//void	reset();
	bool	hasMoreSpace() const {return mCrtSize < eArraySize;}

	//
	// Note: this function will return mArray[0] if it goes out of bound
	//
	C &		operator [] (const int index) 
			{
				// 
				// Chen Ding, 05/21/2003, Change-0003
				//
				if (index >= 0 && index < mCrtSize) return mArray[index];

				OmnProgAlarm << OmnErrId::eAlarmArrayOutOfBound
					<< "OmnArray30 out of bound: " << index << enderr;
				return mArray[0];
			}

    const C&  operator [] (const int index) const
            {
				;
				// 
				// Chen Ding, 05/21/2003, Change-0003
				//
                // if (index < mCrtSize) return mArray[index];
                if (index >= 0 && index < mCrtSize) return mArray[index];

                OmnProgAlarm << OmnErrId::eAlarmProgError
                    << "OmnArray30 out of bound: " << index << enderr;
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
				OmnProgFatal << OmnErrId::eFatalProgramError
					<< "OmnArray30 out of bound in inserting" << enderr;
				return false;
			}

	bool 	remove(const int index)
			{
				// 
				// Chen Ding, 05/21/2003
				//
				// if (index >= mCrtSize)
				if (index < 0 || index >= mCrtSize)
				{
					OmnProgAlarm << OmnErrId::eAlarmProgError
						<< "OmnArray30 remove: out of bound: " 
						<< index << enderr;
					return false;
				}

				if (mCrtSize <= 1)
				{
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

