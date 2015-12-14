////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: memmove.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

/*
 - memmove - fake ANSI C routine
 */
char *
memmove(dst, src, count)
char *dst;
char *src;
size_t count;
{
	register char *s;
	register char *d;
	register size_t n;

	if (dst > src)
		for (d = dst+count, s = src+count, n = count; n > 0; n--)
			*--d = *--s;
	else
		for (d = dst, s = src, n = count; n > 0; n--)
			*d++ = *s++;

	return(dst);
}
