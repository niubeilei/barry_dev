////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Serializer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Serializer.h"

#include "aosUtil/Memory.h"
#include "alarm_c/alarm.h"
#include "aosUtil/ReturnCode.h"
#include "KernelSimu/string.h"


char * AosSerializer_finish(AosSerializer_t *bag)
{
	if (bag->cursor < 4)
	{
		aos_alarm("Invalid cursor: %d", bag->cursor);
		return 0;
	}

	u32 len = bag->cursor-4;
	memcpy(bag->buff, &len, sizeof(int));
	return bag->buff;
}


int AosSerializer_init(AosSerializer_t **bag, const u32 size)
{
	*bag = (AosSerializer_t *)aos_malloc(sizeof(AosSerializer_t));
	aos_assert_r(*bag, -1);
	memset(*bag, 0, sizeof(AosSerializer_t));

	(*bag)->buff = (char *)aos_malloc(size);
	if (!(*bag)->buff)
	{
		aos_free(*bag);
		return aos_alarm("Memory error");
	}

	(*bag)->buff_size = size;
	(*bag)->cursor = sizeof(int);
	return 0;
}


// 
// This function creates a serializer and sets its memory based
// on the contents passed in by 'buff'. It will create its own
// memory. The caller can release 'buff' after calling this 
// function.
//
int AosSerializer_set(
			AosSerializer_t **bag, 
			const char *buff, 
			const u32 buff_size)
{
	// 
	// The first four bytes are for 'buff_size'. 
	// 
	u32 len;
	
	aos_assert_r(buff_size >= 4, -1);

	memcpy(&len, buff, sizeof(int));
	if (buff_size != len + 4)
	{
		return aos_alarm("Invalid length: %d, %d", buff_size, len);
	}

	*bag = (AosSerializer_t *)aos_malloc(sizeof(AosSerializer_t));
	aos_assert_r(*bag, -1);
	(*bag)->buff_size = len;
	(*bag)->cursor = 5;

	(*bag)->buff = (char *)aos_malloc(len);
	if (!(*bag)->buff)
	{
		aos_free(*bag);
		return aos_alarm("Memory error");
	}

	memcpy((*bag)->buff, buff+sizeof(int), len);
	return 0;
}

	
int AosSerializer_addInt(AosSerializer_t *self, const int value)
{
	if (self->cursor + sizeof(value) + 1 >= self->buff_size)
	{
		return aos_alarm("Out of boundary: %d, %d", 
			self->cursor, self->buff_size);
	}

	*(self->buff + self->cursor) = eAosSerializer_Integer;
	memcpy(self->buff + self->cursor + 1, (void *)&value, sizeof(value));
	self->cursor += sizeof(value) + 1;
	return 0;
}


int AosSerializer_addStr(AosSerializer_t *self, const char *str, const u32 len)
{
	if (self->cursor + sizeof(len) + 1 + len >= self->buff_size)
	{
		return aos_alarm("Out of boundary: %d, %d, %d", 
			self->cursor, self->buff_size, len);
	}

	*(self->buff + self->cursor) = eAosSerializer_String;
	memcpy(self->buff + self->cursor + 1, (void *)len, sizeof(len));
	self->cursor += sizeof(len) + 1;

	memcpy(self->buff + self->cursor, str, len);
	return 0;
}


int AosSerializer_getInt(AosSerializer_t *self, int *value)
{
	if (self->cursor + 1 + sizeof(int) > self->buff_size)
	{
		return aos_alarm("Out of boundary: %d, %d", 
			self->cursor, self->buff_size);
	}

	if (*(self->buff + self->cursor) != eAosSerializer_Integer)
	{
		return aos_alarm("Not an integer: %d, %d", 
			self->cursor, 
			*(self->buff + self->cursor));
	}

	memcpy(value, self->buff + self->cursor + 1, sizeof(int));
	self->cursor += sizeof(int) + 1;
	return 0;
}


int AosSerializer_getStr(AosSerializer_t *self, char *str, u32 *len)
{
	if (self->cursor + 1 + sizeof(int) > self->buff_size)
	{
		return aos_alarm("Out of boundary: %d, %d", 
			self->cursor, self->buff_size);
	}

	if (*(self->buff + self->cursor) != eAosSerializer_String)
	{
		return aos_alarm("Not an integer: %d, %d", 
			self->cursor, 
			*(self->buff + self->cursor));
	}

	u32 theLen;
	memcpy(&theLen, self->buff + self->cursor + 1, sizeof(int));
	if (theLen + self->cursor > self->buff_size)
	{
		return aos_alarm("Out of boundary: %d, %d, %d", 
			self->cursor, self->buff_size, theLen);
	}

	if (theLen > *len)
	{
		return aos_alarm("Out of boundary: %d, %d", theLen, *len);
	}

	memcpy(str, self->buff + self->cursor + 1 + sizeof(int), theLen);
	self->cursor += sizeof(int) + 1 + theLen;
	*len = theLen;
	return 0;
}

