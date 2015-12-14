////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: x509_object.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef	__object_util__
#define __object_util__

#include "aosUtil/Types.h"


typedef struct __data_object {
	BYTE * data;		//object pointer
	UINT   length;		//object length
	UINT   pos;		//the next position for parse
	struct __data_object * next;
} DATA_OBJECT;

#define asn_object_data(object)		((object)->data)
#define asn_object_datalen(object)	((object)->pos)
#define asn_object_pos(object)		((object)->pos)
#define asn_object_length(object)	((object)->length)
#define asn_object_cur_ptr(object)	((object)->data + (object)->pos)
extern DATA_OBJECT * asn_malloc_object(int buf_len);
extern void  asn_free_object(DATA_OBJECT *object);
extern DATA_OBJECT *asn_new_object(BYTE *buffer, UINT32 len);

#define asn_make_object(object, buffer, len) {	\
	(object)->data = buffer;	\
	(object)->length = len;	\
	(object)->pos = 0;	\
	(object)->next = NULL;	\
}

/* DATA_OBJECT Utility Routines */
extern int asn_seek(DATA_OBJECT *object, u32 position);
extern int asn_skip(DATA_OBJECT *object, const long offset);

extern int asn_getc(DATA_OBJECT *object);
extern int asn_read(DATA_OBJECT *object, void *buffer, const int length);
extern int asn_peek(DATA_OBJECT *object);

extern int asn_putc(DATA_OBJECT *object, const int ch);
extern int asn_write(DATA_OBJECT *object, const void *buffer, const int length);

#ifndef __KERNEL__
extern DATA_OBJECT * file_to_object(char *filename);
extern int object_to_file(DATA_OBJECT *object, char *filename);
#endif

#endif
