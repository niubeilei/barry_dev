////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TSArray.h
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
//	When the array is initialized, the top level should all point to OmnArray4Dft3.
//
//	Array entries are defined as:
//		entry[0..OMN_TSARRAY_FAN-1]		working pointers
//		entry[OMN_TSARRAY_FAN]			dimension
//		entry[OMN_TSARRAY_REFERENCE]		reference
//		entry[OMN_TSARRAY_FLAGS]			only 0x01 is used 
//		entry[OMN_TSARRAY_PREV]			pointer to previous
//		entry[OMN_TSARRAY_NEXT]			pointer to next
//
//	The dimension is the dimension at which the entry is defined. Dimension
//  is counted from right to left. The rightmost dimension is 1. 
//		dimension = 0		the actual value (there should not be entries at this level)
//		dimension = 1		array of pointers to actual values
//		dimension = 2		array of ...
//		...   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_TArray_h
#define Omn_Util_TArray_h

#include "Alarm/Alarm.h"
#include "aos/aosKernelAlarm.h"
#include "KernelUtil/KernelAlarm.h"
#include "Porting/cout.h"

#include <iomanip>

#define OMN_ENTRY_ALREADY_EXIST	1
#define OMN_SUCCESS				0
#define OMN_TSARRAY_FAN			256
 
// Constants for array
#define OMN_TSARRAY_DIMENSION	256		// The array's dimension
#define OMN_TSARRAY_INSERTED	257		// A list of inserted items. Not used now.
#define OMN_TSARRAY_ENTRYHEAD	258		// A list of all entries created so far
#define OMN_TSARRAY_ENTRIES		259		// Number of entries
#define OMN_TSARRAY_DEFAULT		260		// The array's default value

// Constants for entries
#define OMN_TSARRAY_REFERENCE	257		// The reference count
#define OMN_TSARRAY_FLAGS		258		// 0x01: all default values/pointers
#define OMN_TSARRAY_PREV			259		// Pointer to the previous entry
#define OMN_TSARRAY_NEXT			260		// Pointer to the next entry
#define OMN_TSARRAY_RESERVED		261		// Reserved
#define OMN_TSARRAY_NONDEFAULT	262	
#define OMN_TSARRAY_ENTRYSIZE	263		// The entry size

// Flags
#define OMN_TSARRAY_DEFAULTFLAG	0x01

#define OMN_TSARRAY_LISTSIZE		5000

#ifndef OmnTSArrayDft
#define OmnTSArrayDft(array, dimension) ((void **)array[OMN_TSARRAY_DEFAULT+dimension-1])
#endif

extern int aosTotalUtilTestAlarms;
#define OmnUtilTesterAlarm if (aosTotalUtilTestAlarms > 1000) exit(0); aosTotalUtilTestAlarms++; OmnAlarm

#define OMN_TSARRAY_REF(entry) ((int*)entry)[OMN_TSARRAY_REFERENCE]
#define OMN_TSARRAY_DIM(entry) ((int*)entry)[OMN_TSARRAY_DIMENSION]
#define OMN_TSARRAY_DFT(array) (void *)array[OMN_TSARRAY_DEFAULT]

#define OMN_TSARRAY_IS_DEFAULT(array, entry, i) \
	(entry[i] == OmnTSArrayDft(array, OMN_TSARRAY_DIM(entry)))

#define OMN_TSARRAY_ISENTRYDELETED(entry) \
	(((int*)entry)[OMN_TSARRAY_DIMENSION] == -1)

#define OMN_TSARRAY_SETENTRYDELETED(entry) \
	((int*)entry)[OMN_TSARRAY_DIMENSION] = -1;

#define OMN_TSARRAY_SETDFTFLAG(entry) \
	((int*)entry)[OMN_TSARRAY_FLAGS] |= OMN_TSARRAY_DEFAULTFLAG

#define OMN_TSARRAY_ISALLDEFAULT(entry) \
	((int*)entry)[OMN_TSARRAY_FLAGS] & OMN_TSARRAY_DEFAULTFLAG

#define OMN_TSARRAY_ADDENTRY(array, entry) \
	((void **)entry)[OMN_TSARRAY_NEXT] = (void *)array[OMN_TSARRAY_ENTRYHEAD]; \
	((void **)entry)[OMN_TSARRAY_PREV] = 0; \
	if (array[OMN_TSARRAY_ENTRYHEAD]) \
		((void **)array[OMN_TSARRAY_ENTRYHEAD])[OMN_TSARRAY_PREV] = (void *)entry; \
	((void **)array)[OMN_TSARRAY_ENTRYHEAD] = (void *)entry; \
	((int *)array)[OMN_TSARRAY_ENTRIES]++;

#define OMN_TSARRAY_REMOVEENTRY(array, entry) \
	if (entry[OMN_TSARRAY_PREV]) \
		((void **)entry[OMN_TSARRAY_PREV])[OMN_TSARRAY_NEXT] = (void *)entry[OMN_TSARRAY_NEXT]; \
	if (entry[OMN_TSARRAY_NEXT]) \
		((void **)entry[OMN_TSARRAY_NEXT])[OMN_TSARRAY_PREV] = (void *)entry[OMN_TSARRAY_PREV]; \
	if ((void **)array[OMN_TSARRAY_ENTRYHEAD] == (void **)entry) \
		((void **)array)[OMN_TSARRAY_ENTRYHEAD] = (void *)entry[OMN_TSARRAY_NEXT]; \
	entry[OMN_TSARRAY_NEXT] = 0; \
	entry[OMN_TSARRAY_PREV] = 0; \
	((int *)array)[OMN_TSARRAY_ENTRIES]--;

#define OMN_TSARRAY_SETDFTPTR(array, entry, idx) \
	((int*)entry[idx])[OMN_TSARRAY_REFERENCE]--; \
	entry[idx] = OmnTSArrayDft(array, OMN_TSARRAY_DIM(entry))

struct OmnEntryToAdd
{
	int				mCrt;
	void **			mEntry;
	void **			mNewEntry;
};

extern int tmpMaxListSize;
void OmnTSArray_dump(void ***array);

// 
// If it is dimension 1, simply assign it. Otherwise, decrement
// the reference of the pointed to entry and then assign it.
//
#define OMN_TSARRAY_SETPTR(xEntry, xIndex, xNextPtr) \
	if (OMN_TSARRAY_DIM(xEntry) > 1) \
	{ \
		OMN_TSARRAY_REF(xEntry[xIndex])--; \
	} \
	xEntry[xIndex] = (void**)xNextPtr; 

extern int inline OmnTSArray_isEntryRedundant(void **theEntry, 
									  struct OmnEntryToAdd *list, 
									  void **dft,
									  int startIdx,
									  int endIdx, 
									  void ***redundantEntry, 
									  unsigned char index, 
									  unsigned char mask);


int OmnTSArray_checkArray(void ***array)
{
	void **entry = array[OMN_TSARRAY_ENTRYHEAD];
	int dimension = (int)array[OMN_TSARRAY_DIMENSION];
	int ret = 0;

	// 
	// Reset the entry's OMN_TSARRAY_RESERVED to reference.
	//
	while (entry)
	{
		entry[OMN_TSARRAY_RESERVED] = entry[OMN_TSARRAY_REFERENCE];
		entry = (void **)entry[OMN_TSARRAY_NEXT];
	}

	entry = array[OMN_TSARRAY_ENTRYHEAD];
	while (entry)
	{
		// 
		// For each entry, its dimension should be within the range.
		//
		int dim = (int)entry[OMN_TSARRAY_DIMENSION];
		if (dim < 1 || dim > dimension)
		{
			OmnAlarm << "Internal Error" << enderr;
			// aosAlarmInt2(eAosAlarmFatal, dim, dimension);
			// aosAlarmInt(eAosAlarmFatal, (int)entry);
			// OmnUtilTesterAlarm << dim << ", " << dimension << ", " << (int)entry << enderr;
			ret = 1;
		}

		// 
		// Check the reference count
		//
		int ref = (int)entry[OMN_TSARRAY_REFERENCE];
		if (ref < 0)
		{
			OmnAlarm << "Internal Error" << enderr;
			// aosAlarmInt2(eAosAlarmFatal, (int)entry, ref);
			// OmnUtilTesterAlarm << (int)entry << ", " << ref << enderr;
			ret = 1;
		}
		else if (ref == 0)
		{
			OmnAlarm << "Internal Error" << enderr;
			// aosAlarmInt(eAosAlarm, (int)entry);
			// OmnUtilTesterAlarm << (int)entry << enderr;
			ret = 1;
		}

		if (dim > 1)
		{
			for (int i=0; i<OMN_TSARRAY_FAN; i++)
			{
				((int*)entry[i])[OMN_TSARRAY_RESERVED]--;
			}
		}

		// 
		// Check the flag
		//
		if (entry[OMN_TSARRAY_FLAGS] != 0 && (int)entry[OMN_TSARRAY_FLAGS] != 1)
		{
			OmnAlarm << "Internal Error" << enderr;
			// aosAlarmInt2(eAosAlarm, (int)entry, (int)entry[OMN_TSARRAY_FLAGS]);
			// OmnUtilTesterAlarm << (int)entry << ", " << (int)entry[OMN_TSARRAY_FLAGS] << enderr;
			ret = 1;
		}

		entry = (void **)entry[OMN_TSARRAY_NEXT];
	}

	for (int i=0; i<OMN_TSARRAY_FAN; i++)
	{
		((int*)array[i])[OMN_TSARRAY_RESERVED]--;
	}

	// 
	// All reference counters should be 0 
	//
	entry = array[OMN_TSARRAY_ENTRYHEAD];
	while (entry)
	{
		if (entry[OMN_TSARRAY_RESERVED] != 0)
		{
			OmnTSArray_dump(array);
			OmnAlarm << "Internal Error" << enderr;
			// aosAlarmInt2(eAosAlarm, (int)entry, (int)entry[OMN_TSARRAY_RESERVED]);
			// OmnUtilTesterAlarm << (int)entry << ", " << (int)entry[OMN_TSARRAY_RESERVED] << enderr;
			ret = 1;
		}

		entry = (void **)entry[OMN_TSARRAY_NEXT];
	}

	return ret;
}


// 
// It initializes the array to the value 'value'. 'value' is the default
// value when the array is initialized.
//
int inline OmnTSArray_init(void ***array, 
						 const int dimension, 
						 void *defaultValue)
{
	// 
	// The array is arranged as follows:
	//		array[0..fan-1]		working pointers
	//		array[fan]			the array's dimension
	//		array[fan+1]		the head of inserted list (not used now)
	//		array[fan+2]		the head of entry list
	//		array[fan+3]		first dimension's default pointer (also the default value)
	//		array[fan+4]		second dimension's default pointer
	//		array[fan+5]		third dimension's default pointer 
	//		array[fan+6]		fourth dimension's default pointer (in general, for 
	//							dimension i, its default pointer is 'fan + dimension + 1', 
	//							where dimension counts from 1 (the rightmost dimension). 
	//							For 4-dimensional array, for instance:
	//								array[i]          = array[fan+4+1]
	//								array[i][j]       = array[fan+3+1]
	//								array[i][j][k]    = array[fan+2+1]
	//								array[i][j][k][m] = array[fan+1+1]
	//		...
	//
	void *lowerLevelPtr = 0;
	void **theArray = OmnNew void *[OMN_TSARRAY_DEFAULT+dimension];

	int i, j;

	if (!theArray)
	{
		OmnAlarm << "Internal Error" << enderr;
		// return aosAlarm(eAosAlarmMemErr);
		return eAosAlarmMemErr;
	}

	// The dimension
	theArray[OMN_TSARRAY_FAN] = (void *)dimension;
	theArray[OMN_TSARRAY_INSERTED] = 0;
	theArray[OMN_TSARRAY_ENTRYHEAD] = 0;
	theArray[OMN_TSARRAY_ENTRIES] = 0;

	// Set the default value
	theArray[OMN_TSARRAY_DEFAULT] = defaultValue;

	// 
	// Create the default pointers
	//
	lowerLevelPtr = defaultValue;
	for (i=0; i<dimension-1; i++)
	{
		void **dft = OmnNew void *[OMN_TSARRAY_ENTRYSIZE];
		if (!dft)
		{
			OmnAlarm << "Internal error!" << enderr;
			// return aosAlarm(eAosAlarmMemErr);
			return eAosAlarmMemErr;
		}

		((int*)dft)[OMN_TSARRAY_DIMENSION] = i + 1;
		((int*)dft)[OMN_TSARRAY_REFERENCE] = 0;
		((int*)dft)[OMN_TSARRAY_FLAGS] = 0;
		((int*)dft)[OMN_TSARRAY_PREV] = 0;
		((int*)dft)[OMN_TSARRAY_NEXT] = 0;

		for (j=0; j<OMN_TSARRAY_FAN; j++)
		{
			dft[j] = lowerLevelPtr;
		}
		
		theArray[OMN_TSARRAY_DEFAULT+1+i] = (void *)dft;
		lowerLevelPtr = theArray[OMN_TSARRAY_DEFAULT+1+i];
	}

	// 
	// Set all working pointers to the top dimension's default pointer
	//
	for (i=0; i<OMN_TSARRAY_FAN; i++)
	{
		theArray[i] = OmnTSArrayDft(theArray, dimension);
	}
	*array = theArray;
	return 0;
}


int inline OmnTSArray_destroy(void ***array)
{
	void **next;
	void **entry = array[OMN_TSARRAY_ENTRYHEAD];
	while (entry)
	{
		next = (void **)entry[OMN_TSARRAY_NEXT];
		OmnDelete [] entry;
		entry = next;
	}

	array[OMN_TSARRAY_ENTRYHEAD] = 0;
	array[OMN_TSARRAY_ENTRIES] = 0;
	return 0;
}


void OmnTSArray_dump(void ***array)
{
	void **entry = array[OMN_TSARRAY_ENTRYHEAD];
	int dimension = (int)array[OMN_TSARRAY_DIMENSION];
	void *working = (void*)array[0];
	int startIdx = 0;
	int i;
	for (i=256; i<OMN_TSARRAY_DEFAULT+dimension; i++)
	{
		// cout << "Array: " << dec << i << "=" << hex << array[i] << endl;
		cout << "Array: " << OmnSetbase10 << i << "=" << hex << array[i] << endl;
	}

	entry = (void **)array;
	for (i=0; i<OMN_TSARRAY_FAN; i++)
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

	entry = array[OMN_TSARRAY_ENTRYHEAD];
	while (entry)
	{
		cout << "Entry: " << hex << entry << ": "
			<< dec << (unsigned int)entry[OMN_TSARRAY_REFERENCE] << "\t";
		working = entry[0];
		startIdx = 0;
		int i;
		for (i=0; i<OMN_TSARRAY_FAN; i++)
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

		entry = (void **)entry[OMN_TSARRAY_NEXT];
	}

	// cout << "Max list size: " << tmpMaxListSize << endl;
}


int OmnTSArray_insert(void * **array,
					 unsigned char *index, 
					 void * value, 
					 int isDelete)
{
	int dimension = (int)array[OMN_TSARRAY_DIMENSION];
	int dim = dimension;
	int theIdx;
	void ** list[OMN_TSARRAY_LISTSIZE];
	int listIdx = 1;
	void ** crtEntry;
	void * *dftPtr;
	void * dftValue = OMN_TSARRAY_DFT(array);

	list[0] = (void **)array;

	// 
	// Entry is added into the entry list. Next, modify the array.
	// 'dimension' starts from the leftmost.
	// The rightmost two dimensions (dimension 1 and 2) are processed differently. 
	//
	while (listIdx)
	{
		crtEntry = list[listIdx-1];
		dim = OMN_TSARRAY_DIM(crtEntry);
		theIdx = index[dimension-dim];
		if (dim == 1)
		{
			if (value != dftValue)
			{
				if (crtEntry[theIdx] == dftValue)
				{
					((int*)crtEntry)[OMN_TSARRAY_NONDEFAULT]++;
				}
				crtEntry[theIdx] = value;
				return 0;
			}

			// 
			// The current value is default. Check whether the new value is
			// default. 
			//
			if (crtEntry[theIdx] != dftValue)
			{
				((int*)crtEntry)[OMN_TSARRAY_NONDEFAULT]--;
			}

			crtEntry[theIdx] = dftValue;
			listIdx--;
			break;
		}

		dftPtr     = OmnTSArrayDft(array, dim);
		if (crtEntry[theIdx] == dftPtr)
		{
			void **newEntry;

			if (isDelete)
			{
				return 0;
			}

			// 
			// Found a new segment, but the current one has not finished yet.
			//
			newEntry = (void **)OmnNew void **[OMN_TSARRAY_ENTRYSIZE];
			if (!newEntry)
			{
				OmnAlarm << "Internal error!" << enderr;
				// OmnUtilTesterAlarm << enderr;
				// return aosAlarmInt(eAosAlarmMemErr, (int)array);
				return eAosAlarmMemErr;
			}

			((int*)newEntry)[OMN_TSARRAY_FLAGS] = 0;
			OMN_TSARRAY_ADDENTRY(array, newEntry);
			memcpy(newEntry, dftPtr, sizeof(void **[OMN_TSARRAY_FAN]));
			OMN_TSARRAY_DIM(newEntry) = dim-1;
			OMN_TSARRAY_REF(newEntry) = 1;
			((int*)newEntry)[OMN_TSARRAY_NONDEFAULT] = 0;
			
			crtEntry[theIdx] = newEntry;
			if (crtEntry != (void **)array)
			{
				((int*)crtEntry)[OMN_TSARRAY_NONDEFAULT]++;
			}
			list[listIdx++] = newEntry;
			continue;
		}
		else
		{
			list[listIdx++] = (void **)crtEntry[theIdx];
		}
	}

	while (listIdx)
	{
		crtEntry = list[listIdx-- - 1];
		dim = OMN_TSARRAY_DIM(crtEntry);
		theIdx = index[dimension-dim];
		if (((int*)crtEntry[theIdx])[OMN_TSARRAY_NONDEFAULT] > 0)
		{
			return 0;
		}

		// 
		// The pointed to contains no more non-default elements. 
		// Need to remove the entry. 
		//
		void **ee = (void **)crtEntry[theIdx];
		OMN_TSARRAY_SETDFTPTR(array, crtEntry, theIdx);
		if (OMN_TSARRAY_REF(ee) == 0)
		{
			// 
			// No one points to the entry anymore. Delete it.
			//
			OMN_TSARRAY_REMOVEENTRY(array, ee);
			OmnDelete [] ee;
		}
	}

	return 0;
}


int inline OmnTSArray_remove(void * **array, unsigned char *index)
{
	return OmnTSArray_insert(array, index, OMN_TSARRAY_DFT(array), 1);
}

#endif

