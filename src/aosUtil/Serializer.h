////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Serializer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aosUtil_Serializer_h
#define aos_aosUtil_Serializer_h

#include "aosUtil/Types.h"

enum
{
	eAosSerializer_Integer,
	eAosSerializer_String
};

typedef struct AosSerializer
{
	u32		buff_size;
	char *	buff;
	u32 	cursor;
} AosSerializer_t;

extern char * AosSerializer_finish(AosSerializer_t *bag);
extern int AosSerializer_init(AosSerializer_t **bag, const u32 size);
extern int AosSerializer_set(
            AosSerializer_t **bag,
            const char *buff,
            const u32 buff_size);
extern int AosSerializer_addInt(AosSerializer_t *self, const int value);
extern int AosSerializer_addStr(AosSerializer_t *self, const char *str, const u32 len);
extern int AosSerializer_getInt(AosSerializer_t *self, int *value);
extern int AosSerializer_getStr(AosSerializer_t *self, char *str, u32 *len);
#endif
