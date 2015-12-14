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
// 02/02/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util2_value_h
#define aos_util2_value_h

#include "util_c/types.h"
#include "util_c/list.h"
#include "util_c/data_type.h"

#define AOS_VALUE_TOO_BIG 2000

struct aos_value;
struct aos_xml_node;

// 
// Member functions
//
typedef int (*aos_value_hold_t)(struct aos_value *value);
typedef int (*aos_value_put_t)(struct aos_value *value);

typedef int (*aos_value_serialize_t)(
		struct aos_value *value, 
		struct aos_xml_node *parent);

typedef int (*aos_value_deserialize_t)(
		struct aos_value *value, 
		struct aos_xml_node *node);

typedef int (*aos_value_destroy_t)(
		struct aos_value *value); 

typedef int (*aos_value_release_memory_t)(
		struct aos_value *value); 

typedef int (*aos_value_to_char_t)(
		struct aos_value *value, 
		char *vv);

typedef int (*aos_value_to_int16_t)(
		struct aos_value *value, 
		int16_t *vv);

typedef int (*aos_value_to_int_t)(
		struct aos_value *value, 
		int *vv);

typedef int (*aos_value_to_int32_t)(
		struct aos_value *value, 
		int *vv);

typedef int (*aos_value_to_int64_t)(
		struct aos_value *value, 
		int64_t *vv);

typedef int (*aos_value_to_u8_t)(
		struct aos_value *value, 
		u8 *vv);

typedef int (*aos_value_to_u16_t)(
		struct aos_value *value, 
		u16 *vv);

typedef int (*aos_value_to_u32_t)(
		struct aos_value *value, 
		u32 *vv);

typedef int (*aos_value_to_u64_t)(
		struct aos_value *value, 
		u64 *vv);

typedef int (*aos_value_to_double_t)(
		struct aos_value *value, 
		double *vv);

typedef int (*aos_value_to_str_t)(
		struct aos_value *value, 
		char **vv, 
		int *len);

typedef int (*aos_value_to_str_s_t)(
		struct aos_value *value, 
		char **vv, 
		int *len);

typedef int (*aos_value_to_str_b_t)(
		struct aos_value *value, 
		char *vv, 
		int *len);

typedef int (*aos_value_to_ptr_t)(
		struct aos_value *value, 
		void **ptr);

typedef int (*aos_value_set_char_t)(
		struct aos_value *value, 
		const char);

typedef int (*aos_value_set_value_t)(
		struct aos_value *value, 
		const aos_data_type_e type, 
		const char * const buff, 
		const int len);

typedef struct aos_value *(*aos_value_clone_t)(
		struct aos_value *value);

typedef int (*aos_value_copy_t)(
		struct aos_value *from, 
		struct aos_value *to);

typedef int (*aos_value_integrity_check_t)(
		struct aos_value *value);

typedef int (*aos_value_set_int16_t)(
		struct aos_value *value, 
		const int16_t);

typedef int (*aos_value_set_int_t)(
		struct aos_value *value, 
		const int);

typedef int (*aos_value_set_int32_t)(
		struct aos_value *value, 
		const int32_t);

typedef int (*aos_value_set_int64_t)(
		struct aos_value *value, 
		const int64_t);

typedef int (*aos_value_set_u8_t)(
		struct aos_value *value, 
		const u8);

typedef int (*aos_value_set_u16_t)(
		struct aos_value *value, 
		const u16);

typedef int (*aos_value_set_u32_t)(
		struct aos_value *value, 
		const u32);

typedef int (*aos_value_set_u64_t)(
		struct aos_value *value, 
		const u64);

typedef int (*aos_value_set_double_t)(
		struct aos_value *value, 
		const double);

typedef int (*aos_value_set_str_t)(
		struct aos_value *value, 
		const char *, 
		const int);

typedef int (*aos_value_set_binary_t)(
		struct aos_value *value, 
		const char *, 
		const int);

typedef int (*aos_value_append_str_t)(
		struct aos_value *value, 
		const char *str, 
		const int len);

typedef int (*aos_value_set_ptr_t)(
		struct aos_value *value, 
		void *ptr);

typedef int (*aos_value_set_mem_t)(
		struct aos_value *value, 
		char *, 
		const int);

typedef int (*aos_value_set_int16_from_str_t)(
		struct aos_value *value, 
		const char * const str, 
		const int len);

typedef int (*aos_value_set_int32_from_str_t)(
		struct aos_value *value, 
		const char * const str, 
		const int len);

typedef int (*aos_value_set_int64_from_str_t)(
		struct aos_value *value, 
		const char * const str, 
		const int len);

typedef int (*aos_value_set_u16_from_str_t)(
		struct aos_value *value, 
		const char * const str, 
		const int len);

typedef int (*aos_value_set_u32_from_str_t)(
		struct aos_value *value, 
		const char * const str, 
		const int len);

typedef int (*aos_value_set_addr_from_str_t)(
		struct aos_value *value, 
		const char * const str, 
		const int len);

typedef int (*aos_value_set_u64_from_str_t)(
		struct aos_value *value, 
		const char * const str, 
		const int len);

typedef int (*aos_value_set_double_from_str_t)(
		struct aos_value *value, 
		const char * const str, 
		const int len);

typedef struct aos_value_mf
{
	aos_value_hold_t		hold;
	aos_value_put_t			put;
	aos_value_serialize_t	serialize;
	aos_value_deserialize_t	deserialize;
	aos_value_destroy_t		destroy;
	aos_value_release_memory_t		release_memory;
	aos_value_to_char_t		to_char;
	aos_value_to_int16_t	to_int16;
	aos_value_to_int_t		to_int;
	aos_value_to_int32_t	to_int32;
	aos_value_to_int64_t	to_int64;
	aos_value_to_u8_t		to_u8;
	aos_value_to_u16_t		to_u16;
	aos_value_to_u32_t		to_u32;
	aos_value_to_u64_t		to_u64;
	aos_value_to_double_t	to_double;
	aos_value_to_str_t		to_str;
	aos_value_to_str_b_t	to_str_b;
	aos_value_to_str_s_t	to_str_s;
	aos_value_to_ptr_t		to_ptr;

	aos_value_set_char_t 	set_char;
	aos_value_set_int16_t	set_int16;
	aos_value_set_int_t		set_int;
	aos_value_set_int32_t	set_int32;
	aos_value_set_int64_t	set_int64;
	aos_value_set_u8_t		set_u8;
	aos_value_set_u16_t		set_u16;
	aos_value_set_u32_t		set_u32;
	aos_value_set_u64_t		set_u64;
	aos_value_set_double_t	set_double;
	aos_value_set_str_t		set_str;
	aos_value_set_value_t	set_value;

	aos_value_set_int16_from_str_t	set_int16_from_str;
	aos_value_set_int32_from_str_t	set_int32_from_str;
	aos_value_set_int64_from_str_t	set_int64_from_str;
	aos_value_set_u16_from_str_t	set_u16_from_str;
	aos_value_set_u32_from_str_t	set_u32_from_str;
	aos_value_set_u64_from_str_t	set_u64_from_str;
	aos_value_set_addr_from_str_t	set_addr_from_str;
	aos_value_set_double_from_str_t	set_double_from_str;
	aos_value_set_binary_t	set_binary;

	aos_value_append_str_t	append_str;
	aos_value_set_ptr_t		set_ptr;
	aos_value_integrity_check_t	integrity_check;
	aos_value_clone_t		clone;
	aos_value_copy_t		copy;
} aos_value_mf_t;

#define AOS_VALUE_DEFAULT_MEMORY_SIZE 8

typedef struct aos_value
{
	aos_value_mf_t *mf;

//	char 				default_memory[AOS_VALUE_DEFAULT_MEMORY_SIZE];
	aos_data_type_e		type;
	union
	{
		u8		u8_value;
		u16		u16_value;
		u32		u32_value;
		u64		u64_value;
		int8_t	int8_value;
		int16_t	int16_value;
		int32_t	int32_value;
		int64_t	int64_value;
		double	double_value;
		char *	string;
		void *	ptr;
	} value;

	int					mem_size;
	int					data_size;
	int					ref_count;
	int					is_destroying;
	char *				filename;
	int					bin_len;
} aos_value_t;


#ifdef __cplusplus
extern "C" {
#endif

extern aos_value_t *aos_value_factory(
		const aos_data_type_e type, 
		char * value, 
		const int len);

extern aos_value_t *aos_value_create_xml(struct aos_xml_node *node);

extern int aos_value_init(aos_value_t *value);

extern aos_value_t *aos_value_clone(aos_value_t *value);
extern int aos_value_copy(aos_value_t *from, aos_value_t *to);
extern int aos_value_atoll(const char * const str, int64_t *vv);
extern int aos_value_atoull(const char * const str, u64 *vv);

#ifdef __cplusplus
}
#endif
#endif // End of Include

