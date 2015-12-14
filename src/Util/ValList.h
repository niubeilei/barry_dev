////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ValList.h
// Description:
//	To improve performance, we assume the following:
//		1. The class C must have a default constructor 
//		2. The class C must have a valid assignment operator, 
//		   which can handle the memory correctly.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_ValList_h
#define Omn_Util_ValList_h

#include <iostream>
#include "Debug/Except.h"
#include "Util/OmnNew.h"


template <class C>
class OmnVList
{
private:
	enum
	{
		eInitSize = 30,
		eIncrSize = 30
	};

	struct Elem
	{
		Elem	*prev;
		Elem	*next;
		C		 content;
	};

	Elem *				mHead;
	Elem *				mTail;
	Elem *				mCrtPtr;
	Elem *				mIdle;
	Elem *				mMemory;
	C					mDefault;
	uint				mEntries;

public:
	OmnVList()
	:
	mHead(0),
	mTail(0),
	mCrtPtr(0),
	mIdle(0),
	mMemory(0),
	mEntries(0)
	{
	}

	~OmnVList() 
	{
		while (mMemory)
		{
			Elem *next = mMemory->next;
			OmnDelete [] mMemory;
			mMemory = next;
		}
	}

	int		objSize() const
			{
				int size = sizeof(*this) + mEntries * sizeof(Elem);
				Elem *elem = mIdle;
				while (elem)
				{
					size += sizeof(Elem);
					elem = elem->next;
				}
				return size;
			}

	void	setNullValue(const C c) {mDefault = c;}
    uint	entries() const {return mEntries;}

	void	append(const C& v) 
			{
				if (!mIdle)
				{
					expand();
				}

				Elem *elem = mIdle;
				mIdle = mIdle->next;

				elem->prev = mTail;
				elem->next = 0;
				if (mTail)
				{
					mTail->next = elem;
				}
				elem->content = v;
				mTail = elem;
				mEntries++;

				if (!mHead)
				{
					mHead = elem;
				}
			}

	void	clear() 
			{
				Elem *next;
				while (mHead)
				{
					mHead->content = mDefault;
					next = mHead->next;
					mHead->next = mIdle;
					mIdle = mHead;
					mHead = next;
				}

				mTail = 0;
				mEntries = 0;
			}

	C		popFront() 
			{
				if (!mHead)
				{
					std::cout << "<" << __FILE__ << ":" 
						<< __LINE__ << "> ********** ::eWarnProgError "
						<< "popFront an empty list" << std::endl;
					return mDefault;
				}

				C c = mHead->content;
				mHead->content = mDefault;
				Elem *tmp = mHead->next;
				mHead->next = mIdle;
				mIdle = mHead;
				mEntries--;
				mHead = tmp;

				if (!mHead)
				{
					mTail = 0;
				}
				return c;
			}

	void	reset() {mCrtPtr = mHead;}

	C		crtValue() const 
			{
				if (!mCrtPtr)
				{
					std::cout << "<" << __FILE__ << ":" 
						<< __LINE__ << "> ********** ::eWarnProgError "
						<< "Null pointer in List::crtValue()"
						<< std::endl;
					return mDefault;
				}
							
				return mCrtPtr->content;
			}

	C &		crtValue() 
			{
				if (!mCrtPtr)
				{
					std::cout << "<" << __FILE__ << ":" 
						<< __LINE__ << "> ********** ::eWarnProgError "
						<< "Null pointer in List::crtValue()"
						<< std::endl;
					return mDefault;
				}
							
				return mCrtPtr->content;
			}

	void	eraseCrt() 
			{
				// 
				// It removes the element pointed to by mCrtPtr.
				// If mCrtPtr is null, it is an error. After remove,
				// mCrtPtr points to mCrtPtr->next. If mCrtPtr is the
				// last element, mTail shall be modified and new value
				// for mCrtPtr shall be null.
				//
				if (!mCrtPtr)
				{
					std::cout << "<" << __FILE__ << ":" 
						<< __LINE__ << "> ********** ::eWarnProgError "
						<< "Null poiner in List::eraseCrt()"
						<< std::endl;
					return;
				}

				if (mCrtPtr->prev)
				{
					mCrtPtr->prev->next = mCrtPtr->next;
				}

				if (mCrtPtr->next)
				{
					mCrtPtr->next->prev = mCrtPtr->prev;
				}

				if (mCrtPtr == mTail)
				{
					mTail = mCrtPtr->prev;
				}

				if (mCrtPtr == mHead)
				{
					mHead = mCrtPtr->next;
				}

				mEntries--;
				mCrtPtr->content = mDefault;
				Elem *tmp = mCrtPtr->next;
				mCrtPtr->next = mIdle;
				mIdle = mCrtPtr;
				mCrtPtr = tmp;
			}

	C		next() 
			{
				if (mCrtPtr)
				{
					C rslt = mCrtPtr->content;
					mCrtPtr = mCrtPtr->next;
					return rslt;
				}

				//
				// mCrtPtr already points to the end of the list
				//
				// OmnAlarm << "Calling next() on null mCrtPtr: " << enderr;
				return mDefault;
			}

	void	insertAt(C c) 
			{
				//
				// Insert c before crt pos: mItr
				//
				if (!mIdle)
				{
					expand();
				}

				Elem *tmp = mIdle;
				tmp->content = c;
				mIdle = mIdle->next;

				if (!mHead)
				{
					mHead = tmp;
					mTail = tmp;
					mHead->next = 0;
					mHead->prev = 0;
					mEntries = 1;
				 	return;
				}

				if (!mCrtPtr)
				{
					//
					// Same as append
					//
					tmp->prev = mTail;
					tmp->next = 0;
					mTail->next = tmp;
					mTail = tmp;
					mEntries++;
					return;
				}

				if (mHead == mCrtPtr)
				{
					mHead = tmp;
				}

				tmp->prev = mCrtPtr->prev;
				if (mCrtPtr->prev)
				{
					mCrtPtr->prev->next = tmp;
				}
				tmp->next = mCrtPtr;
				mCrtPtr->prev = tmp;
				mEntries++;
				return;
			}

	bool	hasMore() const 
			{
				return mCrtPtr != 0;
			}

	void	expand()
			{
				//
				// Create the elements first. Note that the 
				// first element is reserved for managing 
				// the memory. 
				//
				Elem *elems = OmnNew Elem[eIncrSize];

				elems[0].next = mMemory;

				// 
				// Chen Ding, 02/06/2005. 
				// The prev pointer was not set. It will not cause problems, but just in case.
				//
				if (mMemory)
				{
					mMemory->prev = elems;
				}
				mMemory = elems;

				Elem *prev = 0;
				for (int i=1; i<eIncrSize-1; i++)
				{
					elems[i].next = &elems[i+1];
					elems[i].prev = prev;
					prev = &elems[i];
				}

				elems[eIncrSize-1].next = 0;
				mIdle = &elems[1];
			}
};

#endif

