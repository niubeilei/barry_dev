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
#ifndef aos_omni_parser_parser_h
#define aos_omni_parser_parser_h

#include "parser/types.h"
#include "parser/xml.h"
#include "util_c/types.h"
#include "util2/value.h"

#define aos_parser_default_ws_len 2
#define aos_parser_ws1_len 5
extern char aos_parser_default_ws[aos_parser_default_ws_len];
extern char aos_parser_ws1[aos_parser_ws1_len];

struct aos_omparser;
struct aos_value;
struct aos_scm_field;

// 
// Member function signature definitions
//
typedef int (*aos_omparser_put_t)(
		struct aos_omparser *parser);

typedef int (*aos_omparser_hold_t)(
		struct aos_omparser *parser);

typedef int (*aos_omparser_release_memory_t)(
		struct aos_omparser *parser);

typedef int (*aos_omparser_destroy_t)(
		struct aos_omparser *parser);

typedef int (*aos_omparser_trim_ws1_t)(
		struct aos_omparser *parser);

typedef int (*aos_omparser_trim_ws_t)(
				struct aos_omparser *parser, 
				const char * const ws, 
				const int ws_len);

typedef int (*aos_omparser_expect_t)(
				struct aos_omparser *parser, 
				const int start_pos, 
				const char * const value, 
				const u32 value_len, 
				const char case_sensitive, 
				const u8 move);

typedef int (*aos_omparser_expect_value_t)(
				struct aos_omparser *parser, 
				const aos_data_type_e data_type, 
				struct aos_value **value); 

typedef int (*aos_omparser_next_word_t)(
				struct aos_omparser *parser, 
				const char * const lws, 
				const int lws_len,
				const char * const terminators,
				const int term_len, 
				char **word, 
				int *word_len);

typedef int (*aos_omparser_next_word_b_t)(
				struct aos_omparser *parser, 
				const char * const lws, 
				const int lws_len,
				const char * const terminators,
				const int term_len, 
				char *word, 
				int *word_len);

typedef int (*aos_omparser_set_schema_t)(
				struct aos_omparser *parser, 
				struct aos_scm_field *schema);

typedef struct aos_field * (*aos_omparser_parse_first_msg_t)(
				struct aos_omparser *parser, 
				int *is_eof);

typedef struct aos_field *(*aos_omparser_parse_next_msg_t)(
				struct aos_omparser *parser, 
				int *is_eof);

typedef int (*aos_omparser_get_str_t)(
				struct aos_omparser *parser, 
				const int offset, 
				const int offset_type,
				const int len, 
				char **contents);	

typedef int (*aos_omparser_get_str_w_stops_t)(
				struct aos_omparser *parser, 
				const int offset, 
				const int offset_type,
				const int min_len, 
				const char * const stop_chars,
				char **contents, 
				int *len);	

typedef char *(*aos_omparser_dump_t)(
				struct aos_omparser *parser);

typedef char (*aos_omparser_peek_next_char_t)(
				struct aos_omparser *parser);

typedef int (*aos_omparser_move_to_close_tag_t)(
				struct aos_omparser *parser, 
				const char * const tag, 
				int *start, 
				int *len); 

typedef int (*aos_omparser_search_word_t)(
		struct aos_omparser *parser, 
		const char * const word, 
		const int word_len, 
		const char * const terminators,
		const int term_len, 
		const char case_sensitive);

#define AOS_OMPARSER_BUFF_MAX_SIZE 300000000

#define AOS_OMPARSER_MEMFUNC_DECL						\
	aos_omparser_hold_t				hold; 				\
	aos_omparser_put_t				put; 				\
	aos_omparser_release_memory_t	release_memory;		\
	aos_omparser_destroy_t			destroy;			\
	aos_omparser_trim_ws1_t			trim_ws1; 			\
	aos_omparser_trim_ws_t			trim_ws; 			\
	aos_omparser_expect_t			expect;				\
	aos_omparser_expect_value_t		expect_value;		\
	aos_omparser_next_word_t		next_word;			\
	aos_omparser_next_word_b_t		next_word_b;		\
	aos_omparser_set_schema_t		set_schema;			\
	aos_omparser_parse_first_msg_t	parse_first_msg;	\
	aos_omparser_parse_next_msg_t	parse_next_msg;		\
	aos_omparser_get_str_t			get_str;			\
	aos_omparser_get_str_w_stops_t	get_str_w_stops;	\
	aos_omparser_dump_t				dump;				\
	aos_omparser_peek_next_char_t	peek_next_char;		\
	aos_omparser_move_to_close_tag_t move_to_close_tag;	\
	aos_omparser_search_word_t		search_word

#define AOS_OMPARSER_MEMFUNC_INIT						\
	aos_omparser_trim_ws1,								\
	aos_omparser_trim_ws,								\
	aos_omparser_expect,								\
	aos_omparser_expect_value,							\
	aos_omparser_next_word,								\
	aos_omparser_next_word_b,							\
	aos_omparser_set_schema,							\
	aos_omparser_parse_first_msg,						\
	aos_omparser_parse_next_msg,						\
	aos_omparser_get_str,								\
	aos_omparser_get_str_w_stops,						\
	aos_omparser_dump,									\
	aos_omparser_peek_next_char,						\
	aos_omparser_move_to_close_tag,						\
	aos_omparser_search_word

typedef struct aos_omparser_mf
{
	AOS_OMPARSER_MEMFUNC_DECL;
	AOS_OMPARSER_XML_MEMFUNC_DECL;
} aos_omparser_mf_t;

typedef struct aos_xml_parser_mf
{
	AOS_OMPARSER_XML_MEMFUNC_DECL;
} aos_xml_omparser_mf_t;

typedef struct aos_omparser
{
	aos_omparser_mf_t	*mf;

	struct aos_scm_field * schema;
	u8			all_contents_read;
	char 	  *	buffer;
	u32			data_len;
	int			crt_pos;
	int			parent_start;
	int			ref_count;
	int			is_destroying;
} aos_omparser_t;

extern int aos_omparser_init(
		aos_omparser_t *parser, 
		char * contents, 
		const int len);

extern int aos_omparser_expect_value(
				struct aos_omparser *parser, 
				const aos_data_type_e data_type, 
				struct aos_value **value); 

extern aos_omparser_t * aos_omparser_create_from_file(const char * const fn);
extern aos_omparser_t * aos_omparser_create_from_str(
		const char * const str, 
		const int len);
#endif

