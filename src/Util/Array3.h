////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array3.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_Array3_h
#define Omn_Util_Array3_h

template <class T>
struct OmnArray3
{
	T *		mDefaultLevel1;
	T **	mDefaultLevel2;
	T ***	mData;
	T		mDefault;
};

template <class T>
bool inline OmnArray3Init(OmnArray3<T> *array, T value)
{
	array->mDefaultLevel1 = OmnNew T[256];
	array->mDefaultLevel2 = OmnNew T*[256];
	array->mData          = OmnNew T**[256];
	array->mDefault       = value;

	int i;
	for (i=0; i<256; i++)
	{
		array->mDefaultLevel1[i] = value;
		array->mDefaultLevel2[i] = array->mDefaultLevel1;
		array->mData[i]          = array->mDefaultLevel2;
	}

	return true;
}


template <class T>
bool inline OmnArray3Destroy(T ****array)
{
	// Not implemented yet
	return true;
}


template <class T>
int inline OmnArray3Insert(OmnArray3<T> *array,
						   unsigned char *index, 
						   unsigned char *mask, 
						   const T &value)
{
	// 
	// It inserts the element "value" into the array. 
	// The 'index' is arranged that index[0] is the least significant and
	// index[3] is the most significant. 
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
	// If index = 10.20.30.0, and netmask is 255.255.255.0, we need to check whether
	// any of array[10][20][30][i] has been instantiated. If yes, delete all but 
	// just one. We will reset the one to contain the value 'value', and make all
	// array[10][20][30][i] point to this element. 
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

	T **tt, *t;
	int i;
	if (mask[2] == 0)
	{
		// 
		// This is incorrect. 
		//
		OmnAlarm << "Mask is 0" << enderr;
		return -1;
	}

	int level = 3;
	char min[3], max[3];
	for (i=2; i>=0; i--)
	{
		if (mask[i] == 0)
		{
			break;
		}

		min[i] = index[i] & mask[i];
		max[i] = index[i] | (~mask[i]);
		level--;
	}

	switch (level)
	{
	case 0:		// x.x.y
		 if (array->mData[min[2]][min[1]] == array->mDefaultLevel1)
		 {
			 array->mData[min[2]][min[1]] = OmnNew T[256];
			 for (int i=0; i<256; i++)
			 {
				 if (i >= min[0] && i <= max[0])
				 {
					 array->mData[min[2]][min[1]][i] = value;
				 }
				 else
				 {
					 array->mData[min[2]][min[1]][i] = array->mDefault;
				 }
			 }
		 }
		 else
		 {
			 for (int i=min[0]; i<=max[0]; i++)
			 {
				array->mData[min[2]][min[1]][i] = value;
			 }
		 }
		 break;

	case 1:		// x.y.0
		 t = OmnNew T[256];
		 for (i=0; i<256; i++)
		 {
			 t[i] = value;
		 }

		 if (array->mData[min[2]] == array->mDefaultLevel2)
		 {
			 array->mData[min[2]] = OmnNew T*[256];
			 for (i=0; i<256; i++)
			 {
				 t[i] = value;
				 if (i>=min[1] && i<=max[1])
				 {
					 array->mData[min[2]][i] = t;
				 }
				 else
				 {
					 array->mData[min[2]][i] = array->mDefaultLevel1;
				 }
			 }
		 }
		 else
		 {
			 for (i=min[1]; i<=max[1]; i++)
			 {
				 array->mData[min[2]][i] = t;
			 }
		 }
		 break;

	case 2:		// y.0.0
		 tt = OmnNew T*[256];
		 t  = OmnNew T[256];
		 for (i=0; i<256; i++)
		 {
			 t[i] = value;
			 tt[i] = t;
		 }

		 for (i=min[2]; i<=max[2]; i++)
		 {
			 array->mData[i] = tt;
		 }
		 break;

	default:
		 OmnAlarm << "Error" << enderr;
		 return -1;
	}

	return 0;
}

#endif

