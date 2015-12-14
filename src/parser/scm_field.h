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
#ifndef aos_parser_scm_field_h
#define aos_parser_scm_field_h

#include "conditions_c/cond.h"
#include "parser/value_map.h"
#include "parser/types.h"
#include "util_c/types.h"
#include "util_c/dyn_array.h"
#include "util_c/list.h"
#include "util2/value.h"

struct aos_xml_node;
struct aos_cond;
struct aos_rvg;
struct aos_value_map;
struct aos_omparser;
struct aos_field;
struct aos_msg_selector;
struct aos_buffer;

#define AOS_OMP_MAX_scm_field_NAME_LEN 	16
#define AOS_MAX_WHITE_SPACE_LEN			256
#define AOS_SCM_FIELD_XML_MAX_CHILDREN 	1000

struct aos_scm_field;
struct aos_omparser;

typedef struct aos_scm_field * (*aos_scm_field_select_msg_t)(
		struct aos_scm_field *schema, 
		struct aos_omparser *parser, 
		int *is_eof);

typedef int (*aos_scm_field_set_msg_selector_t)(
		struct aos_scm_field *schema, 
		struct aos_msg_selector *selector);

typedef int (*aos_scm_field_set_present_t)(
		struct aos_scm_field *schema, 
		const char value);

typedef int (*aos_scm_field_add_child_t)(
		struct aos_scm_field *schema, 
		struct aos_scm_field *child);

typedef int (*aos_scm_field_set_lws_t)(
		struct aos_scm_field *field, 
		char *, 
		const u32);

typedef int (*aos_scm_field_set_tws_t)(
		struct aos_scm_field *field, 
		char *, 
		const u32);

typedef int (*aos_scm_field_set_lws_gen_t)(
		struct aos_scm_field *field, 
		struct aos_rvg *gen);

typedef int (*aos_scm_field_set_tws_gen_t)(
		struct aos_scm_field *field, 
		struct aos_rvg *gen);

typedef int (*aos_scm_field_set_map1_t)(
		struct aos_scm_field *field, 
		aos_value_map_t *);

typedef int (*aos_scm_field_set_map2_t)(
		struct aos_scm_field *field, 
		aos_value_map_t *);

typedef int (*aos_scm_field_set_cond_t)(
		struct aos_scm_field *field, 
		aos_cond_t *);

typedef struct aos_field * (*aos_scm_field_parse_new_t)(
		struct aos_scm_field *schema, 
		struct aos_omparser *parser, 
		int *is_eof);

typedef int (*aos_scm_field_parse_from_buff_t)(
		struct aos_scm_field *schema, 
		struct aos_buffer *buff, 
		struct aos_field **field);

typedef int (*aos_scm_field_data_rcvd_t)(
		struct aos_scm_field *schema, 
		struct aos_buffer *buff, 
		struct aos_field **msg);

typedef int (*aos_scm_field_parse_t)(
		struct aos_scm_field *schema, 
		struct aos_field *field); 

typedef int (*aos_scm_field_serialize_t)(
		struct aos_scm_field *, 
		struct aos_xml_node *);

typedef int (*aos_scm_field_deserialize_t)(
		struct aos_scm_field *, 
		struct aos_xml_node *);

typedef const char * (*aos_scm_field_dump_t)(
		struct aos_scm_field *field);

typedef int (*aos_scm_field_dump_to_t)(
		struct aos_scm_field *field, 
		char *buff, 
		int *buff_len);

typedef int (*aos_scm_field_destroy_t)(
		struct aos_scm_field *field);

typedef int (*aos_scm_field_hold_t)(
		struct aos_scm_field *field);

typedef int (*aos_scm_field_put_t)(
		struct aos_scm_field *field);

typedef int (*aos_scm_field_ic_t)(
		struct aos_scm_field *field); 

typedef int (*aos_scm_field_reset_parsing_t)(
		struct aos_scm_field *field, 
		const int start, 
		const int len);

#define AOS_SCM_FIELD_MEMFUNC_DECL 							\
	aos_scm_field_hold_t			hold;					\
	aos_scm_field_put_t				put;					\
	aos_scm_field_set_present_t 	set_present;			\
	aos_scm_field_set_msg_selector_t set_msg_selector;		\
	aos_scm_field_add_child_t 		add_child;				\
	aos_scm_field_set_lws_gen_t 	set_lws_gen;			\
	aos_scm_field_set_tws_gen_t 	set_tws_gen;			\
	aos_scm_field_set_lws_t   		set_lws;				\
	aos_scm_field_set_tws_t   		set_tws;				\
	aos_scm_field_set_map1_t    	set_map1;				\
	aos_scm_field_set_map2_t    	set_map2;				\
	aos_scm_field_set_cond_t    	set_cond;				\
	aos_scm_field_select_msg_t		select_msg;				\
	aos_scm_field_serialize_t		serialize;				\
	aos_scm_field_deserialize_t		deserialize;			\
	aos_scm_field_parse_new_t		parse_new;				\
	aos_scm_field_parse_from_buff_t	parse_from_buff;		\
	aos_scm_field_data_rcvd_t		data_rcvd;				\
	aos_scm_field_parse_t			parse;					\
	aos_scm_field_dump_t			dump;					\
	aos_scm_field_dump_to_t			dump_to;				\
	aos_scm_field_ic_t				integrity_check;		\
	aos_scm_field_destroy_t			destroy;				\
	aos_scm_field_reset_parsing_t	reset_parsing


#define AOS_SCM_FIELD_MEMFUNC_INIT							\
	aos_scm_field_set_msg_selector,							\
	aos_scm_field_add_child,								\
	aos_scm_field_set_lws_gen,								\
	aos_scm_field_set_tws_gen,								\
	aos_scm_field_set_lws,									\
	aos_scm_field_set_tws,									\
	aos_scm_field_set_map1,									\
	aos_scm_field_set_map2,									\
	aos_scm_field_set_cond


typedef struct aos_scm_field_mf
{
	AOS_SCM_FIELD_MEMFUNC_DECL;
} aos_scm_field_mf_t;

#define AOS_SCM_FIELD_MEMDATA_DECL							\
 	aos_scm_field_type_e    type;                           \
	char *              	name;                           \
	struct aos_msg_selector * msg_selector;					\
 	int                 	start_pos;                      \
 	int                 	len;                            \
	aos_len_unit_e			len_unit;						\
 	aos_len_type_e      	len_type;                       \
 	aos_data_type_e     	data_type;						\
	int						min_len;						\
	int						max_len;						\
	char					need_trim_lws;					\
	char			  * 	lws;							\
	int						lws_len;						\
	char					need_trim_tws;					\
	char			  * 	tws;							\
	int						tws_len;						\
	char					case_sensitivity;				\
	struct aos_rvg * 		lws_gen;						\
	struct aos_rvg * 		tws_gen;						\
	struct aos_value_map * 	value_map1;						\
	struct aos_value_map * 	value_map2;						\
	aos_presence_e			presence_type;					\
	struct aos_cond * 		presence_cond;					\
	struct aos_scm_field ** fields;							\
	struct aos_dyn_array	fields_array;					\
	char					is_mandatory;					\
	char					is_present;						\
	char					mandatory_first;				\
	char					mandatory_ordered;				\
	char					optional_ordered;				\
	int						ref_count


typedef struct aos_scm_field
{
	aos_scm_field_mf_t *mf;
	AOS_SCM_FIELD_MEMDATA_DECL;
} aos_scm_field_t;

extern int aos_scm_field_set_msg_selector(
		struct aos_scm_field *schema, 
		struct aos_msg_selector *selector);

extern int aos_scm_field_set_present(
		struct aos_scm_field *schema, 
		const char value);

extern int aos_scm_field_add_child(
		struct aos_scm_field *schema, 
		struct aos_scm_field *child);

extern int aos_scm_field_set_lws(
		struct aos_scm_field *scm_field, 
		char *lws, 
		const u32 lws_len);

extern int aos_scm_field_set_tws(
		struct aos_scm_field *scm_field, 
		char *lws, 
		const u32 lws_len);

extern int aos_scm_field_set_lws_gen(
		struct aos_scm_field *field, 
		struct aos_rvg *gen);

extern int aos_scm_field_set_tws_gen(
		struct aos_scm_field *field, 
		struct aos_rvg *gen);

extern int aos_scm_field_set_map1(
		struct aos_scm_field *field, 
		aos_value_map_t *map);

extern int aos_scm_field_set_map2(
		struct aos_scm_field *field, 
		aos_value_map_t *map);

extern int aos_scm_field_set_cond(
		struct aos_scm_field *field, 
		aos_cond_t *cond);

extern int aos_scm_field_serialize(
		struct aos_scm_field *, 
		struct aos_xml_node *);

extern int aos_scm_field_deserialize(
		struct aos_scm_field *, 
		struct aos_xml_node *);

extern int aos_scm_field_integrity_check(
		struct aos_scm_field *);

extern int aos_scm_field_destroy(
		struct aos_scm_field *field);

extern int aos_scm_field_reset_parsing(
		struct aos_scm_field *field, 
		const int start, 
		const int len);

// 
// Global functions
//
extern aos_scm_field_t *aos_scm_field_factory(struct aos_xml_node *);

extern int aos_scm_field_put(aos_scm_field_t *field);
extern int aos_scm_field_hold(aos_scm_field_t *field);

extern int aos_scm_field_init(
	aos_scm_field_t *field,
	const aos_scm_field_type_e type, 
	const char * const name, 
	const aos_len_unit_e len_unit,
	const aos_len_type_e len_type, 
	const aos_data_type_e data_type,
	const aos_presence_e presence_type);

#endif

