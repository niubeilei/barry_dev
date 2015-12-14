////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TArray.h
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
//		entry[0..OMN_TARRAY_FAN-1]		working pointers
//		entry[OMN_TARRAY_FAN]			dimension
//		entry[OMN_TARRAY_REFERENCE]		reference
//		entry[OMN_TARRAY_FLAGS]			only 0x01 is used 
//		entry[OMN_TARRAY_PREV]			pointer to previous
//		entry[OMN_TARRAY_NEXT]			pointer to next
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

#ifndef Omn_KernelUtil_TArray_h
#define Omn_KernelUtil_TArray_h

// #include "Alarm/Alarm.h"
#include "aos/aosKernelAlarm.h"
#include "KernelUtil/KernelAlarm.h"
#include "KernelUtil/Netmask.h"

#define OMN_ENTRY_ALREADY_EXIST	1
#define OMN_SUCCESS				0
#define OMN_TARRAY_FAN			256
 
// Constants for array
#define OMN_TARRAY_DIMENSION	256		// The array's dimension
#define OMN_TARRAY_INSERTED		257		// List of inserted items. Not used.
#define OMN_TARRAY_ENTRYHEAD	258		// A list of all entries created so far
#define OMN_TARRAY_ENTRIES		259		// Number of entries
#define OMN_TARRAY_DEFAULT		260		// The array's default value

// Constants for entries
#define OMN_TARRAY_REFERENCE	257		// The reference count
#define OMN_TARRAY_FLAGS		258		// 0x01: all default values/pointers
#define OMN_TARRAY_PREV			259		// Pointer to the previous entry
#define OMN_TARRAY_NEXT			260		// Pointer to the next entry
#define OMN_TARRAY_RESERVED		261		// Reserved
#define OMN_TARRAY_ENTRYSIZE	262		// The entry size

// Flags
#define OMN_TARRAY_DEFAULTFLAG	0x01

#define OMN_TARRAY_LISTSIZE		5000

#ifndef OmnTArrayDft
#define OmnTArrayDft(array, dimension) ((void **)array[OMN_TARRAY_DEFAULT+dimension-1])
#endif

#ifdef AOS_KERNEL_SIMULATE
#include <stdlib.h>
#include <stdio.h>
extern int aosTotalUtilTestAlarms;
#else
#define OmnUtilTesterAlarm //
#endif

#define OMN_TARRAY_REF(entry) ((int*)entry)[OMN_TARRAY_REFERENCE]
#define OMN_TARRAY_DIM(entry) ((int*)entry)[OMN_TARRAY_DIMENSION]
#define OMN_TARRAY_DFT(array) (void *)array[OMN_TARRAY_DEFAULT]

#define OMN_TARRAY_IS_DEFAULT(array, entry, i) \
	(entry[i] == OmnTArrayDft(array, OMN_TARRAY_DIM(entry)))

#define OMN_TARRAY_ISENTRYDELETED(entry) \
	(((int*)entry)[OMN_TARRAY_DIMENSION] == -1)

#define OMN_TARRAY_SETENTRYDELETED(entry) \
	((int*)entry)[OMN_TARRAY_DIMENSION] = -1;

#define OMN_TARRAY_SETDFTFLAG(entry) \
	((int*)entry)[OMN_TARRAY_FLAGS] |= OMN_TARRAY_DEFAULTFLAG

#define OMN_TARRAY_ISALLDEFAULT(entry) \
	((int*)entry)[OMN_TARRAY_FLAGS] & OMN_TARRAY_DEFAULTFLAG

#define OMN_TARRAY_ADDENTRY(array, entry) \
	((void **)entry)[OMN_TARRAY_NEXT] = (void *)array[OMN_TARRAY_ENTRYHEAD]; \
	((void **)entry)[OMN_TARRAY_PREV] = 0; \
	if (array[OMN_TARRAY_ENTRYHEAD]) \
		((void **)array[OMN_TARRAY_ENTRYHEAD])[OMN_TARRAY_PREV] = (void *)entry; \
	((void **)array)[OMN_TARRAY_ENTRYHEAD] = (void *)entry; \
	((int *)array)[OMN_TARRAY_ENTRIES]++;

#define OMN_TARRAY_REMOVEENTRY(array, entry) \
	if (entry[OMN_TARRAY_PREV]) \
		((void **)entry[OMN_TARRAY_PREV])[OMN_TARRAY_NEXT] = (void *)entry[OMN_TARRAY_NEXT]; \
	if (entry[OMN_TARRAY_NEXT]) \
		((void **)entry[OMN_TARRAY_NEXT])[OMN_TARRAY_PREV] = (void *)entry[OMN_TARRAY_PREV]; \
	if ((void **)array[OMN_TARRAY_ENTRYHEAD] == (void **)entry) \
		((void **)array)[OMN_TARRAY_ENTRYHEAD] = (void *)entry[OMN_TARRAY_NEXT]; \
	entry[OMN_TARRAY_NEXT] = 0; \
	entry[OMN_TARRAY_PREV] = 0; \
	((int *)array)[OMN_TARRAY_ENTRIES]--;

#define OMN_TARRAY_SETDFTPTR(array, entry, idx) \
	((int*)entry[idx])[OMN_TARRAY_REFERENCE]--; \
	entry[idx] = OmnTArrayDft(array, OMN_TARRAY_DIM(entry))

struct OmnEntryToAdd
{
	int				mCrt;
	void **			mEntry;
	void **			mNewEntry;
};


// 
// If it is dimension 1, simply assign it. Otherwise, decrement
// the reference of the pointed to entry and then assign it.
//
#define OMN_TARRAY_SETPTR(xEntry, xIndex, xNextPtr) \
	if (OMN_TARRAY_DIM(xEntry) > 1) \
	{ \
		OMN_TARRAY_REF(xEntry[xIndex])--; \
	} \
	xEntry[xIndex] = (void**)xNextPtr; 

extern int OmnTArray_checkArray(void ***array);
extern int OmnTArray_getNumEntries(void ***array);
extern int OmnTArray_deleteEntry(void ***array, void **toBeDeleted);
extern int OmnTArray_init(void ***array, 
						 const int dimension, 
						 void *defaultValue);
extern int OmnTArray_copyEntry(void **toEntry, void **fromEntry, unsigned int size);
extern int OmnTArray_destroy(void ***array);
extern int OmnTArray_deleteSegment(void ***array, 
								   int segmentStart, 
								   int segmentEnd,
								   void ***crtEntry,
								   void **workingEntry, 
								   int dim, 
								   void ****list, 
								   int *listIdx, 
								   int crtEnd);
extern void OmnTArray_dump(void ***array);
extern int  OmnTArray_addSegment1(void ***array, 
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
								 void *value);
extern int OmnTArray_procLastSegment(void ***array, 
								   int min, 
								   int max, 
								   int nextMin,
								   struct OmnEntryToAdd *list, 
								   struct OmnEntryToAdd *crtEntry,
								   int *listIdx, 
								   void **dftPtr, 
								   int dim);
extern int OmnTArray_finishSegment(void ***array, 
								   int min, 
								   int max,
								   void **dftPtr,
								   int dim,
								   int nextmin,
								   struct OmnEntryToAdd *list,
								   int *listIdx,
								   struct OmnEntryToAdd *crtEntry, 
								   int isDelete);
extern int OmnTArray_insert1(void * **array,
					  unsigned char *index, 
					  unsigned char *mask, 
					  void * value, 
					  int isDelete);
static inline int OmnTArray_remove1(void * **array,
						    unsigned char *index, 
						    unsigned char *mask)
{
	return OmnTArray_insert1(array, index, mask, OMN_TARRAY_DFT(array), 1);
}

extern void OmnTArray_getSlab(int *numIdle, int *numAllocated);
#endif

