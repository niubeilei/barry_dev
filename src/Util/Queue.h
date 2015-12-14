////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Queue.h
// Description:
//	It implements a queue.  The queue is designed to be very efficient.
//	The queue can grow in size as needed, but limit to the maximum 
//	size assigned.
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_Queue_h
#define Snt_Util_Queue_h

#include "Debug/ErrId.h"
#include "Util/OmnNew.h"


template <class C, int initsize, int incsize, int maxsize>
class OmnQueue
{
private:
	int		mQueueSize;
	int		mMaxSize;
	C	*	mQueue;
	int		mHead;
	int		mTail;
	int		mNumElements;
	C		mNullValue;
	int		mIndex;
	int		mLoopCount;
	int		mIncsize;

	OmnQueue & operator = (const OmnQueue &rhs);
	OmnQueue(const OmnQueue &rhs);

public:
	OmnQueue()
		:
	mQueueSize(initsize),
	mMaxSize(maxsize),
	mQueue(0),
	mHead(0),
	mTail(0),
	mNumElements(0),
	mIndex(0),
	mLoopCount(0),
	mIncsize(incsize)
	{
		if (mQueueSize <= 0) mQueueSize = 20;
		mQueue = OmnNew C[initsize];
	}


	~OmnQueue()
	{
		if (mQueue)
		{
			OmnDelete [] mQueue;
			mQueue = 0;
		}
	}

	void	setNullValue(const C c) {mNullValue = c;}
	int		getCapacity() const {return mQueueSize;}
	int		getMaxsize() const {return mMaxSize;}
	int		entries() const {return mNumElements;}
	bool	hasMoreSpace() const {return mNumElements < mMaxSize;}

	void	loop() 
			{
				mIndex = mHead;
				mLoopCount = 0;
			}

	const C & crt() const
			{
				// 
				// Chen Ding, 08/09/2003, 2003-0106
				// 
				if (mIndex < 0 || mIndex >= mQueueSize)
				{
					return mNullValue;
				}

				return mQueue[mIndex];
			}

	C &		crt() 
			{
				// 
				// Chen Ding, 08/09/2003, 2003-0106
				// 
				if (mIndex < 0 || mIndex >= mQueueSize)
				{
					return mNullValue;
				}

				return mQueue[mIndex];
			}

	C &		next() 
			{
				mIndex--;
				if (mIndex < 0)
				{
					mIndex = mQueueSize-1;
				}
				mLoopCount++;
				if (mLoopCount > mNumElements)
				{
					return mNullValue;
				}

				return mQueue[mIndex];
			}

	int		crtLoopIndex() const {return mIndex;}
	bool	hasMore() const
			{
				return mLoopCount < mNumElements;
			}

    const C&  operator [] (const int index) const
            {
				// 
				// One should not use this function to access queue
				// elements. 
				//
                if (index < 0 || index >= mQueueSize) 
				{
                	return mNullValue;
				}

				return mQueue[index];
            }

	bool	push(const C &c) 
			{
				if (mNumElements >= mQueueSize)
				{
					// Need to grow the queue
					if (mNumElements >= mMaxSize)
					{
						return false;
					}

					// Need to grow the memory
					C *mem = OmnNew C[mNumElements + mIncsize];
					if (!mem)
					{
						return false;
					}

					memcpy(mem, mQueue, sizeof(C) * mNumElements);
					OmnDelete [] mQueue;
					mQueue = mem;

					// Need to adjust the memory. Move elements starting at 
					// 'mTail' mIncsize number of elements. 
					memmove(&mQueue[mTail+mIncsize], &mQueue[mTail], 
						sizeof(C) * (mQueueSize - mTail));
					mQueueSize = mNumElements + mIncsize;
					mTail += mIncsize;
				}

				mNumElements++;
				mQueue[mHead++] = c;
				if (mHead >= mQueueSize)
				{
					mHead = 0;
				}
				return true;
			}

	C 		pop()
			{
				if (mNumElements <= 0)
				{
					return false;
				}

				C value = mQueue[mTail];
				mQueue[mTail++] = mNullValue;
				mNumElements--;
				if (mTail >= mQueueSize)
				{
					mTail = 0;
				}

				return value;
			}

	void	clear()
			{
				for (int i=0; i<mQueueSize; i++)
				{
					mQueue[i] = mNullValue;
				}

				mHead = 0;
				mTail = 0;
				mNumElements = 0;
			}	

	void	truncate(const int mark)
			{
				// 
				// truncate means to disgard all elements that are behind
				// 'mark'. 
				//
				// Ray Chen, 09/02/2003, 2003-0242
				// Also include the 'mark' itself.
				//
				while (mTail != mark)
				{
					mQueue[mTail] = mNullValue;
					mTail++;
					mNumElements--;
					if (mTail >= mQueueSize)
					{
						mTail = 0;
					}
				}

				//
				// Ray Chen, 09/02/2003, 2003-0242
				// Now, mTail == mark. Need to remove this one, too.
				//
				mQueue[mTail] = mNullValue;
				mTail++;
				mNumElements--;
				if (mTail >= mQueueSize)
				{
					mTail = 0;
				}
				// End of 2003-0242
			}

	int		getHead() const {return mHead;}
	int		getTail() const {return mTail;}
};

#endif

