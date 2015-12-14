////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmtPList.h
// Description:
//	A SmartIntValList is a smart pointer list that its elements 
//  are indexed by the pointer's function ::getHashKey(). 
//
//	This class is essentially a list. Any list operations
//  can be performed on this class. In addition, this class
//  is smart to determine whether an index table needs to 
//  be created. When the list is not big enough, the list
//  does not create an index table. As the list grows to 
//  exceed 'eTooBigThreshold', it will create an index table.
//  When the list thrinks to below 'eSmallThreshold', the index
//  table is removed. 
//  
//  When adding or deleting an element, if an index table is 
//  present, it will add or remove the index table, too. 
// 
//  This class assumes the object C supports the following 
//  functions:
//		uint C::getHashKey() const;
//		bool C::hasSameObjId(const C &rhs);   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_SmtPList_h
#define Omn_Util_SmtPList_h

#include "Alarm/Alarm.h"
#include "Statistics/Statistics.h"
#include "Util/OmnNew.h"
#include "Util/PtrList.h"


template <class C>
class OmnSmtPList
{
private:
	enum
	{
		eInitSize = 30,
		eIncrSize = 30,
		eTooBigThreshold = 200,
		eSmallThreshold = 100,
		eIndexTableSize = 0x3fff
	};

	struct Elem
	{
		Elem       *prev;
		Elem	   *next;
		C			content;
	};

	Elem *				mHead;
	Elem *				mTail;
	Elem *				mCrtPtr;
	Elem *				mIdle;
	Elem *				mMemory;
	C					mDefault;
	uint				mEntries;
	OmnPList<Elem>    * mIndexTable;

public:
	OmnSmtPList()
	:
	mHead(0),
	mTail(0),
	mCrtPtr(0),
	mIdle(0),
	mMemory(0),
	mEntries(0),
	mIndexTable(0)
	{
	}

	~OmnSmtPList() 
	{
		while (mMemory)
		{
			Elem *next = mMemory->next;
			OmnDelete [] mMemory;
			mMemory = next;
		}

		OmnDelete mIndexTable;
		mIndexTable = 0;
	}

	void	setDefaultValue(const C &c) {mDefault = c;}
    uint	entries() const {return mEntries;}

	bool	add(const C &v, const bool overrideFlag) 
			{
				// 
				// Check whether it is already in the list. If yes and 
				// 'overrideFlag' is true, it is overridden. If it is in the
				// list but not overriding, it is an error. If it is not in
				// the list, appended it.
				// 
				Elem *elem = findElem(v);
				if (elem)
				{
					// 
					// It is found. Check whether it can be overridden.
					//
					if (overrideFlag)
					{
						//
						// It can be overridden.
						//
						elem->content = v;
						return true;
					}
					else
					{
						// 
						// This is an error.
						//
						// OmnStatistics::mUtilSmtPListAddError++;
						return false;
					}
				}

				// 
				// Not in the list.
				//
				return insertElem(0, v, overrideFlag);
			}

	void	clear() 
			{
				// 
				// It removes all the elements, returns all the memory. 
				//
				while (mMemory)
				{
					Elem *next = mMemory->next;
					delete [] mMemory;
					mMemory = next;
				}

				mHead = 0;
				mTail = 0;
				mEntries = 0;
				mCrtPtr = 0;
				mMemory = 0;
				mIdle = 0;
				OmnDelete mIndexTable;
				mIndexTable = 0;
			}

	C popFront() 
			{
				if (!mHead)
				{
					OmnAlarm << "popFront an empty list" << enderr;
					return mDefault;
				}

				// 
				// Clear the contents
				//
				C c = mHead->content;
				removeElem(mHead);
				return c;
			}

	void	reset() {mCrtPtr = mHead;}

	C crtValue() const 
			{
				if (!mCrtPtr)
				{
					OmnAlarm << "Null pointer in List::crtValue()" << enderr;
					return mDefault;
				}
							
				return mCrtPtr->content;
			}

	C &	crtValue() 
			{
				if (!mCrtPtr)
				{
					OmnAlarm << "Null pointer in List::crtValue()" << enderr;
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
					OmnAlarm << "Null poiner in List::eraseCrt()" << enderr;
					return;
				}

				Elem *tmp = mCrtPtr;
				mCrtPtr = mCrtPtr->next;
				removeElem(tmp);
			}

	void removeElem(Elem *elem)
	{
		//
		// If it has a prev, set the prev's next to its next
		//
		if (elem->prev)
		{
			elem->prev->next = elem->next;
		}

		//
		// If it has a next, set the next's prev to its prev
		//
		if (elem->next)
		{
			elem->next->prev = elem->prev;
		}

		//
		// If this element is the tail, set mTail to its prev.
		//
		if (elem == mTail)
		{
			mTail = elem->prev;
		}

		// 
		// If this element is the head, set mHead to its next
		//
		if (elem == mHead)
		{
			mHead = elem->next;
		}

		// 
		// This finishes the list operations. The element is removed from the list
		// and both mHead and mTail are adjusted accordingly.
		// Next, need to return the element to the mIdle list.
		//
		C obj = elem->content;
		mEntries--;
		elem->content = mDefault;
		elem->next = mIdle;
		mIdle = elem;

		// 
		// If the index table is not null, check whether the list
		// becomes small enough to remove the index table.
		//
		if (mEntries < eSmallThreshold && mIndexTable)
		{
			OmnDelete mIndexTable;
			mIndexTable = 0;
		}

		// 
		// If the index table is not null, need to remove the object
		// from it.
		//
		if (mIndexTable)
		{
			C &value = elem->content;
			unsigned int hk = (value->getHashKey() & eIndexTableSize);

			//
			// Retrieve it
			//
			OmnPList<Elem> &bucket = mIndexTable[hk];
			const int s = bucket.entries();
			bucket.reset();
			for (int i=0; i<s; i++, bucket.next())
			{
				if ((bucket.crtValue())->content->hasSameObjId(value))
				{
					bucket.eraseCrt();
					break;
				}
			}
		}
	}

	C next() 
	{
		if (mCrtPtr)
		{
			C rslt = mCrtPtr->content;
			mCrtPtr = mCrtPtr->next;
			return rslt;
		}
		else
		{
			//
			// mCrtPtr already points to the end of the list
			//
			OmnAlarm << "Calling next on null mCrtPtr: " << enderr;
			return mDefault;
		}
	}

	C get(const C &obj, const bool deleteFlag = false)
	{
		// 
		// If the index table is present, use the index table to find the entry.
		//
		if (mIndexTable)
		{
			Elem *elem = findElem(obj);
			if (!elem)
			{
				// 
				// Didn't find the entry. Return the default.
				// 
				return mDefault;
			}

			// 
			// Found the entry. Check whether it needs to remove the entry.
			//
			C rslt = elem->content;
			if (deleteFlag)
			{
				removeElem(elem);
			}
			return rslt;
		}

		// 
		// The index table is not present. We need to do the linear search.
		//
		Elem *crt = mHead;
		while (crt)
		{
			if ((crt->content)->hasSameObjId(obj))
			{
				// 
				// Found it.
				//
				if (deleteFlag)
				{
					C rslt = crt->content;
					removeElem(crt);
					return rslt;
				}

				return crt->content;
			}

			crt = crt->next;
		}

		// 
		// Didn't find it.
		//
		return mDefault;
	}


	bool insert(const C &c, const bool overrideFlag) 
	{
		//
		// Insert c before the current position: mCrtPtr. After the insertion
		// mCrtPtr shall not be altered.
		//
		return insertElem(mCrtPtr, c, overrideFlag);
	}

	bool insertElem(Elem *crtElem, const C &c, const bool overrideFlag)
	{
		// 
		// It inserts the value 'c' before 'crtElem'. If 'crtElem' is null, 
		// it appends to the list. 
		//
		if (!mIdle)
		{
			//
			// The mIdle list is null. Need to expand it.
			//
			expand();

			if (!mIdle)
			{
				// 
				// Failed to expand. Raise the alarm and return.
				//
				OmnAlarm << OmnErrId::eMemoryFailed 
					<< "Failed to expand when inserting new object" << enderr;
				return false;
			}
		}

		//
		// Get the first element from the idle list.
		//
		Elem *tmp = mIdle;
		tmp->content = c;
		mIdle = mIdle->next;

		if (!mHead)
		{
			// 
			// mHead is null. This is an empty list. 
			//
			mHead = tmp;
			mTail = tmp;
			tmp->next = 0;
			tmp->prev = 0;
			mEntries = 1;
		 	return true;
		}

		if (!crtElem)
		{
			//
			// crtElem is null. Same as append
			//
			tmp->prev = mTail;
			tmp->next = 0;
			mTail->next = tmp;
			mTail = tmp;
			mEntries++;
		}
		else 
		{
			// 
			// crtElem is not null. We will insert 'tmp' in front of crtElem
			//
			tmp->prev = crtElem->prev;
			if (crtElem->prev)
			{
				crtElem->prev->next = tmp;
			}
			tmp->next = crtElem;
			crtElem->prev = tmp;
			mEntries++;

			if (mHead == crtElem)
			{
				// 
				// Since crtElem points to the first element, the new head
				// shall point to this new element. 
				//
				mHead = tmp;
			}
		}

		// 
		// The element has been inserted into the list. 
		//
		if (!mIndexTable && mEntries >= eTooBigThreshold)
		{
			// 
			// The mIndexTable is not present yet and it is the time to create it.
			//
			createIndexTable();
		}

		if (mIndexTable)
		{
			// 
			// The index table is not null. Need to insert the object.
			//
			addToIndexTable(tmp, overrideFlag);
		}
		return true;
	}

	bool	hasMore() const 
			{
				return mCrtPtr != 0;
			}

	bool	expand()
			{
				//
				// Create the elements first. Note that the 
				// first element is reserved for managing 
				// the memory. 
				//
				Elem *elems = OmnNew Elem[eIncrSize];

				elems[0].next = mMemory;
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

				return true;
			}
	

	bool addToIndexTable(Elem *elemToAdd, const bool overrideFlag)
	{
		if (!mIndexTable)
		{
			OmnAlarm << OmnErrId::eProgError 
				<< "Add to index table but index table is null" << enderr;
			return false;
		}

		unsigned int hk = (elemToAdd->content->getHashKey() & eIndexTableSize);

		//
		// Retrieve it
		//
		OmnPList<Elem> &bucket = mIndexTable[hk];
		const int s = bucket.entries();
		bucket.reset();
		Elem *theElem;
		for (int i=0; i<s; i++, bucket.next())
		{
			theElem = bucket.crtValue();
			if (theElem->content->hasSameObjId(elemToAdd->content))
			{
				// 
				// The object is already in the list. 
				//
				if (overrideFlag)
				{
					theElem->content = elemToAdd->content;

					// 
					// Return the element to the idle list. 
					// 
					elemToAdd->next = mIdle;
					mIdle = elemToAdd;
					return true;
				}
			}
		}

		// 
		// It is not in the list. Add it.
		//
		bucket.append(elemToAdd);
		return true;
	}

	Elem * findElem(const C &value) const
	{
		// 
		// This is a private function to find whether an element identified by 
		// 'key' exists. If yes, the element is returned. Otherwise, null is
		// returned.
		//
		if (mIndexTable)
		{
			// 
			// The index table exists. Retrieve the element from the index table.
			// 1. Calculate the hash key
			//
			unsigned int hk = (value->getHashKey() & eIndexTableSize);

			//
			// Retrieve it
			//
			OmnPList<Elem> &bucket = mIndexTable[hk];
			const int s = bucket.entries();
			bucket.reset();
			Elem *elem;
			for (int i=0; i<s; i++, bucket.next())
			{
				elem = bucket.crtValue();
				if (elem->content->hasSameObjId(value))
				{
					bucket.eraseCrt();
					return elem;
				}
			}

			//
			// Did not find it.
			//
			return 0;
		}

		// 
		// The index table does not exist. Linear search
		//
		Elem * elem = mHead;
		while (elem)
		{
			if ((elem->content)->hasSameObjId(value))
			{
				// 
				// Found it.
				// 
				return elem;
			}

			elem = elem->next;
		}

		// 
		// Didn't find it.
		//
		return 0;
	}

	void createIndexTable()
	{
		if (mIndexTable)
		{
			// 
			// The table is already there
			//
			return;
		}

		// 
		// Create the index table
		//
		mIndexTable = OmnNew OmnPList<Elem>[eIndexTableSize+1];
		Elem *elem = mHead;
		while (elem)
		{
			// 
			// Add the element into the index table
			//
			addToIndexTable(elem, true);
			elem = elem->next;
		}
	}

	int	purge()
	{
		// 
		// It removes old entries. It will return the number of 
		// bytes removed. 
		//
		return 0;
	}
};

#endif

