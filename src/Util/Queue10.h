////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Queue10.h
// Description:
//	This is a fixed size queue (10 elements). It never checks
//  for overflow. It adds elements to the queue's tail. If 
//  there are too many elements, it overrides.
//  	
//	The queue is designed to be very efficient   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_Queue10_h
#define Snt_Util_Queue10_h

#include "Alarm/Alarm.h"
#include "Debug/ErrId.h"
#include "Util/Obj.h"

template <class C>
class OmnQueue10 OmnDeriveFromObj
{
private:
	enum
	{
		eQueueSize = 10
	};

	C		mQueue[eQueueSize];
	int		mHead;
	int		mTail;
	int		mNumElements;
	C		mNullValue;

public:
	OmnQueue10()
		:
	mHead(0),
	mTail(0),
	mNumElements(0)
	{
	}

	OmnQueue10(const OmnQueue10 &rhs)
	:
	mHead(rhs.mHead),
	mTail(rhs.mTail),
	mNumElements(rhs.mNumElements)
	{
		for (int i=0; i<eQueueSize; i++)
		{
			mQueue[i] = rhs.mQueue[i];
		}
	}

	OmnQueue10 & operator = (const OmnQueue10 &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		mHead = rhs.mHead;
		mTail = rhs.mTail;
		mNumElements = rhs.mNumElements;
		for (int i=0; i<eQueueSize; i++)
		{
			mQueue[i] = rhs.mQueue[i];
		}

		return *this;
	}


	~OmnQueue10()
	{
	}

	void	setNullValue(const C c) {mNullValue = c;}
	int		getCapacity() const {return eQueueSize;}
	int		size() const {return mNumElements;}
	bool	hasMoreSpace() const {return mNumElements < eQueueSize;}

	void	reset() 
			{
				for (int i=0; i<eQueueSize; i++) 
				{
					mQueue[i] = mNullValue;
				}
				mCrtIndex = mHead;
			}

	//
	// Note: this function will return mQueue[0] if it goes out of bound
	//
	C &		next() 
			{
				if (index < mCrtSize) return mQueue[index];
				OmnAlarmSerious << OmnErrId::eAlarmProgError
					<< "OmnQueue10 out of bound: " << index << enderr;
				return mQueue[0];
			}

    const C&  operator [] (const int index) const
            {
                if (index < mCrtSize) return mQueue[index];
                OmnAlarmSerious << OmnErrId::eAlarmProgError
                    << "OmnQueue10 out of bound: " << index << enderr;
                return mQueue[0];
            }

	bool	append(C c) 
			{
				if (mCrtSize < eQueueSize)
				{
					mQueue[mCrtSize++] = c;
					return true;
				}

				//
				// it is out of bound
				//
				OmnAlarmFatal << OmnErrId::eFatalProgramError
					<< "OmnQueue10 out of bound in inserting" << enderr;
				return false;
			}

	bool 	remove(const int index)
			{
				if (index >= mCrtSize)
				{
					OmnAlarmSerious << OmnErrId::eAlarmProgError
						<< "OmnQueue10 remove: out of bound: " << index << enderr;
					return false;
				}

				if (mCrtSize <= 1)
				{
					mQueue[0] = mNullValue;
					mCrtSize = 0;
					return true;
				}

				if (index == mCrtSize-1)
				{
					// 
					// it is the last element
					//
					mQueue[index] = mNullValue;
					mCrtSize--;
					return true;
				}

				//
				// Otherwise, swap it with the last element
				//
				mQueue[index] = mQueue[mCrtSize-1];
				mQueue[mCrtSize-1] = mNullValue;
				mCrtSize--;
				return true;
			}

	void	clear()
			{
				for (int i=0; i<mCrtSize; i++)
				{
					mQueue[i] = mNullValue;
				}

				mCrtSize = 0;
			}	
};

#endif

