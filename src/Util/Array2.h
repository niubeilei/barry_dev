////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array2.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_Array2_h
#define Omn_Util_Array2_h

#include "Util/OmnNew.h"

template <class T>
struct OmnArray2
{
	T *		mDefaultLevel1;
	T **	mData;
};

template <class T>
bool inline OmnArray2Init(OmnArray2<T> *array, T value)
{
	array->mDefaultLevel1 = OmnNew T[256];
	array->mData          = OmnNew T*[256];

	int i;
	for (i=0; i<256; i++)
	{
		array->mDefaultLevel1[i] = value;
		array->mData[i]          = array->mDefaultLevel1;
	}

	return true;
}


template <class T>
bool inline OmnArray2Destroy(OmnArray2<T> &array)
{
	// Not implemented yet
	return true;
}


template <class T>
int inline OmnArray2Insert(OmnArray2<T> *array,
						   unsigned char *index, 
						   char *netmask, 
						   int num,
						   T &value)
{
	// 
	// It inserts the element "value" into the array. 
	// The 'index' is arranged that index[0] is the least significant and
	// index[1] is the most significant. 
	// 'netmask' must be a network mask, which is in the form of all 1's 
	// followed by all 0's. 
	// 
	// There is a maximum number of elements to create. If it exceeds the 
	// maximum, ERROR_TOO_MANY_ELEMENTS is returned. 
	// 
	// If memory allocation failed, it returns ERROR_MEMORY_ERROR.
	// 
	// Otherwise, it returns 0.
	// 
	// If index/netmask overlaps with some of existing elements, these elements
	// will be overridden. 
	// 
	// If index = 30.0, and netmask is 255.0, we need to check whether
	// any of array[30][i] has been instantiated. If yes, delete all but 
	// just one. We will reset the one to contain the value 'value', and make all
	// array[30][i] point to this element. 
	// 
	// In general, we consider:
	//	index:	111...111000 000...
	// There are three segments. The first one are all 1's and they are all in the 
	// whole bytes. The second segment is in a byte that contains both 1's and 0's. 
	// The third segment are all 0's. 
	// Below is an example:
	//	index:	11111111 11100000 00000000 00000000
	// In this case, Segment 1 is the first byte, the second segment is the second
	// byte, and the remaining two bytes are segment 3. 
	// 
	// In this case, we need to create a T** and let array[i][j] point to that 
	// element. If array[i][j] is the default, we need to create a new element
	// to replace it. If array[i][j] is not pointing to the default, need to delete
	// all elements the elemnt points to, and then initializes it to point to 
	// 'value'. 
	if (netmask[3] != 0xff)
	{
		// 
		// Create a new element
		//
		T *** newElem = OmnArray4CreateLevel3Elem(value);

		for (int i=(index[3]&netmask[3]); i<(index[3]|~netmask[3]); i++)
		{
			if (array->mDefaultLevel4[i] != mDefaultLevel3)
			{
				// 
				// If no one is using it anymore, delete it. Otherwise, ignore.
				//
				OmnArray4CheckAndDeleteLevel3Elem(array->mDefaultLevel4[i]);
			}
			array->mDefaultLevel4[i] = newElem;
		}

		return true;
	}

	if (netmask[2] != 0xff)
	{
		T ** newElem = OmnArray4CreateLevel2Elem(value);

		if (array->mDefaultLevel4[index[3]] != array->mDefaultLevel3)
		{
			OmnArray4CheckLevel4(index[3]);
		}
		T *** elem = OmnArray4CreateLevel3Elem();

		for (int i=(index[2]&netmask[2]); i<(index[2]|~netmask[2]); i++)
		{
			if (array->mDefaultLevel4[index[3]][i] != mDefaultLevel2)
			{
				// 
				// Need to delete the element.
				//
				OmnArray4DeleteLevel2Elem(array->mDefaultLevel4[index[3]][i]);
			}
			array->mDefaultLevel4[index[3]][i] = newElem;
		}

		return true;
	}

	if (netmask[1] != 0xff)
	{
		T * newElem = OmnArray4CreateLevel1Elem(value);

		for (int i=(index[1]&netmask[1]); i<(index[1]|~netmask[1]); i++)
		{
			if (array->mDefaultLevel4[index[3]][index[2]][i] != mDefaultLevel1)
			{
				// 
				// Need to delete the element.
				//
				OmnArray4DeleteLevel1Elem(array->mDefaultLevel4[index[3]][index[2]][i]);
			}
			array->mDefaultLevel4[index[3]][index[2]][i] = newElem;
		}

		return true;
	}

	if (netmask[0] != 0xff)
	{
		for (int i=(index[0]&netmask[0]); i<(index[0]|~netmask[0]); i++)
		{
			if (array->mDefaultLevel4[index[3]][index[2]][index[1]][i] != mDefaultLevel2)
			{
				// 
				// Need to delete the element.
				//
				OmnArray4DeleteLevel2Elem(array->mDefaultLevel4[index[3]][i]);
			}
			array->mDefaultLevel4[index[3]][i] = newElem;
		}

		return true;
	}
}

#endif

