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
#ifndef aos_util_buffer_h
#define aos_util_buffer_h

struct aos_buffer;

typedef int (*aos_buffer_alloc_memory_t)(
		struct aos_buffer *buff, 
		const int size);

typedef int (*aos_buffer_hold_t)(
		struct aos_buffer *buff);

typedef int (*aos_buffer_put_t)(
		struct aos_buffer *buff);

typedef int (*aos_buffer_reset_t)(
		struct aos_buffer *buff);

typedef int (*aos_buffer_release_mem_t)(
		struct aos_buffer *buff);

typedef int (*aos_buffer_destroy_t)(
		struct aos_buffer *buff);

typedef int (*aos_buffer_append_data_t)(
		struct aos_buffer *buff, 
		struct aos_buffer *data);

typedef int (*aos_buffer_append_str_t)(
		struct aos_buffer *buff, 
		const char * const str);

typedef int (*aos_buffer_append_int_t)(
		struct aos_buffer *buff, 
		const int value);

#define AOS_BUFFER_MEMFUNC_DECL						\
	aos_buffer_hold_t			hold;				\
	aos_buffer_put_t			put;				\
	aos_buffer_alloc_memory_t	alloc_memory;		\
	aos_buffer_reset_t			reset;				\
	aos_buffer_release_mem_t	release_memory;		\
	aos_buffer_destroy_t 		destroy;			\
	aos_buffer_append_str_t 	append_str;			\
	aos_buffer_append_int_t 	append_int;			\
	aos_buffer_append_data_t 	append_data

#define AOS_BUFFER_MEMDATA_DECL						\
	char *			buff;							\
	int				buff_len;						\
	int				data_len;						\
	int				ref_count;						\
	int				is_destroying

typedef struct aos_buffer_mf
{
	AOS_BUFFER_MEMFUNC_DECL;
} aos_buffer_mf_t;

typedef struct aos_buffer
{
	aos_buffer_mf_t *mf;

	AOS_BUFFER_MEMDATA_DECL;
} aos_buffer_t;


#ifdef __cplusplus
extern "C" {
#endif

extern int aos_buffer_init(aos_buffer_t *buff);
extern aos_buffer_t *aos_buffer_create();

extern int aos_buff_append_line_int(
		char *buff, 
		const int buff_len, 
		int *index, 
		const char * const str, 
		const int value);

extern int aos_buff_append_line_str(
		char *buff, 
		const int buff_len, 
		int *index, 
		const char * const str, 
		const char * const value); 

extern int aos_buff_append_line_str_l(
		char *buff, 
		const int buff_len, 
		int *index, 
		const char * const str, 
		const char * const value, 
		const int value_len); 

extern void *aos_memory_expand(
		void *buff, 
		const int crt_size,
		const int new_size);

extern aos_buffer_t *aos_create_conn_read_buffer();

#ifdef __cplusplus
}
#endif

#endif // End of Include

