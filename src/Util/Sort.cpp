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
// based on the string value. It has a companion IIL that keeps the
// same set of Docids but sorted based on docids. 
//
// Modification History:
// 05/18/2012 Created by felicia
////////////////////////////////////////////////////////////////////////////
#include "Util/Sort.h"

#include <math.h>

char *
AosSort::partition(char* first, char* last, const char* pivot, AosCompareFun *comp, char *tmpbuff)
{
	while (true)
	{
		while ((*comp)(first, pivot))
		{
			first += (*comp).size;
		}
		last -= (*comp).size;

		while ((*comp)(pivot, last))
		{
			last -= (*comp).size;
		}
		if (!(first < last))
		{
			return first;
		}
		change(first, last, comp, tmpbuff);
		first += (*comp).size;
	}
}


void
AosSort::move_first(char* a, char* b, char* c, AosCompareFun *comp, char *tmpbuff)
{
	if ((*comp)(a, b))
	{
		// It is: a < b
		if ((*comp)(b, c))
		{
			// b < c
			change(a, b, comp, tmpbuff);
		}
		else if ((*comp)(a, c))
		{
			// a < c
			change(a, c, comp, tmpbuff);
		}
	}
	else if ((*comp)(a, c))
	{
		// a > b && a < c
		return;
	}
	else if ((*comp)(b, c))
	{
		change(a, c, comp, tmpbuff);
	}
	else
	{
		change(a, b, comp, tmpbuff);
	}
}


char *
AosSort::pivot(char* first, char* last, AosCompareFun *comp, char *tmpbuff)
{
	char* mid = first + (((last-first) / (*comp).size) >> 1) * (*comp).size;
	move_first(first, mid, (last - (*comp).size), comp, tmpbuff);
    return partition(first + (*comp).size, last, first, comp, tmpbuff);
}


void
AosSort::quick_sort(char* first, char* last, int deeplimit, AosCompareFun *comp, char *tmpbuff)
{
	while (last - first > MAXNUM)
	{
		if(deeplimit == 0)
		{
			heap_sort(first, last, last, comp, tmpbuff);
			return;
		}
		
		deeplimit--;
		char* cut = pivot(first, last, comp, tmpbuff);
		quick_sort(cut, last, deeplimit, comp, tmpbuff);
		last = cut;
	}
}


void
AosSort::insert(char *last, AosCompareFun *comp, char *tmpbuff)
{
	memcpy(tmpbuff, last, (*comp).size);
	char* next = last;
	next -= (*comp).size;
	while((*comp)(tmpbuff, next))
	{
		memcpy(last, next, (*comp).size);
		last = next;
		next -= (*comp).size;
	}
	memcpy(last, tmpbuff, (*comp).size);
}

void
AosSort::uninsertion_sort(char* first,char* last, AosCompareFun *comp, char *tmpbuff)
{
	for (char* i = first; i != last; i += (*comp).size)
	{
		insert(i, comp, tmpbuff);
	}
}


void
AosSort::insertion_sort(char *first, char *last, AosCompareFun *comp, char *tmpbuff)
{
	if (first == last) return;

	for (char* i = first + (*comp).size; i != last; i += (*comp).size)
	{
		if ((*comp)(i, first))
		{
			memcpy(tmpbuff, i, (*comp).size);
			memmove(first+(*comp).size, first, (i-first)*sizeof(char));
			memcpy(first, tmpbuff, (*comp).size);
		}
		else
		{
			insert(i, comp, tmpbuff);
		}
	}
}


void
AosSort::insert_sort(char* first, char* last, AosCompareFun *comp, char *tmpbuff)
{
    if (last - first > MAXNUM)
	{
	   insertion_sort(first, first + MAXNUM, comp, tmpbuff);
	   uninsertion_sort(first + MAXNUM, last, comp, tmpbuff);
	}
    else
	{
		insertion_sort(first, last, comp, tmpbuff);
	}
}


void
AosSort::push_heap(char* first, int holeidx, int topidx, char* val, AosCompareFun *comp)
{
	int parent = (holeidx - 1) / 2;
	while (holeidx > topidx && (*comp)(first + parent * (*comp).size, val))
	{
		memcpy(first + holeidx * (*comp).size, first + parent * (*comp).size, (*comp).size);
		holeidx = parent;
		parent = (holeidx - 1) / 2;
	}
	memcpy(first + holeidx * (*comp).size, val, (*comp).size);
}


void
AosSort::adjust_heap(char* first, int holeidx, int len, char* val, AosCompareFun *comp)
{
	const int topidx = holeidx;
	int secondChild = holeidx;
	while (secondChild < (len - 1) / 2)
	{
		secondChild = 2 * (secondChild + 1);
		if ((*comp)(first + secondChild * (*comp).size, first + (secondChild - 1) * (*comp).size))
		{
			secondChild--;
		}
		memcpy(first + holeidx * (*comp).size, first + secondChild * (*comp).size, (*comp).size);
		holeidx = secondChild;
	}
	if ((len & 1) == 0 && secondChild == (len - 2) / 2)
	{
		secondChild = 2 * (secondChild + 1);
		memcpy(first + holeidx * (*comp).size, first + (secondChild - 1) * (*comp).size, (*comp).size);
		holeidx = secondChild - 1;
	}
	push_heap(first, holeidx, topidx, val, comp);
}


void
AosSort::make_heap(char* first, char* last, AosCompareFun *comp, char *tmpbuff)
{
	const int len = (last - first) / (*comp).size;
	if (len < 2) return;

	int holeidx = ((len - 2) / 2);
	while (true)
	{
		memcpy(tmpbuff, first + holeidx * (*comp).size, (*comp).size);
		adjust_heap(first, holeidx, len, tmpbuff, comp);
		
		if (holeidx == 0) return;
		holeidx--;
	}
}


void
AosSort::heap_select(char* first, char* mid, char* last, AosCompareFun *comp, char *tmpbuff)
{
	make_heap(first, mid, comp, tmpbuff);
	for (char * i = mid; i != last; i += (*comp).size)
	{
		if ((*comp)(i, first))
		{
			pop_heap(first, mid, i, comp, tmpbuff);
		}
	}
}


void
AosSort::pop_heap(char* first, char* last, char* result, AosCompareFun *comp, char *tmpbuff)
{
	memcpy(tmpbuff, result, (*comp).size);
	memcpy(result, first, (*comp).size);
	int len = (last - first) / (*comp).size;
	adjust_heap(first, 0, len, tmpbuff, comp);
}


void
AosSort::sort_heap(char* first, char* last, AosCompareFun *comp, char *tmpbuff)
{                                                                     
	while (last - first > (*comp).size)
	{
		last -= (*comp).size;
		pop_heap(first, last, last, comp, tmpbuff);
	}
}


void
AosSort::heap_sort(char* first, char* mid, char* last, AosCompareFun *comp, char *tmpbuff)
{
	heap_select(first, mid, last, comp, tmpbuff);
	sort_heap(first, mid, comp, tmpbuff);
}


void
AosSort::sort(char* first, char* last, AosCompareFun *comp, char *tmpbuff)
{
	if (first != last)
	{
		quick_sort(first, last, (int)(log2((last-first) / comp->size)) * 2, comp, tmpbuff);
		insert_sort(first, last, comp, tmpbuff);
	}
}

