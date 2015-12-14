////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array4.h
// Description:
//	The array is arranged as follows:
//		array[0..OMN_ARRAY4_FAN-1]		working pointers
//		array[OMN_ARRAY4_FAN]			count reference
//		array[OMN_ARRAY4_FAN+1]			the head of entry list
//		array[OMN_ARRAY4_FAN+2...+3]	default pointers
//
//	Following are the constants:
//		OMN_ARRAY4_LEVEL		The size of array (levels)
//		OMN_ARRAY4_DFSTART		The start of the default pointers
//		OMN_ARRAY4_TOTAL		The total entries in array[...]
//		OMN_ARRAY4_ENTRYLIST	The head of the entry list
//		OMN_ARRAY4_ENTRYSIZE	The size of entries for non-top levels.
//		OMN_ARRAY4_REF			The position where the reference is tored
//		OMN_ARRAY4_FAN			The number of branches in the tree
//		
//	The array creates three default pointers:
//		OmnArray4Dft1 is an array[OMN_ARRAY4_FAN], each points to the default value
//		OmnArray4Dft2 is an array[OMN_ARRAY4_FAN], each points to OmnArrayDft1
//		OmnArray4Dft3 is an array[OMN_ARRAY4_FAN], each points to OmnArrayDft2
//	When the array is initialized, the top level should all point to OmnArray4Dft3.
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_Array4_h
#define Omn_Util_Array4_h

#include "KernelUtil/KernelAlarm.h"


#define OMN_ENTRY_ALREADY_EXIST	1
#define OMN_SUCCESS				0

#define OMN_ARRAY4_LEVEL		4
#define OMN_ARRAY4_DFSTART		258
#define OMN_ARRAY4_TOTAL		261
#define OMN_ARRAY4_ENTRYLIST	257
#define OMN_ARRAY4_ENTRYSIZE	257
#define OMN_ARRAY4_REF			256
#define OMN_ARRAY4_FAN			256


#ifndef OmnArray4Dft1
#define OmnArray4Dft1(T, array) ((T*)array[OMN_ARRAY4_DFSTART])
#endif

#ifndef OmnArray4Dft2
#define OmnArray4Dft2(T, array) ((T**)array[OMN_ARRAY4_DFSTART+1])
#endif

#ifndef OmnArray4Dft3
#define OmnArray4Dft3(T, array) ((T***)array[OMN_ARRAY4_DFSTART+2])
#endif


template <class T>
struct OmnArray4CtlEntry
{
	OmnArray4CtlEntry *		mPrev;
	OmnArray4CtlEntry *		mNext;
	unsigned char			mIndex[OMN_ARRAY4_LEVEL];
	unsigned char			mMask[OMN_ARRAY4_LEVEL];
	T						mValue;
};


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
int OmnArray4AddEntry(T ****array, 
					  unsigned char *index, 
					  unsigned char *mask, 
					  T value)
{
	OmnArray4CtlEntry<T> *prev = 0;
	OmnArray4CtlEntry<T> *entry = (OmnArray4CtlEntry<T>*)array[OMN_ARRAY4_ENTRYLIST];
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


// 
// It initializes the array to the value 'value'. 'value' is the default
// value when the array is initialized.
//
template <class T>
bool inline OmnArray41Init(T ****array, T value)
{
	// 
	// 'array' is a pointer to 
/*	int i, j;
	T *** lowerLevelPtr = 0;

	// 
	// Create the default pointers
	//
	for (i=0; i<OMN_ARRAY4_LEVEL-1; i++)
	{
		array[OMN_ARRAY4_DFSTART+i]   = (T ***)OmnNew T[OMN_ARRAY4_FAN];
		for (j=0; j<OMN_ARRAY4_FAN; j++)
		{
			array[OMN_ARRAY4_DFSTART+i][j] = lowerLevelPtr;
		}

		lowerLevelPtr = array[OMN_ARRAY4_DFSTART+i];
	}

	for (i=0; i<OMN_ARRAY4_FAN; i++)
	{
		OmnArray4Dft1(T, array)[i] = value;
		OmnArray4Dft2(T, array)[i] = OmnArray4Dft1(T, array);
		OmnArray4Dft3(T, array)[i] = OmnArray4Dft2(T, array);
		array[i] = OmnArray4Dft3(T, array);
	}

	array[OMN_ARRAY4_ENTRYLIST] = 0;
*/
	return true;
}


template <class T>
int OmnArray4DeleteEntry(T ****array, 
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


template <class T>
int OmnArray4CopyEntry(T ****array, 
					   T **toEntry, 
					   T **fromEntry, 
					   T *dftPtr,
					   unsigned int size)
{
	memcpy(toEntry, fromEntry, size);
	for (int i=0; i<256; i++)
	{
		if (toEntry[i] != dftPtr && toEntry[i] != 0)
		{
			unsigned int *v = (unsigned int *)&toEntry[i][OMN_ARRAY4_REF];
			*v = ((unsigned int)toEntry[i][OMN_ARRAY4_REF]) + 1;
		}
	}

	return 0;
}


template <class T>
bool inline OmnArray41Destroy(T ****array)
{
	// Not implemented yet
	return true;
}


/*
template <class T>
int inline OmnArray41Insert(T ****array,
						   unsigned char *index, 
						   unsigned char *mask, 
						   const T &value, 
						   const T &dft)
{
	// 
	// We will create a series of pointers:
	//     Ptr3 -> Ptr2 -> Ptr1 -> value
	// where "Ptri" is a pointer at level i. At each level, there can be 
	// multiple pointers pointing to the same next level Ptri, depending
	// on the netmask. In other word, if the netmask is 0xff, there will be
	// only one pointer to be modified. In general, there will be ~netmask + 1
	// number of pointers to be modified. For instance, if the netmask is
	// 0xf0, the number of pointers to modify is ~0xf0+1, or 0x0f+1, or 16. 
	// 
	// When replacing a pointer, we need to check whether the old pointed to
	// needs to be deleted. This is determined by checking the reference 
	// field. 
	// 
	int level = OMN_ARRAY4_LEVEL-1;
	int idx, i;
	T *dftPtr = 0;				// The default pointer.
	T **list1[10000];
	list1[0] = (T**)array;
	int list1Idx = 1;
	unsigned char min = 0;
	unsigned char max = 0;

	// 
	// Make sure the mask is correct.
	//
	if (!aosIsValidArrayMask(mask, OMN_ARRAY4_LEVEL))
	{
		return aosAlarm(eAosAlarmInvalidArrayMask);
	}

	// 
	// Add the entry into the entry list
	//
	int rslt = OmnArray4AddEntry(array, index, mask, value);
	switch (rslt)
	{
	case OMN_ENTRY_ALREADY_EXIST:
		 return 0;

	case OMN_SUCCESS:
		 break;

	default:
		 OmnKernelAlarm("Unrecognized return value");
		 return rslt;
	}

	// 
	// Entry is added into the entry list. Next, modify the array.
	// 'level' starts at OMN_ARRAY4_LEVEL-1. The lowest level is 0. 
	// The last two levels are processed differently. 
	//
	while (level > 1)
	{
		T **newPtr = 0;				// The new pointer to be created.
		T **reusePtr = 0;
		T **list2[10000];
		int list2Idx = 0;

		dftPtr = (T*)array[OMN_ARRAY4_DFSTART+level-1];
		T *dftPtrNext = (T*)array[OMN_ARRAY4_DFSTART+level-2];
		min = index[3-level] & mask[3-level];
		max = index[3-level] | (~mask[3-level]);

		for (idx=0; idx<list1Idx; idx++)
		{
			T *entry = 0;
			int startIdx = min;
			for (i=min; i<=max; i++)
			{
				if (list1[idx][i] == 0 || list1[idx][i] == dftPtr)
				{
					if (entry)
					{
						// End a segment
						reusePtr = OmnNew T*[OMN_ARRAY4_ENTRYSIZE];
						OmnArray4CopyEntry(array, (T**)reusePtr, (T**)entry, (T*)dftPtrNext, 
							sizeof(T*[OMN_ARRAY4_ENTRYSIZE]));
						reusePtr[OMN_ARRAY4_REF] = (T*)(unsigned int)(i-startIdx);
						list2[list2Idx++] = reusePtr;
						for (int k=startIdx; k<i; k++)
						{
							OmnArray4DeleteEntry(array, level, (T**)list1[idx][k]);
							list1[idx][k] = (T*)reusePtr;
						}
						entry = 0;
					}

					if (newPtr == 0)
					{
						newPtr = OmnNew T*[OMN_ARRAY4_ENTRYSIZE];
						newPtr[OMN_ARRAY4_REF] = 0;
						for (int k=0; k<256; k++)
						{
							newPtr[k] = 0;
						}

						list2[list2Idx++] = newPtr;
					}
						
					list1[idx][i] = (T*)newPtr;
					newPtr[OMN_ARRAY4_REF] = (T*)((unsigned int)newPtr[OMN_ARRAY4_REF] + 1);
					entry = 0;
					continue;
				}

				if (entry == 0)
				{
					// Start a new segment
					entry = list1[idx][i];
					startIdx = i;
					continue;
				}

				if (entry != list1[idx][i])
				{
					// End a segment
					reusePtr = OmnNew T*[OMN_ARRAY4_ENTRYSIZE];
					OmnArray4CopyEntry(array, (T**)reusePtr, (T**)entry, level, (T*)dftPtrNext, 
							sizeof(T*[OMN_ARRAY4_ENTRYSIZE]));
					reusePtr[OMN_ARRAY4_REF] = (T*)(unsigned int)(i-startIdx);
					list2[list2Idx++] = reusePtr;
					for (int k=startIdx; k<i; k++)
					{
						OmnArray4DeleteEntry(array, level, (T**)list1[idx][k]);
						list1[idx][k] = (T*)reusePtr;
					}

					startIdx = i;
					entry = list1[idx][i];
					continue;
				}
			}

			if (entry)
			{
				// This is the last segment
				reusePtr = OmnNew T*[OMN_ARRAY4_ENTRYSIZE];
				OmnArray4CopyEntry(array, (T**)reusePtr, (T**)entry, level, (T*)dftPtrNext, 
							sizeof(T*[OMN_ARRAY4_ENTRYSIZE]));
				reusePtr[OMN_ARRAY4_REF] = (T*)(unsigned int)(max-startIdx+1);
				list2[list2Idx++] = reusePtr;
				for (int k=startIdx; k<=max; k++)
				{
					OmnArray4DeleteEntry(array, level, (T**)list1[idx][k]);
					list1[idx][k] = (T*)reusePtr;
				}
			}

			// Need to go over one more time to replace all defPtr and null ptrs
			for (i=0; i<256; i++)
			{
				if (list1[idx][i] == 0)
				{
					list1[idx][i] = dftPtr;
				}
			}
		}

		for (list1Idx=0; list1Idx<list2Idx; list1Idx++)
		{
			list1[list1Idx] = list2[list1Idx];
		}

		list2Idx = 0;
		level--;
	}

	T *list3[10000];
	int list3Idx = 0;
	for (idx=0; idx<list1Idx; idx++)
	{
		T *newPtr1 = 0;
		T *reusePtr1 = 0;
		dftPtr = (T*)array[OMN_ARRAY4_DFSTART];
		min = index[2] & mask[2];
		max = index[2] | (~mask[2]);

		for (idx=0; idx<list1Idx; idx++)
		{
			T *entry = 0;
			int startIdx = 0;
			for (i=min; i<=max; i++)
			{
				if (list1[idx][i] == 0 || list1[idx][i] == dftPtr)
				{
					if (entry)
					{
						// End a segment
						reusePtr1 = OmnNew T[OMN_ARRAY4_ENTRYSIZE];
						memcpy(reusePtr1, entry, sizeof(T[OMN_ARRAY4_ENTRYSIZE]));
						reusePtr1[OMN_ARRAY4_REF] = (T)(unsigned int)(i-startIdx);
						list3[list3Idx++] = reusePtr1;
						for (int k=startIdx; k<i; k++)
						{
							OmnArray4DeleteEntry(array, 1, (T**)list1[idx][k]);
							list1[idx][k] = (T*)reusePtr1;
						}
						entry = 0;
					}

					if (newPtr1 == 0)
					{
						newPtr1 = OmnNew T[OMN_ARRAY4_ENTRYSIZE];
						newPtr1[OMN_ARRAY4_REF] = (T)(unsigned int)0;
						list3[list3Idx++] = newPtr1;
						for (int k=0; k<256; k++)
						{
							newPtr1[k] = dft;
						}
					}

					list1[idx][i] = (T*)newPtr1;
					newPtr1[OMN_ARRAY4_REF] = (T)((unsigned int)newPtr1[OMN_ARRAY4_REF] + 1);
					continue;
				}

				if (entry == 0)
				{
					// Start a new segment
					entry = list1[idx][i];
					startIdx = i;
					continue;
				}

				if (entry != list1[idx][i])
				{
					// End a segment
					reusePtr1 = OmnNew T[OMN_ARRAY4_ENTRYSIZE];
					memcpy(reusePtr1, entry, sizeof(T[OMN_ARRAY4_ENTRYSIZE]));
					list3[list3Idx++] = reusePtr1;
					reusePtr1[OMN_ARRAY4_REF] = (T)(unsigned int)(i - startIdx);
					for (int k=startIdx; k<i; k++)
					{
						OmnArray4DeleteEntry(array, 1, (T**)list1[idx][k]);
						list1[idx][k] = (T*)reusePtr1;
					}

					startIdx = i;
					entry = list1[idx][i];
					continue;
				}
			}

			if (entry)
			{
				// This is the last segment
				reusePtr1 = OmnNew T[OMN_ARRAY4_ENTRYSIZE];
				memcpy(reusePtr1, entry, sizeof(T[OMN_ARRAY4_ENTRYSIZE]));
				reusePtr1[OMN_ARRAY4_REF] = (T)(unsigned int)(max - startIdx + 1);
				list3[list3Idx++] = reusePtr1;
				for (int k=startIdx; k<=max; k++)
				{
					OmnArray4DeleteEntry(array, 1, (T**)list1[idx][k]);
					list1[idx][k] = (T*)reusePtr1;
				}
			}

			// Need to go over one more time to replace all defPtr and null ptrs
			for (i=0; i<256; i++)
			{
				if (list1[idx][i] == 0)
				{
					list1[idx][i] = dftPtr;
				}
			}
		}
	}

	min = index[3] & mask[3];
	max = index[3] | (~mask[3]);
	for (idx=0; idx<list3Idx; idx++)
	{
		for (int i=min; i<=max; i++)
		{
			list3[idx][i] = value;
		}
	}

	return 0;
}
*/

/*
template <class T>
int inline OmnArray41Remove(T ****array,
						   unsigned char *index, 
						   unsigned char *mask, 
						   T dft)
{
	// 
	// 1. For each given (index, mask), there should be 
	// 1. Start from the last second level. For each matched entry at that
	//    level, reset the value in the pointed to bottom level. If the
	//    bottom level contains nothing but the default value, that bottom
	//    level should be removed (but do not delete it yet since other
	//    entries may point to the same bottom level. We will delete it 
	//    after we finish the last second level).
	// 2. Check whether the entry contains pointers pointing to non-default
	//    next level. If not, 
	int level = 3;
	int idx;
	T **nextLevel = 0;			// Points to the next level.
	T **prevEntry = 0;
	T **nextEntry = 0;
	T *dftPtr = 0;				// The default pointer.
	T **list1[10000];
	T **list2[10000];

	OmnArrayCtlEntry<T> *entry = OmnArray4GetEntry(array, index, mask, value);
	if (!entry)
	{
		// Entry not defined. 
		return -1;
	}

	unsigned char min = index[0] & mask[0];
	unsigned char max = index[0] | (~mask[0]);
	T *entry  = (T**)array;
	list1[0] = (T**)array;
	int list1Idx = 1;
	while (level > 0)
	{
		list2Idx = 0;
		dftPtr = (T*)array[OMN_ARRAY4_DFSTART+byte-1];
		min = index[3-byte] & mask[3-byte];
		max = index[3-byte] | (~mask[3-byte]);
		for (idx=0; idx<list1Idx; idx++)	// Loop on 'list1'
		{
			for (int i=min; i<=max; i++)
			{
				//
				// Need to remove list1[idx][i]:
				// 1. Check whether list1[idx][i] is in the same range as that of
				//	  list1[idx]i-1] or list1[idx][i+1]. If yes, modify list[idx][i]
				//    to point to the same next level. If not, point to the default.
				// 2. Decrement the reference of the pointed to next level.
				// 3. If the reference becomes 0, delete the next level.
				//
				OmnArray4RemoveEntry(array, list1[idx][i], dftPtr, list2);
			}
		}

		for (list1Idx=0; list1Idx<list2Idx; list1Idx++)
		{
			list1[list1Idx] = list2[list1Idx];
		}
		level--;
	}

	unsigned char min = index[1] & mask[1];
	unsigned char max = index[1] | (~mask[1]);
	dftPtr = (T*)array[OMN_ARRAY4_DFSTART];

	T *newPtr1 = 0;
	T *list3[10000];
	int list3Idx = 0;
	for (idx=0; idx<list1Idx; idx++)
	{
		for (int i=0; i<=255; i++)
		{
			if (i>=min && i<=max && list1[idx][i] == dftPtr || list1[idx][i] == 0)
			{
				if (newPtr1 == 0)
				{
					if (sizeof(T) < OMN_CTLSIZE)
					{
						newPtr1 = OmnNew T[OMN_ARRAY_NEWSIZE_LARGE];
					}
					else
					{
						newPtr1 = OmnNew T[OMN_ARRAY4_NEWSIZE];
					}

					((char*)newPtr1)[0] = 0;
					((char*)newPtr1)[1] = index[1];
					((char*)newPtr1)[2] = mask[1];

					list3[list3Idx++] = newPtr1;
					for (int k=0; k<256; k++)
					{
						newPtr1[k] = dft;
					}
				}

				if (i>=min && i<=max)
				{
					list1[idx][i] = newPtr1;
					((char*)newPtr1)[0]++;
				}
				else
				{
					list1[idx][i] = dftPtr;
				}
			}
			else if (i>=min && i<=max)
			{
				list3[list3Idx++] = list1[idx][i];
			}
		}
	}

	min = index[0] & mask[0];
	max = index[0] | (~mask[0]);
	for (idx=0; idx<list3Idx; idx++)
	{
		for (int i=min; i<=max; i++)
		{
			list3[idx][i] = value;
		}
	}

	return 0;
}


template <class T>
int OmnArray4GetChain(T**** array, 
					  unsigned char *index, 
					  unsigned char *mask)
{
	int level = 3;
	T ** list1[4];
	unsigned char min, max;

	while (level > 0)
	{
		min = index[level] & mask[level];
		max = index[level] & (~mask[level]);
		list2Idx = 0;

		for (int idx=0; idx<list1Idx; idx++)
		{
			T ** crtEntry = list1[idx][level];
			for (int i=min; i<=max; i++)
			{
				T **ptr = (T**)crtEntry[i];

				// If not in list2 yet, put it in.
				for (int k=0; k<list2Idx; k++)
				{
					if (ptr == list2[k][level-1])
					{
						break;
					}
				}

				if (k < list2Idx)
				{
					// Not in list2 yet. Put it in.
					for (int m=0; m<4; m++)
					{
						list2[list2Idx][m] = list1[idx][m];
					}
					list2[list2Idx++][level-1] = ptr;
				}
			}
		}

		// Move all elements from list2 to list1
		for (int i=0; i<list2Idx; i++)
		{
			for (int m=0; m<4; m++)
			{
				list1[i][m] = list2[i][m];
			}
		}

		list2Idx = 0;
	}

	// Now, list1 contains all the paths
	return 0;
}
*/


#endif

