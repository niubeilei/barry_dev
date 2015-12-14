////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmtIdxPList.h
// Description:
//	A SmtIdxPList is a list that its elements are indexed by 
//  strings. This is similar to arrays, where arrays use
//  integers as the index while SmtIdxPList uses strings
//  as the index. 
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
//  The class is indexed on type IndType. This class assumes that
//  the IndType supports a member function 'getHashKey()'. It also
//  assumes the object C supports a function:
//		IndType	C::getKey() const;   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_SmtIdxPList_h
#define Omn_Util_SmtIdxPList_h

#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"
#include "Util/HashInt.h"
#include "Util/IndexPtrList.h"

#ifndef OmnSmtIdxPList
#define OmnSmtIdxPList OmnSIPL
#endif

template <class IndType, class C>
class OmnSIPL
{
private:
	enum
	{
		eInitSize = 30,
		eIncrSize = 30,
		eTooBigThreshold = 200,
		eSmallThreshold = 100,
		eIndexTableSize = 2048
	};

	struct Elem
	{
		Elem       *prev;
		Elem	   *next;
		C			content;
	};

	Elem *					mHead;
	Elem *					mTail;
	Elem *					mCrtPtr;
	Elem *					mIdle;
	Elem *					mMemory;
	C						mDefault;
	uint					mEntries;
	OmnIdxPList<IndType, Elem *, eIndexTableSize>    *mIndexTable;

public:
	OmnSIPL()
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

	~OmnSIPL() 
	{
		while (mMemory)
		{
			Elem *next = mMemory->next;
			delete [] mMemory;
			mMemory = next;
		}

		OmnDelete mIndexTable;
		mIndexTable = 0;
	}

	void	setDefaultValue(const C &c) {mDefault = c;}
    uint	entries() const {return mEntries;}

	bool	append(const C &v, const bool overrideFlag) 
			{
				// 
				// Check whether it is already in the list. If yes and 
				// 'overrideFlag' is true, it is overridden. If it is in the
				// list but not overriding, it is an error. If it is not in
				// the list, appended it.
				// 
				Elem *elem = findElem(v->getIndex());
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
						OmnAlarm << "Object already exist in the list for entry: " 
							<< (v->getIndex()).toString() << enderr;
						return false;
					}
				}

				// 
				// Not in the list.
				//
				if (!mIdle)
				{
					// 
					// No idle items. Need to expand.
					//
					expand();

					if (!mIdle)
					{
						// 
						// Failed to expand the list. This is an error.
						//
						OmnAlarm << "Failed to expand the list!" << enderr;
						return false;
					}
				}

				elem = mIdle;
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

				// 
				// If the index table is not present yet and the list is too big,
				// we need to create the index table.
				//
				if (!mIndexTable && mEntries > eTooBigThreshold)
				{
					// 
					// Need to create the index table
					//
					createIndexTable();
				}
				return true;
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

				// 
				// If the index table is present, delete it.
				//
				if (mIndexTable)
				{
					OmnDelete mIndexTable;
					mIndexTable = 0;
				}
			}

	C popFront() 
			{
				if (!mHead)
				{
					OmnAlarm << "popFront an empty list" << enderr;
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

				// 
				// If the index table is present, we need to remove the entry 'c'
				// from the index table
				//
				if (mIndexTable)
				{
					mIndexTable->get(c.getKey(), true);
				}

				// 
				// If the table is small enough, make sure the index table 
				// is removed.
				//
				if (mEntries < eSmallThreshold && mIndexTable)
				{
					OmnDelete mIndexTable;
					mIndexTable = 0;
				}

				return c;
			}

	void	resetLoop() {mCrtPtr = mHead;}

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
			mIndexTable->get(obj->getIndex(), true);
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
			OmnAlarm << "Calling next() on null mCrtPtr: " << enderr;
			return mDefault;
		}
	}

	C get(const IndType &key, const bool deleteFlag = false)
	{
		// 
		// If the index table is present, use the index table to find the entry.
		//
		if (mIndexTable)
		{
			Elem *elem = findElem(key);
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
			if ((crt->content)->getIndex() == key)
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


	void insert(const C &c) 
	{
		//
		// Insert c before the current position: mCrtPtr. After the insertion
		// mCrtPtr shall not be altered.
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
				OmnAlarm << "Failed to expand when inserting new object" << enderr;
				return;
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
			mHead->next = 0;
			mHead->prev = 0;
			mEntries = 1;
		 	return;
		}

		if (!mCrtPtr)
		{
			//
			// mCrtPtr is null. Same as append
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
			// mCrtPtr is not null. We will insert 'tmp' in front of mCrtPtr
			//
			tmp->prev = mCrtPtr->prev;
			if (mCrtPtr->prev)
			{
				mCrtPtr->prev->next = tmp;
			}
			tmp->next = mCrtPtr;
			mCrtPtr->prev = tmp;
			mEntries++;

			if (mHead == mCrtPtr)
			{
				// 
				// Since mCrtPtr points to the first element, the new head
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
			mIndexTable->add(c->getKey(), tmp);
		}
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
				//Elem *elems = OmnNew Elem[eIncrSize];
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
			}

	bool	isInList(const C &c) const
			{
				if (mIndexTable || mEntries >= eTooBigThreshold)
				{
					if (!mIndexTable)
					{
					}

					// 
					// We will use the index table to check whether the element
					// is in the list. 
					// 
					C rslt = mIndexTable->get(c->getKey());
					return !rslt.isNull();
				}
				else
				{
					// 
					// There is no index table. Linear search the list.
					//
					Elem *ptr = mHead;
					while (ptr)
					{
						if (ptr->content->getKey() == c->getKey())
						{
							return true;
						}

						ptr = ptr->next;
					}

					return false;
				}

				return false;
			}

	Elem * findElem(const IndType &key) const
	{
		// 
		// This is a private function to find whether an element identified by 
		// 'key' exists. If yes, the element is returned. Otherwise, null is
		// returned.
		//
		if (mIndexTable)
		{
			// 
			// The index table exists. Ask the index table.
			//
			return mIndexTable->get(key, false);
		}

		// 
		// The index table does not exist. Linear search
		//
		Elem * elem = mHead;
		while (elem)
		{
			if ((elem->content)->getIndex() == key)
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
		mIndexTable = OmnNew OmnIdxPList<IndType, Elem *, eIndexTableSize>();
		Elem *elem = mHead;
		while (elem)
		{
			// 
			// Add the element into the index table
			//
			mIndexTable->add((elem->content)->getIndex(), elem);
			elem = elem->next;
		}
	}
};

#endif

