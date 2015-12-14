////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TArray.cpp
// Description:
//	Following are the constants:
//		OMN_ARRAY4_LEVEL		The size of array (levels)
//		OMN_ARRAY4_DFSTART		The start of the default pointers
//		OMN_ARRAY4_TOTAL		The total entries in array[...]
//		OMN_ARRAY4_ENTRYLIST	The head of the entry list
//		OMN_ARRAY4_ENTRYSIZE	The size of entries for non-top levels.
//		OMN_ARRAY4_REF			The position where the reference is tored
//		OMN_ARRAY4_FAN			The number of branches in the tree
//		
//	When the array is initialized, the top level should all point to 
//	OmnArray4Dft3.
//
//	Array entries are defined as:
//		entry[0..OMN_TARRAY_FAN-1]		working pointers
//		entry[OMN_TARRAY_FAN]			dimension
//		entry[OMN_TARRAY_REFERENCE]		reference
//		entry[OMN_TARRAY_FLAGS]			only 0x01 is used 
//		entry[OMN_TARRAY_PREV]			pointer to previous
//		entry[OMN_TARRAY_NEXT]			pointer to next
//
//	The dimension is the dimension at which the entry is defined. Dimension
//  is counted from right to left. The rightmost dimension is 1. 
//		dimension = 0		the actual value (there should not be entries 
//							at this level)
//		dimension = 1		array of pointers to actual values
//		dimension = 2		array of ...
//		...
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelUtil/TArray.h"

#include <KernelSimu/string.h>
#include "aosUtil/Memory.h"
#include "aosUtil/Slab.h"

#ifdef AOS_KERNEL_SIMULATE
#include <iostream>
#include <iomanip>
using namespace std;
#endif
 
const int sgMaxEntry = 500000;
static struct AosSlab *sgSlab = 0;

int OmnTArray_checkArray(void ***array)
{
	void **entry = array[OMN_TARRAY_ENTRYHEAD];
	int dimension = (int)array[OMN_TARRAY_DIMENSION];
	int ret = 0;
	int i;

	u32 entries = OmnTArray_getNumEntries(array);
	if (entries != sgSlab->total_allocated - sgSlab->idle_blocks)
	{
		aosAlarmInt2(eAosAlarm, entries, sgSlab->total_allocated);
		return aosAlarmInt(eAosAlarm, sgSlab->idle_blocks);
	}

	// 
	// Reset the entry's OMN_TARRAY_RESERVED to reference.
	//
	while (entry)
	{
		entry[OMN_TARRAY_RESERVED] = entry[OMN_TARRAY_REFERENCE];
		entry = (void **)entry[OMN_TARRAY_NEXT];
	}

	entry = array[OMN_TARRAY_ENTRYHEAD];
	while (entry)
	{
		// 
		// For each entry, its dimension should be within the range.
		//
		int ref;
		int dim = (int)entry[OMN_TARRAY_DIMENSION];
		if (dim < 1 || dim > dimension)
		{
			aosAlarmInt2(eAosAlarmFatal, dim, dimension);
			aosAlarmInt(eAosAlarmFatal, (int)entry);
			ret = 1;
		}

		// 
		// Check the reference count
		//
		ref = (int)entry[OMN_TARRAY_REFERENCE];
		if (ref < 0)
		{
			aosAlarmInt2(eAosAlarmFatal, (int)entry, ref);
			ret = 1;
		}
		else if (ref == 0)
		{
			aosAlarmInt(eAosAlarm, (int)entry);
			ret = 1;
		}

		if (dim > 1)
		{
			int i;
			for (i=0; i<OMN_TARRAY_FAN; i++)
			{
				((int*)entry[i])[OMN_TARRAY_RESERVED]--;
			}
		}

		// 
		// Check the flag
		//
		if (entry[OMN_TARRAY_FLAGS] != 0 && (int)entry[OMN_TARRAY_FLAGS] != 1)
		{
			aosAlarmInt2(eAosAlarm, (int)entry, (int)entry[OMN_TARRAY_FLAGS]);
			ret = 1;
		}

		entry = (void **)entry[OMN_TARRAY_NEXT];
	}

	for (i=0; i<OMN_TARRAY_FAN; i++)
	{
		((int*)array[i])[OMN_TARRAY_RESERVED]--;
	}

	// 
	// All reference counters should be 0 
	//
	entry = array[OMN_TARRAY_ENTRYHEAD];
	while (entry)
	{
		if (entry[OMN_TARRAY_RESERVED] != 0)
		{
			aosAlarmInt2(eAosAlarm, (int)entry, 
				(int)entry[OMN_TARRAY_RESERVED]);
			ret = 1;
		}

		entry = (void **)entry[OMN_TARRAY_NEXT];
	}

	return ret;
}


int OmnTArray_deleteEntry(void ***array, void **toBeDeleted)
{
	// 
	// It deletes the entry 'entry'. The caller should have made sure
	// no one in the array points to this entry. 
	// 
	// 1. If the entry dimension is 1. Delete it. 
	// 2. Otherwise, for every non-default pointer in the entry, 
	//    decrement the reference count of the pointed to. If the
	//    reference becomes 0, that entry (next level) should be
	//    deleted too. Put it in the list.
	//
	void **list[OMN_TARRAY_LISTSIZE];
	void **entry;
	int size = 1;
	int i;
	int dim;
	list[0] = toBeDeleted;

	while (size)
	{
		// 
		// Remove the entry from the tail
		//
		entry = list[size-- - 1];
		dim = OMN_TARRAY_DIM(entry);
		if (dim < 1 || dim > (int)array[OMN_TARRAY_DIMENSION])
		{
			return aosAlarmInt(eAosAlarmFatal, dim);
		}
			
		if (dim == 1)
		{
			// 
			// The rightmost entry. If it is not the default pointer and 
			// its reference count is 0, delete it.
			//
			if (OmnTArrayDft(array, dim) != entry && OMN_TARRAY_REF(entry) == 0)
			{
				OMN_TARRAY_REMOVEENTRY(array, entry);
				AosSlab_release(sgSlab, entry);
				continue;
			}
		}

		for (i=0; i<OMN_TARRAY_FAN; i++)
		{
			if (!OMN_TARRAY_IS_DEFAULT(array, entry, i) && 
				--OMN_TARRAY_REF(entry[i]) == 0)
			{
				// 
				// The entry should be deleted.
				//
				list[size++] = (void **)entry[i];
			}
		}

		// 
		// Delete the entry
		//
		OMN_TARRAY_REMOVEENTRY(array, entry);
		AosSlab_release(sgSlab, entry);
	}

	return 0;
}


/*
// 
// All the entries are stored in a linked list whose head is stored
// in array[OMN_ARRAY4_ENTRYLIST]. This function appends a new entry
// to the list. If the entry is already there, it returns an error.
//
// Open Issues:
// 1. Since it uses a link list, if there are too many entries, the
//    performance to add/delete entries can be a problem.
// 2. Do we really need to store the entries, if overlapped entries
//    are not allowed?
//
template <class T>
int OmnTArrayAddEntry(T ****array, 
					  unsigned char *index, 
					  unsigned char *mask, 
					  T value)
{
	OmnTArrayCtlEntry<T> *prev = 0;
	OmnTArrayCtlEntry<T> *entry = (OmnTArrayCtlEntry<T>*)array[OMN_ARRAY4_ENTRYLIST];
	while (entry)
	{
		if (memcmp(entry->mIndex, index, OMN_ARRAY4_LEVEL) == 0 &&
			memcmp(entry->mMask, mask, OMN_ARRAY4_LEVEL) == 0 &&
			entry->mValue == value)
		{
			// The entry is already in the list. Return
			return OMN_ENTRY_ALREADY_EXIST;
		}

		prev = entry;
		entry = entry->mNext;
	}

	// Entry is not in the list. Add it.
	entry = OmnNew OmnArray4CtlEntry<T>;
	entry->mPrev = prev;
	entry->mNext = 0;
	entry->mValue = value;
	memcpy(entry->mIndex, index, OMN_ARRAY4_LEVEL);
	memcpy(entry->mMask, mask, OMN_ARRAY4_LEVEL);

	if (prev == 0)
	{
		array[OMN_ARRAY4_ENTRYLIST] = (T ***)entry;
	}

	return OMN_SUCCESS;
}


template <class T>
OmnArray4CtlEntry<T> * OmnArray4GetEntry(T ****array, 
					  unsigned char *index, 
					  unsigned char *mask)
{
	OmnArray4CtlEntry<T> *entry = (OmnArray4CtlEntry<T>*)array[OMN_ARRAY4_ENTRYLIST];
	while (entry)
	{
		if (memcmp(entry->mIndex, index, OMN_ARRAY4_LEVEL) == 0 &&
			memcmp(entry->mMask, mask, OMN_ARRAY4_LEVEL) == 0)
		{
			return entry;
		}

		entry = entry->mNext;
	}

	return 0;
}
*/


// 
// It initializes the array to the value 'value'. 'value' is the default
// value when the array is initialized.
//
int OmnTArray_init(void ***array, 
				   const int dimension, 
				   void *defaultValue)
{
	// 
	// The array is arranged as follows:
	//		array[0..fan-1]		working pointers
	//		array[fan]			the array's dimension
	//		array[fan+1]		the head of inserted list (not used now)
	//		array[fan+2]		the head of entry list
	//		array[fan+3]		first dimension's default pointer 
	//							(also the default value)
	//		array[fan+4]		second dimension's default pointer
	//		array[fan+5]		third dimension's default pointer 
	//		array[fan+6]		fourth dimension's default pointer (in 
	//							general, for dimension i, its default pointer 
	//							is 'fan + dimension + 1', where dimension 
	//							counts from 1 (the rightmost dimension). 
	//							For 4-dimensional array, for instance:
	//								array[i]          = array[fan+4+1]
	//								array[i][j]       = array[fan+3+1]
	//								array[i][j][k]    = array[fan+2+1]
	//								array[i][j][k][m] = array[fan+1+1]
	//		...
	//
	void *lowerLevelPtr = 0;
	void **theArray = (void **)aos_malloc(4*(OMN_TARRAY_DEFAULT+dimension));

	int i, j;

	if (!sgSlab)
	{
		AosSlab_constructor("TArray", 
			OMN_TARRAY_ENTRYSIZE * sizeof(void*), sgMaxEntry, &sgSlab);
		if (!sgSlab)
		{
			return aosAlarm(eAosAlarmMemErr);
		}
	}

	if (!theArray)
	{
		return aosAlarm(eAosAlarmMemErr);
	}

	// The dimension
	theArray[OMN_TARRAY_FAN] = (void *)dimension;
	theArray[OMN_TARRAY_INSERTED] = 0;
	theArray[OMN_TARRAY_ENTRYHEAD] = 0;
	theArray[OMN_TARRAY_ENTRIES] = 0;

	// Set the default value
	theArray[OMN_TARRAY_DEFAULT] = defaultValue;

	// 
	// Create the default pointers
	//
	lowerLevelPtr = defaultValue;
	for (i=0; i<dimension-1; i++)
	{
//		void **dft = OmnNew void *[OMN_TARRAY_ENTRYSIZE];
		void **dft = (void **)AosSlab_get(sgSlab);
		if (!dft)
		{
			return aosAlarm(eAosAlarmMemErr);
		}

		((int*)dft)[OMN_TARRAY_REFERENCE] = 0;
		((int*)dft)[OMN_TARRAY_FLAGS] = 0;
		((int*)dft)[OMN_TARRAY_PREV] = 0;
		((int*)dft)[OMN_TARRAY_NEXT] = 0;

		for (j=0; j<OMN_TARRAY_FAN; j++)
		{
			dft[j] = lowerLevelPtr;
		}
		
		theArray[OMN_TARRAY_DEFAULT+1+i] = (void *)dft;
		lowerLevelPtr = theArray[OMN_TARRAY_DEFAULT+1+i];
	}

	// 
	// Set all working pointers to the top dimension's default pointer
	//
	for (i=0; i<OMN_TARRAY_FAN; i++)
	{
		theArray[i] = OmnTArrayDft(theArray, dimension);
	}
	*array = theArray;
	return 0;
}


/*
int OmnTArray_deleteEntry(T ****array, 
						  int level, 
						  T **entry)
{
	if (level < 1)
	{
		// This is an error. 
		OmnKernelAlarm("Invalid level");
		return -1;
	}

	entry[OMN_ARRAY4_REF] = (T*)((int)entry[OMN_ARRAY4_REF] - 1);
	if ((int)entry[OMN_ARRAY4_REF] > 0)
	{
		return 0;
	}

	if (level == 1)
	{
		OmnDelete [] entry;
		return 0;
	}

	for (int i=0; i<256; i++)
	{
		if ((void*)entry[i] != (void*)array[OMN_ARRAY4_DFSTART+level-1])
		{
			OmnArray4DeleteEntry(array, level-1, (T**)entry[i]);
		}
	}

	OmnDelete [] entry;
	return 0;
}
*/

int OmnTArray_copyEntry(void **toEntry, void **fromEntry, unsigned int size)
{
	int i;
	memcpy(toEntry, fromEntry, size);
	if (OMN_TARRAY_DIM(toEntry) <= 1)
	{
		return 0;
	}

	for (i=0; i<OMN_TARRAY_FAN; i++)
	{
		OMN_TARRAY_REF(toEntry[i])++;
	}

	return 0;
}


int OmnTArray_destroy(void ***array)
{
	void **next;
	void **entry = array[OMN_TARRAY_ENTRYHEAD];
	while (entry)
	{
		next = (void **)entry[OMN_TARRAY_NEXT];
		AosSlab_release(sgSlab, entry);
		entry = next;
	}

	array[OMN_TARRAY_ENTRYHEAD] = 0;
	array[OMN_TARRAY_ENTRIES] = 0;
	return 0;
}


int OmnTArray_deleteSegment(void ***array, 
							int segmentStart, 
							int segmentEnd,
							void ***crtEntry,
							void **workingEntry, 
							int dim, 
							void ****list, 
							int *listIdx, 
							int crtEnd)
{
	int k;
	int segmentSize = segmentEnd - segmentStart;

	// 
	// Sanity checking
	//
	if (OMN_TARRAY_REF(workingEntry) < segmentSize)
	{
		// 
		// Program error. Should never happen. Treated as if it is
		// > segmentSize
		//
		if (OmnTArray_checkArray((array)))
		{
			aosAlarm(eAosAlarm);
		}
			
		aosAlarmInt2(eAosAlarmFatal, OMN_TARRAY_REF(workingEntry), segmentSize);
		aosAlarmInt(eAosAlarmFatal, (int)array);
	}

	if (OMN_TARRAY_REF(workingEntry) == segmentSize)
	{
		//
		// The pointer is not shared by others. If the pointer is not already 
		// in the list yet, add it.
		//
		for (k=crtEnd; k<*listIdx; k++)
		{
			if (list[k] == (void ***)workingEntry)
			{
				break;
			}
		}

		if (k >= *listIdx)
		{
			list[(*listIdx)++] = (void ***)workingEntry;
		}
	}
	else
	{
		// 
		// The pointed to (i.e., workingEntry) is shared by others. Need to
		// duplicate it, modify the pointers [segmentStart, i-1], and add
		// the duplicated entry into the list. 
		// 
		void **newNextEntry = (void **)AosSlab_get(sgSlab);
		if (!newNextEntry)
		{
			return aosAlarmInt(eAosAlarmMemErr, (int)array);
		}
		((int*)newNextEntry)[OMN_TARRAY_FLAGS] = 0;
		OMN_TARRAY_ADDENTRY(array, newNextEntry);

		OMN_TARRAY_DIM(newNextEntry) = dim-1;
		OmnTArray_copyEntry((void **)newNextEntry, (void **)workingEntry, 
			sizeof(void **[OMN_TARRAY_FAN]));

		for (k=segmentStart; k<segmentEnd; k++)
		{
			OMN_TARRAY_SETPTR(crtEntry, k, newNextEntry);
		}
		((int*)newNextEntry)[OMN_TARRAY_REFERENCE] = segmentSize;
		list[(*listIdx)++] = (void ***)newNextEntry;
		
#ifdef OMN_DEBUG_FLAG
		if (OmnTArray_checkArray(array))
		{
			aosAlarm(eAosAlarm);
		}
#endif
	}

	return 0;
}


#ifndef __KERNEL__
void OmnTArray_dump(void ***array)
{
	void **entry = array[OMN_TARRAY_ENTRYHEAD];
	int dimension = (int)array[OMN_TARRAY_DIMENSION];
	void *working = (void*)array[0];
	int startIdx = 0;
	int i;
	for (i=256; i<OMN_TARRAY_DEFAULT+dimension; i++)
	{
		cout << "Array: " << dec << i << "=" << hex << array[i] << endl;
	}

	entry = (void **)array;
	for (i=0; i<OMN_TARRAY_FAN; i++)
	{
		if (entry[i] != working)
		{
			cout << "[" << dec << startIdx << "-" << i-1 << " "
				<< dec << working << "] ";
			working = entry[i];
			startIdx = i;
		}
	}
	cout << "[" << dec << startIdx << "-" << i-1 << " "
				<< dec << working << "] " << endl;

	entry = array[OMN_TARRAY_ENTRYHEAD];
	while (entry)
	{
		cout << "Entry: " << hex << entry << ": "
			<< dec << (unsigned int)entry[OMN_TARRAY_REFERENCE] << "\t";
		working = entry[0];
		startIdx = 0;
		for (i=0; i<OMN_TARRAY_FAN; i++)
		{
			if (entry[i] != working)
			{
				cout << "[" << dec << startIdx << "-" << i-1 << " "
					<< dec << working << "] ";
				working = entry[i];
				startIdx = i;
			}
		}
		cout << "[" << dec << startIdx << "-" << i-1 << " "
					<< dec << working << "] " << endl;

		entry = (void **)entry[OMN_TARRAY_NEXT];
	}
}
#endif


int OmnTArray_addSegment1(void ***array, 
						  void **crtEntry,
						  void **workingEntry, 
						  int segmentStart, 
						  int segmentEnd, 
						  int nextmin,
						  int nextmax,
						  void **nextDft, 
						  struct OmnEntryToAdd *list, 
						  int *listIdx,
						  int dim, 
						  int isDelete, 
						  void *value)
{
	int m;
	int segmentSize = segmentEnd - segmentStart;
	int p;
	void ***newNextEntry;

	if (OMN_TARRAY_REF(workingEntry) == segmentSize)
	{
		// 
		// It is not shared by others. Add it to list.
		//
		list[*listIdx].mCrt = nextmin;
		list[*listIdx].mNewEntry = 0;
		list[*listIdx].mEntry = workingEntry;
		(*listIdx)++;
		return 0;
	}

	// 
	// It is shared by others. 
	//
	if (OMN_TARRAY_REF(workingEntry) < segmentSize)
	{
		// 
		// Program error. Should never happen. Treated as if it is
		// > segmentSize
		//
		if (OmnTArray_checkArray((array)))
		{
			aosAlarm(eAosAlarm);
		}

		aosAlarmInt2(eAosAlarmFatal, OMN_TARRAY_REF(workingEntry), segmentSize);
		aosAlarmInt(eAosAlarmFatal, (int)array);
	}

	
	// 
	// If it is delete and workingEntry[nextmin, nextmax] == default, 
	// no need to duplicate it.
	// 
	if (isDelete)
	{
		for (p=nextmin; p<=nextmax; p++)
		{
			if (workingEntry[p] != nextDft)
			{
				break;
			}
		}

		if (p > nextmax)
		{
			return 0;
		}
	}
	else
	{
		// 
		// It is insert. If it is dim=1 and workingEntry[nextmin, nextmax] 
		// == value do not do anything.
		//
		if (dim == 2)
		{
			for (p=nextmin; p<=nextmax; p++)
			{
				if (workingEntry[p] != value)
				{
					break;
				}
			}

			if (p > nextmax)
			{
				return 0;
			}
		}
	}

	// It is shared by others. Need to duplicate it.
	//
	newNextEntry = (void ***)AosSlab_get(sgSlab);
	if (!newNextEntry)
	{
		return aosAlarmInt2(eAosAlarmMemErr, (int)array, 
			OmnTArray_getNumEntries(array));
	}
	((int*)newNextEntry)[OMN_TARRAY_FLAGS] = 0;
	OMN_TARRAY_ADDENTRY(array, newNextEntry);

	OMN_TARRAY_DIM(newNextEntry) = dim-1;
	OmnTArray_copyEntry((void **)newNextEntry, (void **)workingEntry, 
		sizeof(void **[OMN_TARRAY_FAN]));

	if (*listIdx >= OMN_TARRAY_LISTSIZE)
	{
		aosAlarm(eAosAlarm);
		return -1;
	}

	list[*listIdx].mCrt = nextmin;
	list[*listIdx].mNewEntry = 0;
	list[*listIdx].mEntry = (void **)newNextEntry;
	(*listIdx)++;

	for (m=segmentStart; m<segmentEnd; m++)
	{
		OMN_TARRAY_SETPTR(crtEntry, m, newNextEntry);
	}
	OMN_TARRAY_REF(newNextEntry) = segmentSize;

#ifdef OMN_DEBUG_FLAG
	if (OmnTArray_checkArray(array))
	{
		aosAlarm(eAosAlarm);
	}
#endif
	return 0;
}


int OmnTArray_getNumEntries(void ***array)
{
	int *entry = (int *)array[OMN_TARRAY_ENTRYHEAD];
	int entries = 0;
	while (entry)
	{
		entries++;
		entry = (int *)entry[OMN_TARRAY_NEXT];
	}

	return entries;
}


int OmnTArray_procLastSegment(void ***array, 
								   int min, 
								   int max, 
								   int nextMin,
								   struct OmnEntryToAdd *list, 
								   struct OmnEntryToAdd *crtEntry,
								   int *listIdx, 
								   void **dftPtr, 
								   int dim)
{
	//
	// Need to go over one more time to replace all defPtr
	//
	int isAllDefault = 1;
	int i;

	for (i=min; i<=max; i++)
	{
		if (crtEntry->mEntry[i] == dftPtr)
		{
			if (!crtEntry->mNewEntry)
			{
				// 
				// The entry can be used.
				//
				crtEntry->mNewEntry = (void **)AosSlab_get(sgSlab);
				if (!crtEntry->mNewEntry)
				{
					return aosAlarmInt(eAosAlarmMemErr, (int)array);
				}
				((int*)crtEntry->mNewEntry)[OMN_TARRAY_FLAGS] = 0;
				OMN_TARRAY_ADDENTRY(array, crtEntry->mNewEntry);
				memcpy(crtEntry->mNewEntry, dftPtr, 
					sizeof(void **[OMN_TARRAY_FAN]));
				OMN_TARRAY_DIM(crtEntry->mNewEntry) = dim-1;
				OMN_TARRAY_REF(crtEntry->mNewEntry) = 0;

				list[*listIdx].mCrt = nextMin;
				list[*listIdx].mNewEntry = 0;
				list[*listIdx].mEntry = (void **)crtEntry->mNewEntry;
				(*listIdx)++;
			}

			crtEntry->mEntry[i] = (void**)crtEntry->mNewEntry;
			isAllDefault = 0;
			OMN_TARRAY_REF(crtEntry->mNewEntry)++;
		}
	}
			
#ifdef OMN_DEBUG_FLAG
	if (OmnTArray_checkArray(array))
	{
		aosAlarm(eAosAlarm);
	}
#endif

	return 0;
}


int OmnTArray_finishSegment(void ***array, 
							int min, 
							int max,
							void **dftPtr,
							int dim,
							int nextmin,
							struct OmnEntryToAdd *list,
							int *listIdx,
							struct OmnEntryToAdd *crtEntry, 
							int isDelete)
{
	//
	// Need to go over one more time to replace all defPtr
	//
	int isAllDefault = 1;
	int i;

	for (i=0; i<OMN_TARRAY_FAN; i++)
	{
		// 
		// If the entry does not points to an entry that contains all 
		// default nor it points to its default entry, 'isAllDefault' 
		// is set to 0.
		//
		if (i >= min && i <= max && crtEntry->mEntry[i] == dftPtr)
		{
			if (!isDelete)
			{
				if (!crtEntry->mNewEntry)
				{
					crtEntry->mNewEntry = (void **)AosSlab_get(sgSlab);
					if (!crtEntry->mNewEntry)
					{
						return aosAlarmInt(eAosAlarmMemErr, (int)array);
					}

					((int*)crtEntry->mNewEntry)[OMN_TARRAY_FLAGS] = 0;
					OMN_TARRAY_ADDENTRY(array, crtEntry->mNewEntry);
					memcpy(crtEntry->mNewEntry, dftPtr, 
						sizeof(void **[OMN_TARRAY_FAN]));
					OMN_TARRAY_DIM(crtEntry->mNewEntry) = dim-1;
					OMN_TARRAY_REF(crtEntry->mNewEntry) = 0;

					list[*listIdx].mCrt = nextmin;
					list[*listIdx].mNewEntry = 0;
					list[*listIdx].mEntry = (void **)crtEntry->mNewEntry;
					(*listIdx)++;
				}

				crtEntry->mEntry[i] = (void**)crtEntry->mNewEntry;
				isAllDefault = 0;
				OMN_TARRAY_REF(crtEntry->mNewEntry)++;
			}
		}

		if (OMN_TARRAY_ISALLDEFAULT(crtEntry->mEntry[i]))
		{
			void **ee = (void **)crtEntry->mEntry[i];
			OMN_TARRAY_SETDFTPTR(array, crtEntry->mEntry, i);
			if (OMN_TARRAY_REF(ee) == 0)
			{
				// 
				// No one points to the entry anymore. Delete it.
				//
				OMN_TARRAY_REMOVEENTRY(array, ee);
				AosSlab_release(sgSlab, ee);
			}
		}
		else if (isAllDefault && 
					!OMN_TARRAY_IS_DEFAULT(array, crtEntry->mEntry, i))
		{
			isAllDefault = 0;
		}
	}

	if (isAllDefault && crtEntry->mEntry != (void **)array)
	{
		OMN_TARRAY_SETDFTFLAG(crtEntry->mEntry);
	}

#ifdef OMN_DEBUG_FLAG
	if (OmnTArray_checkArray(array))
	{
		aosAlarm(eAosAlarm);
	}
#endif

	return 0;
}


int OmnTArray_insert1(void * **array,
					  unsigned char *index, 
					  unsigned char *mask, 
					  void * value, 
					  int isDelete)
{
	int dimension = (int)array[OMN_TARRAY_DIMENSION];
	int dim = dimension;
	unsigned char min, max, nextmin, nextmax; 
	struct OmnEntryToAdd list[OMN_TARRAY_LISTSIZE];
	int listIdx = 1;
	void * *workingEntry;
	struct OmnEntryToAdd crtEntry;
	void * *theEntry;
	void * *dftPtr;
	void * *dftPtrNext;
	int segmentStart;
	int isAllDefault;
	void * dftValue = OMN_TARRAY_DFT(array);
	int crtIdx;

	list[0].mCrt = (index[0] & mask[0]);
	list[0].mNewEntry = 0;
	list[0].mEntry = (void **)array;

	// 
	// Make sure the mask is correct.
	//
	if (!aosIsValidArrayMask(mask, (int)array[OMN_TARRAY_DIMENSION]))
	{
		return aosAlarmInt(eAosAlarmFatal, (int)array[OMN_TARRAY_DIMENSION]);
	}

	// 
	// Entry is added into the entry list. Next, modify the array.
	// 'dimension' starts from the leftmost.
	// The rightmost two dimensions (dimension 1 and 2) are 
	// processed differently. 
	//

	while (listIdx)
	{
		int i;

		crtIdx = listIdx-1;
		crtEntry = list[crtIdx];
		dim = OMN_TARRAY_DIM(crtEntry.mEntry);
		min = index[dimension-dim] & mask[dimension-dim];
		max = index[dimension-dim] | (~mask[dimension-dim]);
		if (dim == 1)
		{
			isAllDefault = 1;
			for (i=0; i<OMN_TARRAY_FAN; i++)
			{
				if (i>=min && i<=max)
				{
					crtEntry.mEntry[i] = (void**)value;
				}

				if (crtEntry.mEntry[i] != dftValue)
				{
					isAllDefault = 0;
				}
			}

			if (isAllDefault)
			{
				OMN_TARRAY_SETDFTFLAG(crtEntry.mEntry);
			}

			listIdx--;
			continue;
		}

		dftPtr     = OmnTArrayDft(array, dim);
		dftPtrNext = OmnTArrayDft(array, dim-1);
		nextmin = index[dimension-dim+1] & mask[dimension-dim+1];
		nextmax = index[dimension-dim+1] | (~mask[dimension-dim+1]);

		segmentStart = crtEntry.mCrt;
		if (segmentStart > max)
		{
			// 
			// It finishes the current entry processing. 
			//
			listIdx--;
			OmnTArray_finishSegment(array, min, max, dftPtr, dim, nextmin, 
				list, &listIdx, &crtEntry, isDelete);
			continue;
		}

		workingEntry = (void **)crtEntry.mEntry[segmentStart];
		for (i=segmentStart; i<=max; i++)
		{
			// 
			// Searching for the next segment. A segment is a continuous
			// same entries that are not default pointers.
			//
			theEntry = (void **)crtEntry.mEntry[i];
			if (theEntry == workingEntry)
			{
				continue;
			}

			if (workingEntry == dftPtr)
			{
				workingEntry = theEntry;
				segmentStart = i;
				continue;
			}

			// 
			// Found a new segment, but the current one has not finished yet.
			//
			OmnTArray_addSegment1(array, crtEntry.mEntry, workingEntry, 
				segmentStart, i, nextmin, nextmax, dftPtrNext, list, 
				&listIdx, dim, isDelete, value);
			break;
		}

		list[crtIdx].mCrt = i;

		// 
		// Check for the last segment
		//
		if (i > max)
		{
			if (workingEntry != dftPtr)
			{
				// 
				// This is the last segment. Need to add it to the list
				//
				OmnTArray_addSegment1(array, crtEntry.mEntry, 
					workingEntry, segmentStart, i, nextmin, nextmax, 
					dftPtrNext, list, &listIdx, dim, isDelete, value);
			}
			else
			{
				if (!isDelete)
				{
					OmnTArray_procLastSegment(array, min, max, nextmin, 
						list, &crtEntry, &listIdx, dftPtr, dim);
				}
			}
		}
	}

	return 0;
}


void OmnTArray_getSlab(int *numIdle, int *numAllocated)
{
	*numIdle = sgSlab->idle_blocks;
	*numAllocated = sgSlab->total_allocated;
	return;
}

