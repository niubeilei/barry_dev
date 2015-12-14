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
// This is a fixed content selector. The content it will select is
// (offset, len), which means the contents start 'offset' number of 
// bytes from the parser's current position. The contents length is 'len'. 
//   
//
// Modification History:
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/cs_fixed.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "util_c/strutil.h"
#include "util_c/memory.h"


static int aos_cs_fixed_select_w_conts(
			struct aos_cont_selector *cs, 
			struct aos_omparser *parser, 
			char **contents, 
			int *len)
{
	aos_assert_r(cs, -1);
	aos_assert_r(parser, -1);
	aos_assert_r(cs->type == eAosCSType_Fixed, -1);
	aos_cs_fixed_t *self = (aos_cs_fixed_t *)cs;
	aos_assert_r(!aos_content_type_check(self->cont_type), -1);
	if (!parser->mf->get_str(parser, self->offset, self->offset_type, 
			self->len, contents))
	{
		aos_alarm("Expecting contents: %d:%d:%d but failed: %s", 
			self->offset, self->len, self->offset_type, 
			parser->mf->dump(parser));
		return -1;
	}

	switch (self->cont_type)
	{
	case eAosContentType_Binary:
	case eAosContentType_CharStrCS:
		 return (memcmp(contents, self->expected, self->len) == 0);

	case eAosContentType_CharStrCI:
		 return (strcasecmp(*contents, self->expected) == 0);

	default:
		 aos_alarm("Unreconized content type: %d", self->cont_type);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


static int aos_cs_fixed_select(
			struct aos_cont_selector *cs, 
			struct aos_omparser *parser) 
{
	char *contents; 
	int len;
	int ret = aos_cs_fixed_select_w_conts(cs, parser, &contents, &len);
	if (!ret)
	{
		aos_free(contents);
	}
	return ret;	
}


static int aos_cs_fixed_serialize(
		aos_cont_selector_t *cs, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<CS_Fixed>		// This object starts here
	// 			<offset>
	// 			<offset_type>
	// 			<len>
	// 			<expected>
	// 		</CS_Fixed>
	//
	//
	aos_assert_r(cs, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(cs->type == eAosCSType_Fixed, -1);

	aos_cs_fixed_t *self = (aos_cs_fixed_t *)cs;
	aos_xml_node_t *node = aos_xml_node_create1("CS_Fixed");
	aos_assert_r(node, -1);
	aos_assert_g(!node->mf->append_child_int(node, 
		"offset", self->offset), cleanup);
	aos_assert_g(!node->mf->append_child_int(node, 
		"offset_type", self->offset_type), cleanup);
	aos_assert_g(!node->mf->append_child_int(node, 
		"len", self->len), cleanup);
	aos_assert_g(!node->mf->append_child_bin(node, 
		"expected", self->expected, self->expected_len), cleanup);

	aos_assert_g(!parent->mf->add_child((aos_field_t *)parent, 
				(aos_field_t *)node), cleanup);

	return 0;

cleanup:
	if (node) aos_free(node);
	return -1;
}
			

static int aos_cs_fixed_deserialize(
		aos_cont_selector_t *cs, 
		aos_xml_node_t *node)
{
	// 
	// 	<CS_Fixed>
	// 		<offset>
	// 		<offset_type>
	// 		<len>
	// 		<expected>
	// 	</CS_Fixed>
	//
	aos_assert_r(cs, -1);
	aos_assert_r(node, -1);
	aos_assert_r(node->mf->match_name((aos_field_t *)node, "CS_Fixed"), -1);
	aos_assert_r(cs->type == eAosCSType_Fixed, -1);
	aos_cs_fixed_t *self = (aos_cs_fixed_t *)cs;

	aos_assert_r(!node->mf->first_child_int(node, "offset", 
		&self->offset), -1);
	aos_assert_r(!node->mf->first_child_char(node, "offset_type", 
		&self->offset_type), -1);
	aos_assert_r(!node->mf->first_child_int(node, "len", 
		&self->len), -1);
	aos_assert_r(!node->mf->first_child_str(node, "expected", 
		&self->expected, &self->expected_len, 0), -1);
	return 0;
}


static aos_cs_fixed_mf_t sg_mf = 
{
	aos_cs_fixed_select_w_conts,
	aos_cs_fixed_select,
	aos_cs_get_selected,
	aos_cs_fixed_serialize,
	aos_cs_fixed_deserialize
};


aos_cs_fixed_t * aos_cs_fixed_create(
		const int offset, 
		const aos_parser_offset_type_e offset_type, 
		const int len)
{
	aos_assert_r(!aos_offset_type_check(offset_type), 0);
	aos_str_len_assert_r(len, 0);

	aos_cs_fixed_t *cs = aos_malloc(sizeof(aos_cs_fixed_t));
	aos_assert_r(cs, 0);
	memset(cs, 0, sizeof(aos_cs_fixed_t));

	aos_assert_g(aos_cont_selector_init((aos_cont_selector_t *)cs, 
	 		eAosCSType_Fixed), cleanup);

	cs->offset = offset;
	cs->offset_type = offset_type;
	cs->len = len;
	cs->mf = &sg_mf;
	return cs;

cleanup:
	aos_free(cs);
	return 0;
}

