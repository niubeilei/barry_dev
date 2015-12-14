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
#ifndef aos_parser_field_var_h
#define aos_parser_field_var_h

#include "parser/field.h"

typedef struct aos_field_var_mf
{
	AOS_FIELD_MEMFUNC_DECL;
} aos_field_var_mf_t;

#define AOS_FIELD_VAR_MEMFUNC_INIT				\
	aos_field_var_dump,							\
	aos_field_var_serialize,					\
	aos_field_var_deserialize

typedef struct aos_field_var
{
	aos_field_var_mf_t *mf;
	AOS_FIELD_MEMDATA_DECL;

	char *		stop_chars;
	u32			stop_len;
	char *		orig_value;
	int			orig_value_len;
	char		need_trim_lws;
	char 		need_trim_tws;
	char *		lws;
	int			lws_len;
	char *		tws;
	int 		tws_len;
} aos_field_var_t;

extern aos_field_var_t *
aos_field_var_create(const aos_field_type_e type, 
					 const char * const name,
					 const aos_len_type_e len_type,
					 const aos_data_type_e data_type, 
					 const char * const value, 
					 const u32 value_len);
#endif

