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
// This is a content selector that selects an integer based on the 
// contents. The contents start at the position 'offset' and stops
// by any character defined in 'stop_chars'.
// For example, 
//		0	INVITE
//		1	ACK
//		2	BYE
//
// Modification History:
// 02/01/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/cs_enum.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "parser/types.h"
#include "util_c/strutil.h"
#include "util_c/memory.h"

#define AOS_CSENUM_ENTRY_ALLO_SIZE 10

/*
// 
// Description
// It first retrieves the contents from the position specified. The contents
// stop when any of the characters in 'stop_chars' is encountered. 
// If the desired contents are received, the contents are compared with 
// the entries defined in this class. If the contents match any of the 
// stored ones, the corresponding index is returned. If the contents match
// no entries, -1 is returned.
//
static int aos_cs_enum_select_w_conts(
			struct aos_cont_selector *cs, 
			struct aos_omparser *parser, 
			char **contents, 
			int *len)
{
	int i;
	aos_assert_r(cs, -1);
	aos_assert_r(parser, -1);
	aos_assert_r(cs->type == eAosCSType_Enum, -1);

	aos_cs_enum_t *self = (aos_cs_enum_t *)cs;
	if (parser->mf->get_str_w_stops(parser, self->offset, self->offset_type, 
			self->min_len, self->stop_chars, contents, len))
	{
		aos_alarm("Expecting contents: %d:%d:%d but failed: %s", 
			self->offset, self->min_len, self->offset_type, 
			parser->mf->dump(parser));
		return -1;
	}

	aos_assert_r(*contents, -1);
	aos_assert_r(*len > 0, -1);
	for (i=0; i<self->noe; i++)
	{
		if (*len != self->entries[i].len) continue;

		switch (self->content_type)
		{
		case eAosContentType_Binary:
		case eAosContentType_CharStrCS:
			 if (memcmp(*contents, self->entries[i].contents, *len) == 0)
			 {
				return self->entries[i].index;
			 }
			 break;

		case eAosContentType_CharStrCI:
			 if (strncasecmp(*contents, self->entries[i].contents, *len) == 0)
			 {
				 return self->entries[i].index;
			 }
			 break;

		default:
			 aos_alarm("Unrecognized content type: %d", self->content_type);
			 return -1;
		}
	}

	// 
	// The contents do not match any stored.
	//
	return -1;
}
*/


static int aos_cs_enum_add_entry(
		aos_cs_enum_t *cs, 
		char * contents, 
		const int len, 
		const int index, 
		const int keep_flag)
{
	int ret, idx;
	aos_assert_r(cs, -1);
	aos_assert_r(contents, -1);
	aos_assert_r(len >= 0, -1);
	aos_assert_r(index > 0, -1);

	if (!cs->entries)
	{
		cs->entries = aos_malloc(sizeof(aos_csenum_entry_t) * 
				AOS_CSENUM_ENTRY_ALLO_SIZE);
		aos_assert_r(cs->entries, -1);
		memset(cs->entries, 0, sizeof(aos_csenum_entry_t) * 
				AOS_CSENUM_ENTRY_ALLO_SIZE);
		cs->entry_size = AOS_CSENUM_ENTRY_ALLO_SIZE;
	}
	else if (cs->noe >= cs->entry_size)
	{
		int new_size = cs->entry_size + AOS_CSENUM_ENTRY_ALLO_SIZE;
		void *block = aos_malloc(sizeof(aos_csenum_entry_t) * new_size);
		aos_assert_r(block, -1);
		memcpy(block, cs->entries, 
				sizeof(aos_csenum_entry_t) * cs->entry_size);
		aos_free(cs->entries);
		cs->entries = block;
		cs->entry_size = new_size;
	}

	// 
	// Create a new entry
	//
	idx = cs->noe;
	if (keep_flag)
	{
		cs->entries[idx].contents = contents;
	}
	else
	{
		ret = aos_str_set(&cs->entries[idx].contents, contents, len);
		aos_assert_r(!ret, -1);
	}
	cs->entries[idx].len = len;
	cs->entries[idx].index = index;
	cs->noe++;
	return 0;
}


static int aos_cs_enum_select(
			struct aos_cont_selector *cs, 
			struct aos_omparser *parser) 
{
	return aos_cs_enum_select(cs, parser);
}


static int aos_cs_enum_serialize(
		aos_cont_selector_t *cs, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		<CS_Enum>
	// 			<offset>
	// 			<offset_type>
	// 			<stop_chars>
	// 			<content_type>
	// 			<min_len>
	// 			<entries>
	// 				<entry>
	// 					<contents>
	// 					<index>
	// 				</entry>
	// 				...
	// 			</entries>
	// 		</CS_Enum>
	//
	int i;
	aos_xml_node_t *entry, *entries;
	aos_xml_node_t *crt = 0;
	aos_cs_enum_t *self = 0;
	aos_assert_r(cs, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(cs->type == eAosCSType_Enum, -1);
	self = (aos_cs_enum_t *)cs;
	
	crt = aos_xml_node_create1("CS_Enum");
	aos_assert_r(crt, -1);

	aos_assert_g(!crt->mf->append_child_int(crt, "offset", 
			self->offset), cleanup);
	aos_assert_g(!crt->mf->append_child_int(crt, "offset_type", 
			self->offset_type), cleanup);
	aos_assert_g(!crt->mf->append_child_hex(crt, "stop_chars", 
			self->stop_chars, self->stop_char_len), cleanup);
	aos_assert_g(!crt->mf->append_child_str(crt, "content_type", 
			aos_content_type_2str(self->content_type), 0, 0), cleanup);
	aos_assert_g(!crt->mf->append_child_int(crt, "min_len", 
			self->min_len), cleanup);

	entries = aos_xml_node_create1("entries");
	aos_assert_g(entries, cleanup);
	aos_assert_g(!crt->mf->add_child((aos_field_t *)crt, 
			(aos_field_t *)entries), cleanup);

	for (i=0; i<self->noe; i++)
	{
		entry = aos_xml_node_create1("entry");
		aos_assert_g(entry, cleanup);
		aos_assert_g(!entries->mf->add_child(
				(aos_field_t *)entries, (aos_field_t *)entry), cleanup);
		aos_assert_g(!entry->mf->append_child_bin(entry, "contents",
			self->entries[i].contents, self->entries[i].len), cleanup);
		aos_assert_g(!entry->mf->append_child_int(entry, "index",
			self->entries[i].index), cleanup);
	}

cleanup:
	if (crt) crt->mf->destroy((aos_field_t *)crt);
	return -1;
}


static int aos_cs_enum_deserialize(
		aos_cont_selector_t *cs, 
		aos_xml_node_t *node)
{
	// 
	// 	<CS_Enum>
	// 		<offset>
	// 		<offset_type>
	// 		<stop_chars>
	// 		<content_type>
	// 		<min_len>
	// 		<entries>
	// 			<entry>
	// 				<contents>
	// 				<index>
	// 			</entry>
	// 			...
	// 		</entries>
	// 	</CS_Enum>
	//
	char *cont_type = 0;
	char *contents;
	int len, index;

	aos_assert_r(cs, -1);
	aos_assert_r(node, -1);
	aos_assert_r(cs->type == eAosCSType_Enum, -1);
	aos_assert_r(node->mf->match_name((aos_field_t *)node, "CS_Enum"), -1);
	aos_cs_enum_t *self = (aos_cs_enum_t *)cs;
	
	aos_assert_r(!node->mf->first_child_int(node, "offset", 
			&self->offset), -1);
	aos_assert_r(!node->mf->first_child_char(node, "offset_type", 
			&self->offset_type), -1);
	aos_assert_r(!node->mf->first_child_hex(node, "stop_chars", 
			&self->stop_chars, &self->stop_char_len), -1);
	aos_assert_r(!node->mf->first_child_str(node, "content_type", 
			&cont_type, &len, 0), -1);
	self->content_type = aos_content_type_2enum(cont_type);
	aos_free(cont_type);
	aos_assert_r(!aos_content_type_check(self->content_type), -1);
	aos_assert_r(!node->mf->first_child_int(node, "min_len", 
			&self->min_len), -1);

	// Retrieve the entries
	aos_xml_node_t *entries = node->mf->first_named_child(node, "entries");
	aos_assert_r(entries, -1);
	aos_xml_node_t *entry = entries->mf->first_child(entries);
	while (entry)
	{
		aos_assert_r(!entry->mf->match_name((aos_field_t *)entry, "entry"), -1);
		aos_assert_r(!entry->mf->first_child_str(entry, "contents", 
			&contents, &len, 0), -1);
		aos_assert_r(!entry->mf->first_child_int(entry, "index", &index), -1);
		aos_assert_r(!self->mf->add_entry(self, contents, len, index, 1), -1);
		entry = entries->mf->next_sibling(entries);
	}
	return 0;
}



static aos_cs_enum_mf_t sg_mf = 
{
	aos_cs_select_w_conts,
	aos_cs_select,
	aos_cs_get_selected,
	aos_cs_enum_serialize, 
	aos_cs_enum_deserialize, 
	
	aos_cs_enum_add_entry
};


aos_cs_enum_t * aos_cs_enum_create(
		const int offset, 
		const aos_parser_offset_type_e offset_type, 
		const aos_content_type_e cont_type, 
		const char * const stop_chars, 
		const int stop_char_len, 
		const int min_len) 
{
	int ret;
	aos_assert_r(!aos_offset_type_check(offset_type), 0);
	aos_assert_r(stop_char_len >= 0, 0);
	aos_assert_r(!aos_content_type_check(cont_type), 0);

	aos_cs_enum_t *cs = aos_malloc(sizeof(aos_cs_enum_t));
	aos_assert_r(cs, 0);
	memset(cs, 0, sizeof(aos_cs_enum_t));

	aos_assert_g(aos_cont_selector_init((aos_cont_selector_t *)cs, 
	 		eAosCSType_Enum), cleanup);

	cs->offset = offset;
	cs->offset_type = offset_type;
	cs->content_type = cont_type;
	cs->stop_char_len = (stop_chars)?stop_char_len:aos_parser_default_ws_len;
	ret = aos_str_set(&cs->stop_chars, (stop_chars)?stop_chars:aos_parser_default_ws, 
			cs->stop_char_len);
	aos_assert_r(!ret, 0);
	cs->min_len = min_len;
	cs->mf = &sg_mf;
	return cs;

cleanup:
	aos_free(cs);
	return 0;
}

