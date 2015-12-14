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
// 02/15/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_charset_h
#define aos_rvg_charset_h

#include "rvg_c/types.h"
#include "util_c/types.h"
#include "util_c/dyn_array.h"

#define AOS_CHARSET_MAX_WEIGHT 1000
#define AOS_CHARSET_MAX_TOTAL_WEIGHTS 10000

struct aos_xml_node;
struct aos_charset;
struct aos_rig;
struct aos_value;
struct aos_buffer;

#define AOS_CHARSET_MAX_RANGES 256

typedef struct aos_charset_range
{
	u8					start;
	u8					end;
	int					weight;
} aos_charset_range_t;

typedef int (*aos_charset_hold_t)(struct aos_charset *cs);
typedef int (*aos_charset_put_t)(struct aos_charset *cs);

typedef int (*aos_charset_release_memory_t)(
		struct aos_charset *cs);

typedef int (*aos_charset_destroy_t)(
		struct aos_charset *cs);

typedef int (*aos_charset_integrity_check_t)(
		struct aos_charset *cs);

typedef int (*aos_charset_serialize_t)(
		struct aos_charset *cs, 
		struct aos_xml_node *parent);

typedef int (*aos_charset_deserialize_t)(
		struct aos_charset *cs, 
		struct aos_xml_node *node);

typedef char (*aos_charset_next_char_t)(
		struct aos_charset *cs);

typedef int (*aos_charset_next_str_t)(
		struct aos_charset *cs, 
		char *str, 
		const int len,
		const int repeat, 
		const aos_order_e order);

typedef int (*aos_charset_next_value_t)(
		struct aos_charset *cs, 
		struct aos_value *value,
		const int len, 
		const int repeat, 
		const aos_order_e order);

typedef int (*aos_charset_is_char_in_t)(
		struct aos_charset *cs, 
		const unsigned char c);

typedef int (*aos_charset_is_valid_str_t)(
		struct aos_charset *cs, 
		const char * const str, 
		const int len);

typedef char *(*aos_charset_get_desc_t)(
		struct aos_charset *cs);

typedef char *(*aos_charset_get_keywords_t)(
		struct aos_charset *cs);

typedef int (*aos_charset_add_range_t)(
		struct aos_charset *cs, 
		const u8 start, 
		const u8 end, 
		const int weight);

typedef int (*aos_charset_domain_check_t)(
		struct aos_charset *cs, 
		struct aos_value *value);

typedef int (*aos_charset_domain_check_len_t)(
		struct aos_charset *cs, 
		struct aos_value *value, 
		int *idx, 
		aos_domain_check_e type, 
		const int min_len, 
		const int max_len);

#define AOS_CHARSET_MEMFUNC_DECL						\
	aos_charset_hold_t				hold;				\
	aos_charset_put_t				put;				\
	aos_charset_destroy_t			release_memory;		\
	aos_charset_destroy_t			destroy;			\
	aos_charset_integrity_check_t	integrity_check;	\
	aos_charset_serialize_t			serialize;			\
	aos_charset_deserialize_t		deserialize;		\
	aos_charset_next_char_t			next_char;			\
	aos_charset_next_value_t		next_value;			\
	aos_charset_next_str_t			next_str;			\
	aos_charset_is_char_in_t		is_char_in;			\
	aos_charset_is_valid_str_t		is_valid_str;		\
	aos_charset_add_range_t			add_range;			\
	aos_charset_domain_check_t		domain_check;		\
	aos_charset_domain_check_len_t	domain_check_len;	\
	aos_charset_get_desc_t			get_desc;			\
	aos_charset_get_keywords_t		get_keywords



#define AOS_CHARSET_MEMDATA_DECL						\
	char *					name;						\
	char *					desc;						\
	char *					keywords;					\
	char					map[256];					\
	aos_charset_range_t **	ranges;						\
	aos_dyn_array_t			ranges_array;				\
	int						ref_count;					\
	struct aos_rig *		range_selector;				\
	int						is_destroying

typedef struct aos_charset_xml_mf
{
	AOS_CHARSET_MEMFUNC_DECL;
} aos_charset_mf_t;

typedef struct aos_charset
{
	aos_charset_mf_t *mf;

	AOS_CHARSET_MEMDATA_DECL;
} aos_charset_t;

#ifdef __cplusplus
extern "C" {
#endif

extern aos_charset_t *aos_charset_create_str(const char * const contents);
extern aos_charset_t *aos_charset_create_xml(struct aos_xml_node *node);

#ifdef __cplusplus
}
#endif

#endif

