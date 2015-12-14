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
// This field parses the current string until one of the characters 
// defined in stop_chars is hit.  
//
// Modification History:
// 01/29/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/field_var.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include <string.h>


static int aos_field_var_serialize(
		struct aos_field *field, 
		struct aos_xml_node *parent)
{
	aos_not_implemented_yet;
	return -1;
}


static int aos_field_var_deserialize(
		struct aos_field *field, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return -1;
}


static const char *aos_field_var_dump(
		struct aos_field *field) 
{
	aos_not_implemented_yet;
	return 0;
}


// 
// Description:
// 	This is the function to parse the input. 
//
static int aos_field_var_parse(aos_field_t *field)
{
	/*
	char *buff;
	char *stop_chars;
	int crt_pos, start, max_len, stop_len, buff_len;
	aos_assert_r(field, -1);
	aos_assert_r(parser, -1);
	aos_assert_r(field->type == eAosFieldType_Var, -1);
	aos_field_var_t *self = (aos_field_var_t *)field;

	if (self->need_trim_lws)
	{
		aos_assert_r(!parser->mf->trim_ws(parser, self->lws, 
				self->lws_len), -1);
	}

	if (self->stop_chars)
	{
		stop_chars = self->stop_chars;
		stop_len = self->stop_len;
		aos_assert_r(stop_len > 0, -1);
	}
	else
	{
		stop_chars = aos_parser_default_ws;
		stop_len = aos_parser_default_ws_len;
	}

	aos_assert_r(stop_chars, -1);
	aos_assert_r(stop_len > 0, -1);
	buff = parser->buffer;
	crt_pos = parser->crt_pos;
	start = crt_pos;
	buff_len = parser->data_len;
	max_len = field->max_len;
	while (crt_pos < buff_len)
	{
		if (aos_str_in(stop_chars, buff[crt_pos], stop_len) == 1)
		{
			// 
			// Found the field value
			//
			self->orig_value_len = crt_pos - start;

			// 
			// Make sure it is no less than the minimum
			//
			aos_assert_m(self->orig_value_len >= field->min_len, -1,
					"Parsing field: %s but it is less than min: %d:%d", 
					field->name, field->min_len, self->orig_value_len);

			aos_assert_r(!aos_str_set(&self->orig_value, &buff[start], 
					self->orig_value_len), -1);

			if (field->need_trim_tws)
			{
				aos_assert_r(!parser->mf->trim_ws(parser, field->tws), -1);
			}
		}

		// 
		// Make sure it is no longer than the maximum
		//
		if (self->max_len > 0)
		{
			aos_assert_m(self->orig_value_len < self->max_len, -1, 
				"Parsing field: %s but it is longer than max: %d:%d", 
				field->name, field->max_len, self->orig_value_len);
		}
		crt_pos++;
	}
	*/
	aos_not_implemented_yet;
	return -1;
}


int aos_field_var_dump_to(
		struct aos_field *field, 
		char *buff, 
		int *len)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_field_var_destroy(
		struct aos_field *field)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_field_var_integrity_check(
		struct aos_field *field)
{
	aos_not_implemented_yet;
	return -1;
}


static aos_field_var_mf_t sg_mf = 
{
	AOS_FIELD_MEMFUNC_INIT,
	aos_field_var_dump_to,
	aos_field_var_dump, 
	aos_field_var_destroy, 
	aos_field_var_serialize, 
	aos_field_var_deserialize,
	aos_field_var_integrity_check, 
	aos_field_var_parse
};


aos_field_var_t *
aos_field_var_create(const aos_field_type_e type, 
					 const char * const name,
					 const aos_len_type_e len_type,
					 const aos_data_type_e data_type, 
					 const char * const stop_chars, 
					 const u32 stop_len)
{
	aos_field_var_t *field = aos_malloc(sizeof(aos_field_var_t));
	aos_assert_r(field, 0);
	memset(field, 0, sizeof(aos_field_var_t));

	if (aos_field_init((aos_field_t*)field, type, name, data_type, 0, 0))
	{
		goto failed;
	}

	if (aos_str_set(&field->stop_chars, stop_chars, stop_len)) goto failed;
	field->stop_len = stop_len;
	field->mf = &sg_mf;
	field->orig_value_len = -1;
	return field;

failed:
	aos_free(field);
	return 0;
}

