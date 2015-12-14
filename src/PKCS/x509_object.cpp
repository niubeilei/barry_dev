////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: x509_object.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "aosUtil/Types.h"
#include "aosUtil/Memory.h"
#include "CertVerify/common.h"
#include "PKCS/x509_object.h"

DATA_OBJECT * asn_malloc_object(int buf_len) {
	DATA_OBJECT *object = NULL;

	if (buf_len < 0)
		return NULL;

	object = (DATA_OBJECT *)aos_malloc(sizeof(DATA_OBJECT));
	if (!object)
		return NULL;
	if (buf_len > 0) {
		object->data = (BYTE *)aos_malloc(buf_len);
		if (!object->data) {
			aos_free(object);
			return NULL;
		}
	} else
		object->data = NULL;

	object->length = buf_len;
	object->pos = 0;
	object->next = NULL;

	return object;
}

void  asn_free_object(DATA_OBJECT *object) {
	if (object) {
		if (object->length > 0 && object->data)
			aos_free(object->data);

		aos_free(object);
	}

	return;
}

DATA_OBJECT *asn_new_object(BYTE *buffer, UINT32 len) {
	DATA_OBJECT *object;

	if (!buffer)
		return NULL;

	object = asn_malloc_object(len);
	if (object)
		asn_write(object, buffer, len);

	return object;
}


/****************************************************************************
 *                 DATA_OBJECT Utility Routines                             *
 ****************************************************************************/

/* read one byte from memory buffer of DATA_OBJECT */
int
asn_getc(DATA_OBJECT *object) {
	BYTE c;

	if (object->pos  >= object->length)
		return -1;

	c = object->data[object->pos++];
	return c;
}


/* read BYTEs of size length to buffer */
int
asn_read(DATA_OBJECT *object, void *buffer, const int length) {
	if (!buffer || length <= 0 || (object->pos + length) > object->length)
		return -1;

	memcpy(buffer, object->data + object->pos, length);
	object->pos += length;

	return 0;
}

int
asn_peek(DATA_OBJECT *object) {
	if (object->pos >= object->length)
		return -1;

	/* Read the data from the buffer, but without advancing the read pointer
	   like asn_getc() does */
	return object->data[object->pos];
}

/* Move to an absolute position in a object */

int
asn_seek(DATA_OBJECT *object, const u32 position) {
	if (position < 0 || position > object->length)
		return -1;

	object->pos = position;
	return 0;
}

/* Skip a number of bytes in a object */

int
asn_skip(DATA_OBJECT *object, const long offset) {
	if (offset < 0)
		return -1;

	return asn_seek(object, object->pos + offset);
}


int
asn_putc(DATA_OBJECT *object, const int ch) 	{
	if (object->pos < 0 || object->pos > object->length)
		return -1;

	object->data[object->pos++] = ch;
	return 0;

}

/* Write the data to the object buffer */
int
asn_write(DATA_OBJECT *object, const void *buffer, const int length) {
	if (length < 0 || (object->pos + length) > (object->length + 1))
		return -1;

	memcpy(object->data + object->pos, buffer, length);
	object->pos += length;
	return 0;
}


#ifndef __KERNEL__
DATA_OBJECT * file_to_object(char *filename) {
	FILE *fp;
	DATA_OBJECT *object;
	int len, length;
	struct stat stat;

	if (!filename)
		return NULL;
	if (lstat(filename, &stat) < 0)
		return NULL;

	length = stat.st_size;
	object = asn_malloc_object(length);
	if (!object)
		return NULL;

	fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "open file:%s failed\n", filename);
		goto error;
	}

	len = 0;
	do {
		int c;

		c = fgetc(fp);
		if (c == EOF)
			break;
		object->data[len++] = c;
	} while(len < length);

	fclose(fp);
	object->length = len;

	return object;
error:
	if (object)	asn_free_object(object);
	return NULL;
}



int object_to_file(DATA_OBJECT *object, char *filename) {
	FILE *fp;
	int len;

	if (!object || !object->data || !filename)
		return -1;

	fp = fopen(filename, "wb");
	if (!fp) {
		printf("object_to_file:fopen %s failed\n", filename);
		return -1;
	}

	if (object->length > 0) {
		len = fwrite(object->data, 1, object->length, fp);
		printf("object_to_file:write %d / %d\n", len, object->length);
	}

	fclose(fp);

	return 0;
}
#endif
