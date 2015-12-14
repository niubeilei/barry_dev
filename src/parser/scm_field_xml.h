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
// 02/08/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_scm_field_xml_h
#define aos_parser_scm_field_xml_h

#include "parser/scm_field.h"

struct aos_scm_field;
struct aos_scm_field_xml;
struct aos_buffer;

typedef struct aos_scm_field *(*aos_scm_field_xml_find_child_t)(
		struct aos_scm_field_xml *schema, 
		const char * const label);

typedef int (*aos_scm_field_xml_field_allowed_t)(
		struct aos_scm_field_xml *schema, 
		struct aos_field *message, 
		struct aos_field *field);

#define AOS_SCM_FIELD_XML_MEMFUNC_DECL						\
	aos_scm_field_xml_find_child_t 		find_child;			\
	aos_scm_field_xml_field_allowed_t	field_allowed

#define AOS_SCM_FIELD_XML_MEMDATA_DECL						\
	char *				tag_name;							\
	char				undefined_child_allowed;			\
	char				is_atomic;							\
	struct aos_buffer *	buffer

typedef struct aos_scm_field_xml_mf
{
	AOS_SCM_FIELD_MEMFUNC_DECL;
	AOS_SCM_FIELD_XML_MEMFUNC_DECL;
} aos_scm_field_xml_mf_t;


typedef struct aos_scm_field_xml
{
	aos_scm_field_xml_mf_t *mf;

	AOS_SCM_FIELD_MEMDATA_DECL;
	AOS_SCM_FIELD_XML_MEMDATA_DECL;
} aos_scm_field_xml_t;

extern aos_scm_field_xml_t *aos_scm_field_xml_create(
		struct aos_xml_node *node);

extern int aos_scm_field_xml_integrity_check(
		aos_scm_field_t *self);

extern struct aos_field * aos_scm_field_xml_parse_new(
		struct aos_scm_field *schema, 
		struct aos_omparser *parser, 
		int *is_eof); 

extern struct aos_scm_field *aos_scm_field_xml_find_child(
		struct aos_scm_field_xml *schema, 
		const char * const label);

extern aos_scm_field_xml_t *aos_scm_field_xml_create_default();
#endif

