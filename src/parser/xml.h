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
// 01/29/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_parser_xml_h
#define aos_omni_parser_xml_h

#include "parser/types.h"

struct aos_omparser;
struct aos_xml_node;
struct aos_value;
struct aos_attr1;

#define AOS_XML_LABEL_MAX_LEN 100

typedef int (*aos_xml_parser_next_named_tag_t)(
				struct aos_omparser *parser, 
				const char * const tag,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr); 

typedef int (*aos_xml_parser_next_tag_t)(
				struct aos_omparser *parser, 
				char *tag,
				const int tag_len, 
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr); 

typedef int (*aos_xml_parser_first_named_child_t)(
				struct aos_omparser *parser, 
				const char * const tag,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr);

// typedef int (*aos_xml_parser_first_child_t)(
// 				struct aos_omparser *parser, 
// 				char *tag,
// 				const int tag_len,
// 				int *start_pos, 
// 				int *len);

typedef int (*aos_xml_parser_next_named_sibling_t)(
				struct aos_omparser *parser, 
				const char * const tag,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr);

typedef int (*aos_xml_parser_next_sibling_t)(
				struct aos_omparser *parser, 
				char * tag,
				const int tag_len,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr);

typedef int (*aos_xml_parser_get_descendent_t)(
				struct aos_omparser *parser, 
				const char * const tag,
				int *start_pos, 
				int *len);

typedef int (*aos_xml_parser_expect_open_tag_t)(
				struct aos_omparser *parser, 
				char ** label, 
				struct aos_attr1 ***attr, 
				int *num_attr); 

typedef int (*aos_xml_parser_parse_attr_t)(
		struct aos_omparser *parser, 
		struct aos_attr1 ***attr, 
		int *num_attr); 

#define AOS_OMPARSER_XML_MEMFUNC_INIT							\
	aos_xml_parser_next_named_tag,								\
	aos_xml_parser_next_tag,									\
	aos_xml_parser_first_named_child,							\
	aos_xml_parser_next_named_sibling,							\
	aos_xml_parser_next_sibling,								\
	aos_xml_parser_get_descendent,								\
	aos_xml_parser_expect_open_tag,								\
	aos_xml_parser_parse_attr

#define AOS_OMPARSER_XML_MEMFUNC_DECL							\
	aos_xml_parser_next_named_tag_t       next_named_tag;		\
	aos_xml_parser_next_tag_t             next_tag;				\
	aos_xml_parser_first_named_child_t    first_named_child;	\
	aos_xml_parser_next_named_sibling_t   next_named_sibling;	\
	aos_xml_parser_next_sibling_t         next_sibling;			\
	aos_xml_parser_get_descendent_t       get_descendent;		\
	aos_xml_parser_expect_open_tag_t	  expect_open_tag;		\
	aos_xml_parser_parse_attr_t			  parse_attr

extern int aos_xml_parser_next_named_tag(
				struct aos_omparser *parser, 
				const char * const tag,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr);

extern int aos_xml_parser_next_tag(
				struct aos_omparser *parser, 
				char *tag,
				const int tag_len,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr); 

extern int aos_xml_parser_first_named_child(
				struct aos_omparser *parser, 
				const char * const tag,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr);

// extern int aos_xml_parser_first_child(
// 				struct aos_omparser *parser, 
// 				char *tag,
// 				const int tag_len,
// 				int *start_pos, 
// 				int *len);

extern int aos_xml_parser_next_named_sibling(
				struct aos_omparser *parser, 
				const char * const tag,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr);

extern int aos_xml_parser_next_sibling(
				struct aos_omparser *parser, 
				char * tag,
				const int tag_len,
				int *start_pos, 
				int *len, 
				struct aos_attr1 ***attr, 
				int *num_attr);

extern int aos_xml_parser_get_descendent(
				struct aos_omparser *parser, 
				const char * const tag,
				int *start_pos, 
				int *len);

extern int aos_xml_parser_expect_open_tag(
				struct aos_omparser *parser, 
				char ** label, 
				struct aos_attr1 ***attr, 
				int *num_attr); 

extern int aos_xml_parser_parse_attr(
		struct aos_omparser *parser, 
		struct aos_attr1 ***attr, 
		int *num_attr);

#endif

