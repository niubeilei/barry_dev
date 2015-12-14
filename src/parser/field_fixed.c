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
#include "parser/field_fixed.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include <string.h>



int aos_field_fixed_serialize(
		struct aos_field *field, 
		struct aos_xml_node *parent)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_field_fixed_deserialize(
		struct aos_field *field, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_field_fixed_dump_to(
		struct aos_field *field, 
		char *buff, 
		int *len)
{
	aos_not_implemented_yet;
	return -1;
}


/*
// 
// Description:
// 	This is the function to parse the input. 
//
static int aos_field_fixed_parse(aos_field_t *field)
{
	aos_assert_r(field, -1);
	aos_assert_r(parser, -1);
	aos_assert_r(field->type == eAosFieldType_Fixed, -1);
	aos_field_fixed_t *self = (aos_field_fixed_t *)field;

	aos_assert_r(self->value, -1);
	aos_assert_r(self->value_len > 0, -1);
	if (self->need_trim_lws)
	{
		aos_assert_r(!parser->mf->trim_ws(parser, self->lws), -1);
	}

	aos_assert_r(!parser->mf->expect(parser, self->start_pos, 
			self->value, self->value_len, self->case_sensitive), -1);

	if (self->need_trim_tws)
	{
		aos_assert_r(!parser->mf->trim_ws(parser, self->tws), -1);
	}

	return 0;
}
*/


static aos_field_fixed_mf_t sg_mf = 
{
	AOS_FIELD_MEMFUNC_INIT,
	aos_field_fixed_dump_to,
	aos_field_fixed_dump, 
	aos_field_fixed_destroy, 
	aos_field_fixed_serialize, 
	aos_field_fixed_deserialize,
	aos_field_fixed_integrity_check, 
	aos_field_fixed_parse,
};


aos_field_fixed_t *
aos_field_fixed_create(const aos_field_type_e type, 
					 const char * const name,
					 const aos_len_type_e len_type,
					 const aos_presence_e presence,
					 const aos_data_type_e data_type, 
					 const char * const value, 
					 const u32 value_len)
{
	aos_field_fixed_t *field = aos_malloc(sizeof(aos_field_fixed_t));
	aos_assert_r(field, 0);
	memset(field, 0, sizeof(aos_field_fixed_t));
	field->mf = &sg_mf;

	if (aos_field_init((aos_field_t*)field, type, name, data_type, value, value_len))
	{
		goto failed;
	}

	return field;

failed:
	aos_free(field);
	return 0;
}


