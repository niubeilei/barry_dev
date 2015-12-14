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
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_xml_node_h
#define aos_parser_xml_node_h

#include "parser/field.h"
#include "util_c/list.h"
#include "util_c/types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aos_omparser;
struct aos_xml_node;

// 
// Member Function Declarations
//

typedef struct aos_xml_node * (*aos_xml_node_first_child_t)(
		struct aos_xml_node *node); 

typedef struct aos_xml_node * (*aos_xml_node_first_named_child_t)(
		struct aos_xml_node *node, 
		const char * const name); 

typedef struct aos_xml_node * (*aos_xml_node_next_named_child_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		struct aos_omparser *parser); 

typedef struct aos_xml_node * (*aos_xml_node_next_sibling_t)(
		struct aos_xml_node *node);

typedef int (*aos_xml_node_del_first_named_child_t)(
		struct aos_xml_node *node, 
		const char * const label); 

typedef const char * (*aos_xml_node_dump_t)(
		struct aos_xml_node *node);

typedef int (*aos_xml_node_destroy_t)(
		struct aos_xml_node *node);

typedef int (*aos_xml_node_first_child_int_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		int *value);

typedef int (*aos_xml_node_first_child_u32_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		u32 *value);

typedef int (*aos_xml_node_first_child_int_dft_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		int *value, 
		const int dft_value);

typedef int (*aos_xml_node_first_child_addr_dft_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		u32 *value, 
		const u32 dft_value);

typedef int (*aos_xml_node_first_child_char_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		char *value);

typedef int (*aos_xml_node_first_child_char_dft_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		char *value, 
		const char dft_value);

typedef int (*aos_xml_node_first_child_hex_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len);

typedef int (*aos_xml_node_first_child_hex_dft_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		const char * const dft_value, 
		const int dft_len);

typedef int (*aos_xml_node_first_child_str_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		struct aos_xml_node **child_found); 

typedef int (*aos_xml_node_first_child_str_b_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		char *contents, 
		int *contents_len, 
		struct aos_xml_node **child_found); 

typedef int (*aos_xml_node_first_child_str_dft_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		const char * const dft_value); 

typedef int (*aos_xml_node_first_child_str_b_dft_t)(
		struct aos_xml_node *node, 
		const char * const name, 
		char *contents, 
		int *len, 
		const char * const dft_value); 

typedef int (*aos_xml_node_append_child_int_t)(
		struct aos_xml_node *node, 
		const char * const label,
		const int value);

typedef int (*aos_xml_node_append_child_addr_t)(
		struct aos_xml_node *node, 
		const char * const label,
		const u32 value);

typedef int (*aos_xml_node_append_child_hex_t)(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents,
		const int len);

typedef int (*aos_xml_node_append_child_bin_t)(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents,
		const int len);

typedef int (*aos_xml_node_append_child_str_t)(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents, 
		struct aos_attr1 **attrs, 
		const int num_attrs);

typedef struct aos_xml_node * (*aos_xml_node_append_child_node_t)(
		struct aos_xml_node *node, 
		const char * const label);

typedef int (*aos_xml_node_append_child_value_t)(
		struct aos_xml_node *node, 
		const char * const label, 
		struct aos_value *value);

#define AOS_XML_NODE_MEMFUNC_DECL								\
	aos_xml_node_first_child_t				first_child;		\
	aos_xml_node_first_named_child_t		first_named_child;	\
	aos_xml_node_next_named_child_t			next_named_child;	\
	aos_xml_node_next_sibling_t				next_sibling;		\
	aos_xml_node_del_first_named_child_t	del_first_child;	\
	aos_xml_node_first_child_int_t			first_child_int;	\
	aos_xml_node_first_child_u32_t			first_child_u32;	\
	aos_xml_node_first_child_int_dft_t		first_child_int_dft;\
	aos_xml_node_first_child_char_t			first_child_char;	\
	aos_xml_node_first_child_char_dft_t		first_child_char_dft;	\
	aos_xml_node_first_child_str_t			first_child_str;	\
	aos_xml_node_first_child_str_b_t		first_child_str_b;	\
	aos_xml_node_first_child_str_dft_t		first_child_str_dft;	\
	aos_xml_node_first_child_str_b_dft_t	first_child_str_b_dft;	\
	aos_xml_node_first_child_hex_t			first_child_hex;	\
	aos_xml_node_first_child_hex_dft_t		first_child_hex_dft;	\
	aos_xml_node_first_child_addr_dft_t		first_child_addr_dft;	\
	aos_xml_node_append_child_int_t			append_child_int;	\
	aos_xml_node_append_child_hex_t			append_child_hex;	\
	aos_xml_node_append_child_str_t			append_child_str;	\
	aos_xml_node_append_child_bin_t			append_child_bin;	\
	aos_xml_node_append_child_node_t		append_child_node;	\
	aos_xml_node_append_child_addr_t		append_child_addr;	\
	aos_xml_node_append_child_value_t		append_child_value;	


#define AOS_XML_NODE_MEMFUNC_INIT								\
	aos_xml_node_first_child,									\
	aos_xml_node_first_named_child,								\
	aos_xml_node_next_named_child,								\
	aos_xml_node_next_sibling,									\
	aos_xml_node_append_child,									\
	aos_xml_node_del_first_named_child,							\
	aos_xml_node_first_child_int,								\
	aos_xml_node_first_child_u32,								\
	aos_xml_node_first_child_int_dft,							\
	aos_xml_node_first_child_char,								\
	aos_xml_node_first_child_char_dft,							\
	aos_xml_node_first_child_str,								\
	aos_xml_node_first_child_str_b,								\
	aos_xml_node_first_child_str_dft,							\
	aos_xml_node_first_child_str_b_dft,							\
	aos_xml_node_first_child_hex,								\
	aos_xml_node_first_child_hex_dft,							\
	aos_xml_node_append_child_int,								\
	aos_xml_node_append_child_hex,								\
	aos_xml_node_append_child_str,								\
	aos_xml_node_append_child_bin,								\
	aos_xml_node_append_child_node,								\
	aos_xml_node_append_child_addr,								\
	aos_xml_node_append_child_value
	

#define AOS_XML_NODE_MEMDATA_DECL						\
	struct aos_xml_node*	parent;						\
	int						crt_child

typedef struct aos_xml_node_mf
{
	AOS_FIELD_MEMFUNC_DECL;
	AOS_XML_NODE_MEMFUNC_DECL;
} aos_xml_node_mf_t;

typedef struct aos_xml_node
{
	aos_xml_node_mf_t *mf;

	AOS_FIELD_MEMDATA_DECL;
	AOS_XML_NODE_MEMDATA_DECL;
} aos_xml_node_t;


extern struct aos_xml_node * aos_xml_node_first_child(
		struct aos_xml_node *node);

extern struct aos_xml_node * aos_xml_node_first_named_child(
		struct aos_xml_node *node, 
		const char * const name); 

extern struct aos_xml_node * aos_xml_node_next_named_child(
		struct aos_xml_node *node, 
		const char * const name, 
		struct aos_omparser *parser); 

extern struct aos_xml_node * aos_xml_node_next_sibling(
		struct aos_xml_node *node); 

extern int aos_xml_node_append_child(
		struct aos_xml_node *node, 
		struct aos_xml_node *child); 

extern int aos_xml_node_del_first_named_child(
		struct aos_xml_node *node, 
		const char * const label); 

extern const char * aos_xml_node_dump(
		struct aos_field *node);

extern int aos_xml_node_destroy(
		struct aos_field *node);

extern int aos_xml_node_first_child_int(
		struct aos_xml_node *node, 
		const char * const name, 
		int *value);

extern int aos_xml_node_first_child_int_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		int *value, 
		const int dft_value);

extern int aos_xml_node_first_child_char(
		struct aos_xml_node *node, 
		const char * const name, 
		char *value);

extern int aos_xml_node_first_child_char_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		char *value, 
		const char dft_value);

extern int aos_xml_node_first_child_hex(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len);

extern int aos_xml_node_first_child_hex_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		const char * const dft_value, 
		const int dft_len);

extern int aos_xml_node_first_child_str(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		struct aos_xml_node **child_found); 

extern int aos_xml_node_first_child_str_b(
		struct aos_xml_node *node, 
		const char * const name, 
		char *contents, 
		int *contents_len, 
		struct aos_xml_node **child_found); 

extern int aos_xml_node_first_child_str_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		char **contents, 
		int *len, 
		const char * const dft_value); 

extern int aos_xml_node_first_child_str_b_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		char *contents, 
		int *len, 
		const char * const dft_value); 

extern int aos_xml_node_append_child_int(
		struct aos_xml_node *node, 
		const char * const label,
		const int value);

extern int aos_xml_node_append_child_hex(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents,
		const int len);

extern int aos_xml_node_append_child_bin(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents,
		const int len);

extern int aos_xml_node_append_child_str(
		struct aos_xml_node *node, 
		const char * const label,
		const char * const contents, 
		struct aos_attr1 **attrs, 
		const int num_attrs);

extern struct aos_xml_node * aos_xml_node_append_child_node(
		struct aos_xml_node *node, 
		const char * const label);

extern int aos_xml_node_init(
		aos_xml_node_t *node,
		const aos_field_type_e type,
		const char * const name, 
		const char * const contents, 
		const int len);

extern aos_xml_node_t *  aos_xml_node_create1(
		const char * const label);

extern aos_xml_node_t * aos_xml_node_create3(
		const char * const label,
		struct aos_scm_field *schema, 
		const char * const contents, 
		const int len);

extern aos_xml_node_t *  aos_xml_node_create2(
		const char * const label, 
		const int start, 
		const int len, 
		struct aos_scm_field *schema, 
		struct aos_omparser *parser);

extern aos_xml_node_t *  aos_xml_node_create_from_file(
		const char * const fn, 
		const int with_root);

extern aos_xml_node_t * aos_xml_node_create_from_str(
		struct aos_scm_field *schema,
		const char * const str, 
		int *str_len, 
		int *is_incomplete, 
		const int with_root);

extern int aos_xml_node_cleanup(aos_xml_node_t *node);

extern int aos_xml_node_serialize(
		aos_field_t *msg, 
		struct aos_xml_node *parent);

extern int aos_xml_node_deserialize(
		aos_field_t *msg, 
		struct aos_xml_node *node);

extern int aos_xml_node_integrity_check(
		struct aos_field *field);

extern int aos_xml_node_parse(struct aos_field *field); 

extern int aos_xml_node_get_value(
		aos_field_t *node, 
		aos_value_t *value);

extern int aos_xml_node_first_child_addr_dft(
		struct aos_xml_node *node, 
		const char * const name, 
		u32 *value, 
		const u32 dft_value);

extern int aos_xml_node_first_child_u32(
		struct aos_xml_node *node, 
		const char * const name, 
		u32 *value);

#define AOS_XML_NODE_CHILDREN_INC_SIZE 10

#ifdef __cplusplus
}
#endif

#endif

