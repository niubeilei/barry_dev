////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 05/18/2012 Created by felicia
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_Sort_h
#define AOS_Util_Sort_h

#include "Util/CompareFun.h"
#define MAXNUM  16*(*comp).size

#define change(first, last, comp, tmpbuff) \
{\
	{\
		memcpy(tmpbuff, first, (*comp).size);\
		memcpy(first, last, (*comp).size);\
		memcpy(last, tmpbuff, (*comp).size);\
	}\
}

class AosSort
{
public:
	static void sort(char* first, char* last, AosCompareFun *comp, char *tmpbuff);

	// Thease all the internal function used by AosSort
	static inline char* partition(char* first,char* last,const char* pivot, AosCompareFun *comp, char *tmpbuff);

	static inline void move_first(char* a, char* b, char* c, AosCompareFun *comp, char *tmpbuff);

	static inline char* pivot(char* first,char* last, AosCompareFun *comp, char *tmpbuff);

	static inline void quick_sort(char* first, char* last, int deeplimit, AosCompareFun *comp, char *tmpbuff);

	static inline void insert(char *last, AosCompareFun *comp, char *tmpbuff);

	static inline void uninsertion_sort(char* first, char* last, AosCompareFun *comp, char *tmpbuff);

	static inline void insertion_sort(char* first, char* last, AosCompareFun *comp, char *tmpbuff);

	static inline void insert_sort(char* first, char* last, AosCompareFun *comp, char *tmpbuff);

	static inline void heap_sort(char* first, char* mid, char* last, AosCompareFun *comp, char *tmpbuff);

	static inline void sort_heap(char* first, char* last, AosCompareFun *comp, char *tmpbuff);

	static inline void pop_heap(char* first, char* last, char* result, AosCompareFun *comp, char *tmpbuff);

	static inline void heap_select(char* first, char* mid, char* last, AosCompareFun *comp, char *tmpbuff);

	static inline void make_heap(char* first, char* last, AosCompareFun *comp, char *tmpbuff);

	static inline void adjust_heap(char* first, int holeidx, int len, char* val, AosCompareFun *comp);

	static inline void push_heap(char* first, int holeidx, int topidx, char* val, AosCompareFun *comp);
};

#endif
