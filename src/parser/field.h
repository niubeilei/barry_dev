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
// 01/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_field_h
#define aos_parser_field_h

#include "conditions_c/cond.h"
#include "parser/value_map.h"
#include "parser/types.h"
#include "parser/attr.h"
#include "util_c/types.h"
#include "util_c/list.h"
#include "util_c/dyn_array.h"
#include "util2/value.h"



struct aos_field;
struct aos_omparser;
struct aos_xml_node;
struct aos_rvg;
struct aos_value;

#define AOS_OMP_MAX_FIELD_NAME_LEN 32
#define AOS_FIELD_MAX_MEMBERS 1000
#define AOS_FIELD_MAX_ATTRS 10000

typedef int (*aos_field_match_name_t)(
		struct aos_field *node, 
		const char * const name);

typedef int (*aos_field_set_lws_gen_t)(
		struct aos_field *field, 
		struct aos_rvg *gen);

typedef int (*aos_field_set_tws_gen_t)(
		struct aos_field *field, 
		struct aos_rvg *gen);

typedef int (*aos_field_add_child_t)(
		struct aos_field *field, 
		struct aos_field *child);

typedef int (*aos_field_destroy_t)(
		struct aos_field *field);

typedef int (*aos_field_serialize_t)(
		struct aos_field *field, 
		struct aos_xml_node *parent);

typedef int (*aos_field_deserialize_t)(
		struct aos_field *field, 
		struct aos_xml_node *node);

typedef int (*aos_field_dump_to_t)(
		struct aos_field *field, 
		char *buff, 
		int *len);

typedef const char * (*aos_field_dump_t)(
		struct aos_field *field); 

typedef int (*aos_field_integrity_check_t)(
		struct aos_field *field);

typedef int (*aos_field_set_value_t)(
		struct aos_field *field, 
		struct aos_value *value);

typedef int (*aos_field_put_t)(
		struct aos_field *field);

typedef int (*aos_field_hold_t)(
		struct aos_field *field);

typedef int (*aos_field_parse_t)(struct aos_field *field); 

typedef int (*aos_field_get_value_t)(
		struct aos_field *field, 
		struct aos_value *value);

typedef int (*aos_field_get_str_t)(
		struct aos_field *field, 
		char * value, 
		int *value_len);

typedef int (*aos_field_get_str_by_attr_t)(
		struct aos_field *field, 
		const char * const attr_name,
		const char * const attr_value,
		char **value); 

typedef int (*aos_field_get_int_by_attr_t)(
		struct aos_field *field, 
		const char * const attr_name,
		const char * const attr_value,
		int *value); 

typedef int (*aos_field_get_str_b_t)(
		struct aos_field *field, 
		char **value); 

typedef int (*aos_field_set_contents_t)(
		struct aos_field *field, 
		const char * const contents, 
		const int len);

typedef int (*aos_field_replace_contents_t)(
		struct aos_field *field, 
		const char * const contents);

typedef int (*aos_field_set_attr_t)(
		struct aos_field *field, 
		aos_attr1_t **attr, 
		const int num_attr);

typedef int (*aos_field_get_attr_int64_t)(
		struct aos_field *field, 
		const char * const name, 
		int64_t *value);

typedef int (*aos_field_get_attr_u64_t)(
		struct aos_field *field, 
		const char * const name, 
		u64 *value);

typedef int (*aos_field_get_int64_t)(
		struct aos_field *field, 
		int64_t *value);

typedef int (*aos_field_get_u64_t)(
		struct aos_field *field, 
		u64 *value);

typedef char *(*aos_field_get_attr_t)(
		struct aos_field *field, 
		const char * const attr);

typedef int (*aos_field_add_attr_t)(
		struct aos_field *field, 
		const char * const name,
		const char * const value);

#define AOS_FIELD_MAX_NAME_LEN	32

#define AOS_FIELD_MEMFUNC_DECL 							\
	aos_field_hold_t			hold;					\
	aos_field_put_t				put;					\
	aos_field_match_name_t 		match_name;				\
	aos_field_set_lws_gen_t   	set_lws_gen;			\
	aos_field_set_tws_gen_t   	set_tws_gen;			\
	aos_field_add_child_t		add_child;				\
	aos_field_set_value_t		set_value;				\
	aos_field_get_str_t			get_str;				\
	aos_field_get_str_b_t		get_str_b;				\
	aos_field_get_str_by_attr_t	get_str_by_attr;		\
	aos_field_get_int_by_attr_t	get_int_by_attr;		\
	aos_field_set_contents_t 	set_contents;			\
	aos_field_replace_contents_t replace_contents;		\
	aos_field_add_attr_t		add_attr;				\
	aos_field_set_attr_t		set_attr;				\
	aos_field_get_attr_t		get_attr;				\
	aos_field_get_attr_int64_t	get_attr_int64;			\
	aos_field_get_attr_u64_t	get_attr_u64;			\
	aos_field_get_int64_t		get_int64;				\
	aos_field_get_u64_t			get_u64;				\
	aos_field_dump_to_t			dump_to;				\
	aos_field_dump_t			dump;					\
	aos_field_destroy_t			destroy;				\
	aos_field_serialize_t		serialize;				\
	aos_field_deserialize_t		deserialize;			\
	aos_field_integrity_check_t	integrity_check;		\
	aos_field_parse_t			parse;					\
	aos_field_get_value_t		get_value

#define AOS_FIELD_MEMFUNC_INIT							\
	aos_field_hold,										\
	aos_field_put,										\
	aos_field_match_name,								\
	aos_field_set_lws_gen,								\
	aos_field_set_tws_gen,								\
	aos_field_add_child,								\
	aos_field_set_value,								\
	aos_field_get_str,									\
	aos_field_get_str_b,								\
	aos_field_get_str_by_attr,							\
	aos_field_get_int_by_attr,							\
	aos_field_set_contents,								\
	aos_field_replace_contents,							\
	aos_field_add_attr,									\
	aos_field_set_attr,									\
	aos_field_get_attr,									\
	aos_field_get_attr_int64,							\
	aos_field_get_attr_u64,								\
	aos_field_get_int64,								\
	aos_field_get_u64

#define AOS_FIELD_MEMDATA_DECL							\
	aos_field_type_e		type;						\
	char *					name;						\
	struct aos_scm_field *	schema;						\
	char * 					contents;					\
	char					parsed;						\
	int						content_len;				\
	struct aos_rvg * 		lws_gen;					\
	struct aos_rvg * 		tws_gen;					\
	aos_len_type_e			len_type;					\
	aos_data_type_e			data_type;					\
	struct aos_field **		fields;						\
	aos_dyn_array_t			fields_array;				\
	struct aos_value *		value;						\
	char					with_schema;				\
	int						ref_count;					\
	char					is_root;					\
	struct aos_attr1 **		attrs;						\
	aos_dyn_array_t			attrs_array;

typedef struct aos_field_mf
{
	AOS_FIELD_MEMFUNC_DECL;
} aos_field_mf_t;

typedef struct aos_field
{
	aos_field_mf_t *mf;
	AOS_FIELD_MEMDATA_DECL;
} aos_field_t;


// 
// Public member function declarations
//
extern int aos_field_match_name(
		struct aos_field *node, 
		const char * const name);

extern int aos_field_set_lws_gen(
		struct aos_field *field, 
		struct aos_rvg *gen);

extern int aos_field_set_tws_gen(
		struct aos_field *field, 
		struct aos_rvg *gen);

extern int aos_field_add_child(
		struct aos_field *field, 
		struct aos_field *child);

extern int aos_field_release_memory(
		struct aos_field *field);

extern int aos_field_serialize(
		struct aos_field *field, 
		struct aos_xml_node *parent);

extern int aos_field_deserialize(
		struct aos_field *field, 
		struct aos_xml_node *node);

extern int aos_field_dump(
		struct aos_field *field, 
		char *buff, 
		int *len);

extern int aos_field_set_value(
		struct aos_field *field, 
		struct aos_value *value);

extern int aos_field_put(
		struct aos_field *field);

extern int aos_field_hold(
		struct aos_field *field);

extern int aos_field_reset(
		struct aos_field *field); 

extern int aos_field_get_str(
		struct aos_field *field, 
		char * value, 
		int *value_len);

extern int aos_field_get_str_by_attr(
		struct aos_field *field, 
		const char * const attr_name,
		const char * const attr_value,
		char **value); 

extern int aos_field_get_int_by_attr(
		struct aos_field *field, 
		const char * const attr_name,
		const char * const attr_value,
		int *value); 

extern int aos_field_get_str_b(
		struct aos_field *field, 
		char ** value);

extern int aos_field_replace_contents(
		struct aos_field *field, 
		const char * const contents);

extern int aos_field_add_attr(
		aos_field_t *field, 
		const char * const name,
		const char * const value);

extern int aos_field_set_attr(
		struct aos_field *field, 
		aos_attr1_t **attr, 
		const int num_attr);

extern char *aos_field_get_attr(
		struct aos_field *field, 
		const char * const attr);

extern int aos_field_set_contents(
		struct aos_field *field, 
		const char * const contents, 
		const int len);

extern int aos_field_get_attr_int64(
		struct aos_field *field, 
		const char * const name, 
		int64_t *value);

extern int aos_field_get_attr_u64(
		struct aos_field *field, 
		const char * const name, 
		u64 *value);

extern int aos_field_get_int64(
		struct aos_field *field, 
		int64_t *value);

extern int aos_field_get_u64(
		struct aos_field *field, 
		u64 *value);

// 
// Global Functions
//
extern int aos_field_init(
		aos_field_t *field, 
	   	const aos_field_type_e type, 
	   	const char * const name,
	   	const aos_data_type_e data_type, 
		const char * const contents, 
		const int len);

extern aos_field_t *aos_field_factory(
		struct aos_xml_node *node);

extern int aos_field_integrity_check(
		struct aos_field *field);

extern int aos_field_dump_to(
		struct aos_field *field, 
		char *buff, 
		int *len);

#endif

