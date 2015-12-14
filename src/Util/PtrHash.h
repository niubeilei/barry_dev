////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PtrHash.h
// Description:
//	This table is designed to hash raw pointers. Raw pointers have good
//  randomness on the lower two bytes. This hash table will use a 16-bit
//  long array (that is, 65535 element array). Each element in the
//  array is a linked list.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_PtrHash_h
#define Omn_Util_PtrHash_h

#include "Porting/Mutex.h"
#include "Util/OmnNew.h"

// #include <stdio.h>


#ifndef OmnPtrHashIndex
#define OmnPtrHashIndex(x) (unsigned long)( ((unsigned long)x) & (unsigned long)eArraySize)
#endif

class OmnPtrHash
{
private:
	struct Elem
	{
		Elem *	mNext;
		void *	mPtr;
	};

	enum 
	{
		eArraySize = 0xffff,
		eMaxIdleElems = 10000
	};

	Elem *			mPtrs[eArraySize+1];
	Elem *			mIdleHead;
	OmnMutexType	mLock;	

	// 
	// These are statistics
	//
	int				mNumPtrsAdded;
	int				mNumPtrsDeleted;
	int				mNumIdleElems;
	int				mNumElemsCreated;
	int				mNumElemsDeleted;
	int				mNumErrorAdd;
	int				mNumFailedAllocation;
	int				mNumFailedQuery;
	int				mNumProgErrors;

public:
	OmnPtrHash()
		:
	mIdleHead(0),
	mNumPtrsAdded(0),
	mNumPtrsDeleted(0),
	mNumIdleElems(0),
	mNumElemsCreated(0),
	mNumElemsDeleted(0),
	mNumErrorAdd(0),
	mNumFailedAllocation(0),
	mNumFailedQuery(0),
	mNumProgErrors(0)
	{
		OmnInitMutex(mLock);

		OmnMutexLock(mLock);
		for (int i=0; i<=eArraySize; i++)
		{
			mPtrs[i] = 0;
		}
		OmnMutexUnlock(mLock);
	}

	~OmnPtrHash()
	{
		OmnMutexLock(mLock);
		for (int i=0; i<=eArraySize; i++)
		{
			Elem *head = mPtrs[i];
			while (head)
			{
				Elem *next = head->mNext;
				delete head;
				mNumElemsDeleted++;
				head = next;
			}
		}

		while (mIdleHead)
		{
			Elem *next = mIdleHead->mNext;
			delete mIdleHead;
			mNumElemsDeleted++;
			mIdleHead = next;
		}

		mIdleHead = 0;
		mNumIdleElems = 0;

		OmnMutexUnlock(mLock);
	}

	int	getNumPtrsAdded() const {return mNumPtrsAdded;}
	int	getNumPtrsDeleted() const {return mNumPtrsDeleted;}
	int	getNumIdleElems() const {return mNumIdleElems;}
	int	getNumElemsCreated() const {return mNumElemsCreated;}
	int	getNumElemsDeleted() const {return mNumElemsDeleted;}
	int	getNumErrorAdd() const {return mNumErrorAdd;}
	int	getNumFailedAllocation() const {return mNumFailedAllocation;}
	int	getNumFailedQuery() const {return mNumFailedQuery;}

	bool addPtr(const void *ptr)
	{
		// 
		// It checks whether ptr is already in the table. If yes, it is an error, but
		// it does not report the error and returns true. Otherwise, it allocates an 
		// Elem and insert it into the table. If it failed to allocate Elem, it is a
		// serious problem. Currently, it simply returns false.
		//
		OmnMutexLock(mLock);
		unsigned long index = OmnPtrHashIndex(ptr);
		Elem *head = mPtrs[index];
		while (head)
		{
			if (head->mPtr == ptr)
			{
				// 
				// The pointer is already in the list. This is an error. But we will
				// return true anyway.
				//
				mNumErrorAdd++;
				OmnMutexUnlock(mLock);
				return true;
			}
			head = head->mNext;
		}

		// 
		// We searched the list. It is not in the list. Time to add the ptr into 
		// the list.
		// 
		Elem *elem = getElem();
		if (!elem)
		{
			// 
			// This is a serious problem. Most likely, the system runs out of memory.
			// 
			OmnMutexUnlock(mLock);
			return false;
		}

		mNumPtrsAdded++;
		elem->mPtr = (void *)ptr;
		elem->mNext = mPtrs[index];
		mPtrs[index] = elem;
		OmnMutexUnlock(mLock);
		return true;
	}

	bool remove(const void *ptr)
	{
		return inTable(ptr, true);
	}

	bool inTable(const void *ptr, const bool deleteFlag = false)
	{
		OmnMutexLock(mLock);
		unsigned int index = OmnPtrHashIndex(ptr);
		Elem *head = mPtrs[index];
		Elem *prev = 0;
		while (head)
		{
			if (head->mPtr == ptr)
			{
				// 
				// The pointer is in the list. 
				//
				if (deleteFlag)
				{
					mNumPtrsDeleted++;

					// 
					// Need to remove the element.
					//
					if (mPtrs[index] == head)
					{
						mPtrs[index] = head->mNext;
					}
					else 
					{
						if (prev)
						{
							prev->mNext = head->mNext;
						}
						else
						{
							// 
							// This is an error since if prev == 0, 
							// mPtrs[index] must be head. 
							//
							mNumProgErrors++;
						}
					}

					if (mNumIdleElems >= eMaxIdleElems)
					{
						// 
						// Need to delete the element
						//
						delete head;
						mNumElemsDeleted++;
						head = 0;
					}
					else
					{
						// 
						// Need to insert the element into the idle list
						//
						head->mNext = mIdleHead;
						head->mPtr = 0;
						mIdleHead = head;
						mNumIdleElems++;
					}
				}

				OmnMutexUnlock(mLock);
				return true;
			}

			prev = head;
			head = head->mNext;
		}

		// 
		// Not found
		//
		mNumFailedQuery++;
		OmnMutexUnlock(mLock);
		return false;
	}

private:
	Elem * getElem()
	{
		// 
		// IMPORTANT: when calling this function, the caller should have already
		// locked the mutex. 
		//
		if (mIdleHead)
		{
			Elem *elem = mIdleHead;
			mIdleHead = mIdleHead->mNext;
			elem->mNext = 0;
			elem->mPtr = 0;
			mNumIdleElems--;
			return elem;
		}

		// 
		// Need to allocate it.
		//
		Elem *elem = OmnNew Elem;
		if (!elem)
		{
			// 
			// This is a serious problem. 
			//
			mNumFailedAllocation++;
			return 0;
		}

		mNumElemsCreated++;
		elem->mNext = 0;
		elem->mPtr = 0;
		return elem;
	}
		
};
#endif

