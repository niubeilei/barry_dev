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
// 02/22/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util2_gen_data_h
#define aos_util2_gen_data_h

#include "util_c/types.h"
#include "util_c/list.h"
#include "util2/value.h"
#include "util_c/gen_data_tags.h"


struct aos_xml_node;
struct aos_gen_data;

typedef struct aos_gen_data_entry
{
	aos_list_head_t		link;
	aos_gen_data_tag_e	tag;
	aos_value_t			value;
} aos_gen_data_entry_t;

typedef int (*aos_gen_data_hold_t)(
		struct aos_gen_data *data); 

typedef int (*aos_gen_data_put_t)(
		struct aos_gen_data *data); 

typedef int (*aos_gen_data_serialize_t)(
		struct aos_gen_data *data, 
		struct aos_xml_node *parent);

typedef int (*aos_gen_data_deserialize_t)(
		struct aos_gen_data *data, 
		struct aos_xml_node *node);

typedef int (*aos_gen_data_release_memory_t)(
		struct aos_gen_data *data); 

typedef int (*aos_gen_data_destroy_t)(
		struct aos_gen_data *data); 

typedef int (*aos_gen_data_get_value_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		struct aos_value **value); 

typedef int (*aos_gen_data_set_str_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		char *value, 
		const int alloc_memory);

typedef aos_gen_data_entry_t *(*aos_gen_data_get_entry_t)(
		struct aos_gen_data *data, 
		const aos_gen_data_tag_e tag, 
		const int flag);

typedef int (*aos_gen_data_set_int_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		const int value);

typedef int (*aos_gen_data_set_u32_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		const u32 value);

typedef int (*aos_gen_data_set_ptr_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		void *value);

typedef int (*aos_gen_data_get_ptr_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		void **ptr);

typedef int (*aos_gen_data_get_str_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		char **value, 
		int *len);

typedef int (*aos_gen_data_get_int_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		int *value); 

typedef int (*aos_gen_data_get_int_dft_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		int *value, 
		const int dft); 

typedef int (*aos_gen_data_get_u32_t)(
		struct aos_gen_data *data, 
		const u16 tag, 
		u32 *value); 

typedef int (*aos_gen_data_exist_t)(
		struct aos_gen_data *data, 
		const u16 tag);

typedef int (*aos_gen_data_is_empty_t)(
		struct aos_gen_data *data);

#define AOS_GEN_DATA_MEMFUNC_DECL						\
	aos_gen_data_hold_t				hold;				\
	aos_gen_data_put_t				put;				\
	aos_gen_data_serialize_t		serialize;			\
	aos_gen_data_deserialize_t		deserialize;		\
	aos_gen_data_release_memory_t	release_memory;		\
	aos_gen_data_destroy_t			destroy;			\
	aos_gen_data_set_str_t			set_str;			\
	aos_gen_data_set_int_t			set_int;			\
	aos_gen_data_set_u32_t			set_u32;			\
	aos_gen_data_set_ptr_t			set_ptr;			\
	aos_gen_data_get_value_t		get_value;			\
	aos_gen_data_get_entry_t		get_entry;			\
	aos_gen_data_get_ptr_t			get_ptr;			\
	aos_gen_data_get_str_t			get_str;			\
	aos_gen_data_get_int_t			get_int;			\
	aos_gen_data_get_int_dft_t		get_int_dft;		\
	aos_gen_data_get_u32_t			get_u32;			\
	aos_gen_data_exist_t			data_exist;			\
	aos_gen_data_is_empty_t			is_empty


#define AOS_GEN_DATA_MEMDATA_DECL						\
	aos_list_head_t					values;				\
	int								ref_count;			\
	int								is_destroying

typedef struct aos_gen_data_mf
{
	AOS_GEN_DATA_MEMFUNC_DECL;
} aos_gen_data_mf_t;

typedef struct aos_gen_data
{
	aos_gen_data_mf_t *mf;

	AOS_GEN_DATA_MEMDATA_DECL;
} aos_gen_data_t;

extern int aos_gen_data_init(aos_gen_data_t *sm);
extern aos_gen_data_t * aos_gen_data_create();
extern int aos_gen_data_clone(aos_gen_data_t **to, aos_gen_data_t *from);
extern int aos_gen_data_copy(aos_gen_data_t *to, aos_gen_data_t *from);

#endif

