////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 02/05/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/buffer.h"

#include "alarm_c/alarm.h"
#include "net/types.h"
#include "util_c/memory.h"
#include "util_c/rc_obj.h"
#include <string.h>
#include <stdio.h>

AOS_DECLARE_RCOBJ_LOCK;

// 
// Description
// This function appends a string 'str' + 'value' to the location 
// pointed to by 'index' in the buffer 'buff'.
//
int aos_buff_append_line_int(
		char *buff, 
		const int buff_len, 
		int *index, 
		const char * const str, 
		const int value)
{
	char local[20];
	aos_assert_r(buff, -1);
	aos_assert_r(index, -1);
	aos_assert_r(str, -1);
	int slen = strlen(str);
	sprintf(local, "%d\n", value);
	int vlen = strlen(local);
	aos_assert_rm(*index + slen + vlen < buff_len, -1, 
			"Index: %d, slen: %d, vlen: %d, buff_len: %d", 
			*index, slen, vlen, buff_len);
	memcpy(&buff[*index], str, slen);
	memcpy(&buff[*index + slen], local, vlen);
	*index += slen + vlen;
	return 0;
}


int aos_buff_append_line_str(
		char *buff, 
		const int buff_len, 
		int *index, 
		const char * const str, 
		const char * const value) 
{
	aos_assert_r(value, -1);
	return aos_buff_append_line_str_l(buff, buff_len, index, str, 
			value, strlen(value));
}


int aos_buff_append_line_str_l(
		char *buff, 
		const int buff_len, 
		int *index, 
		const char * const str, 
		const char * const value, 
		const int value_len) 
{
	aos_assert_r(buff, -1);
	aos_assert_r(index, -1);
	aos_assert_r(str, -1);
	aos_assert_r(value, -1);
	aos_assert_r(value_len >= 0, -1);

	int slen = strlen(str);
	int vlen = value_len;
	if (*index + slen + vlen > buff_len-1)
	{
		vlen = buff_len - *index - slen - 2;
	}

	memcpy(&buff[*index], str, slen);
	memcpy(&buff[*index + slen], value, vlen);
	buff[*index + slen + vlen] = '\n';
	*index += slen + vlen + 1;
	return 0;
}


int aos_buffer_hold(aos_buffer_t *buff)
{
	AOS_RC_OBJ_HOLD(buff);
}


int aos_buffer_put(struct aos_buffer *buff)
{
	AOS_RC_OBJ_PUT(buff);
}


int aos_buffer_reset(struct aos_buffer *buff)
{
	aos_not_implemented_yet;
	return -1;
}


// 
// Description
// It appends the string 'str' to the buffer.
//
static int aos_buffer_append_str(
		aos_buffer_t *buff, 
		const char * const str)
{
	aos_assert_r(buff, -1);
	aos_assert_r(str, -1);

	int len = strlen(str);
	aos_assert_r(!buff->mf->alloc_memory(buff, 
			buff->data_len + len + 1), -1);
	strcpy(&buff->buff[buff->data_len], str);
	buff->data_len += len;
	return 0;
}


// 
// Description
// It converts the integer into a string and appends the 
// string to the buffer. 
//
static int aos_buffer_append_int(
		aos_buffer_t *buff, 
		const int value)
{
	char b[100];
	sprintf(b, "%d", value);
	return aos_buffer_append_str(buff, b);
}


// 
// Description
// This function appends the data 'data' to the current buffer.
//
int aos_buffer_append_data(
		aos_buffer_t *buff, 
		aos_buffer_t *data)
{
	aos_assert_r(buff, -1);
	aos_assert_r(data, -1);

	aos_assert_r(!buff->mf->alloc_memory(buff, 
			buff->data_len + data->data_len), -1);
	memcpy(&buff->buff[buff->data_len], data->buff, 
			data->data_len);
	buff->data_len += data->data_len;
	return 0;
}


// 
// Description
// It allocates memory of at least 'size' big. If successful, 
// the contents are copied to the new memory and the old memory
// is released. 
//
int aos_buffer_alloc_memory(
		aos_buffer_t *buff, 
		const int size)
{
	aos_assert_r(buff, -1);
	aos_assert_r(size > 0, -1);
	if (buff->buff_len >= size) return 0;

	// Allocate the memory
	char *tmp = aos_malloc(size);
	aos_assert_r(tmp, -1);

	if (buff->data_len > 0)
	{
		aos_assert_g(buff->buff, cleanup);
		memcpy(tmp, buff->buff, buff->data_len);
	}

	if (buff->buff) aos_free(buff->buff);
	buff->buff = tmp;
	buff->buff_len = size;
	return 0;

cleanup:
	aos_free(tmp);
	return -1;
}


int aos_buffer_release_mem(struct aos_buffer *buff)
{
	aos_assert_r(buff, -1);
	if (buff->buff) aos_free(buff->buff);
	buff->buff = 0;
	buff->buff_len = 0;
	return 0;
}


int aos_buffer_destroy(struct aos_buffer *buff)
{
	aos_assert_r(!aos_buffer_release_mem(buff), -1);
	aos_free(buff);
	return 0;
}


static aos_buffer_mf_t sg_mf = 
{
	aos_buffer_hold,
	aos_buffer_put,
	aos_buffer_alloc_memory,
	aos_buffer_reset,
	aos_buffer_release_mem,
	aos_buffer_destroy,
	aos_buffer_append_str,
	aos_buffer_append_int,
	aos_buffer_append_data

};

int aos_buffer_init(aos_buffer_t *buff)
{
	aos_assert_r(buff, -1);
	memset(buff, 0, sizeof(*buff));
	buff->mf = &sg_mf;
	buff->buff = 0;
	buff->buff_len = 0;
	buff->data_len = 0;
	return 0;
}


aos_buffer_t *aos_create_conn_read_buffer()
{
	aos_buffer_t *buff = aos_malloc(sizeof(aos_buffer_t));
	aos_assert_r(buff, 0);
	memset(buff, 0, sizeof(aos_buffer_t));
	buff->mf = &sg_mf;

	buff->buff = aos_malloc(AOS_MAX_CONN_READ_BUFF);
	aos_assert_g(buff->buff, cleanup);
	buff->buff_len = AOS_MAX_CONN_READ_BUFF;
	buff->data_len = 0;
	return buff;

cleanup:
	aos_free(buff);
	return 0;
}


