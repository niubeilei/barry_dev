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
#ifndef aos_omni_parser_field_fixed_h
#define aos_omni_parser_field_fixed_h

#include "parser/field.h"

typedef struct aos_field_fixed_mf
{
	AOS_FIELD_MEMFUNC_DECL;
} aos_field_fixed_mf_t;

#define AOS_FIELD_FIXED_MEMFUNC_INIT				\
	aos_field_fixed_dump,							\
	aos_field_fixed_serialize,						\
	aos_field_fixed_deserialize

typedef struct aos_field_fixed
{
	aos_field_fixed_mf_t *mf;
	AOS_FIELD_MEMDATA_DECL;
} aos_field_fixed_t;

extern int aos_field_fixed_dump_to(
		struct aos_field *field,
		char *buff,
		int *len);

extern const char * aos_field_fixed_dump(
		struct aos_field *field);

extern int aos_field_fixed_destroy(
		struct aos_field *field);

extern int aos_field_fixed_serialize(
		struct aos_field *field, 
		struct aos_xml_node *parent);

extern int aos_field_fixed_deserialize(
		struct aos_field *field, 
		struct aos_xml_node *node);

extern int aos_field_fixed_integrity_check(
		struct aos_field *field);

extern int aos_field_fixed_parse(struct aos_field *field); 


extern aos_field_fixed_t *
aos_field_fixed_create(const aos_field_type_e type, 
					 const char * const name,
					 const aos_len_type_e len_type,
					 const aos_presence_e presence,
					 const aos_data_type_e data_type, 
					 const char * const value, 
					 const u32 value_len);
#endif

