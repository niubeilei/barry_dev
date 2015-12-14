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
#include "parser/xml.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/types.h"
#include "parser/attr.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/tracer.h"
#include <string.h>

// 
// Description:
// It assumes the current position points to the named tag. Otherwise, 
// it is an error. If so, it retrieves the start_index and the length.
// It moves the current position right after the end of the current
// tag. 
//
// Parameters:
// 	parser: 			the parser
// 	tag(IN):			the name of the expected tag
// 	start_index(OUT):	the start index of the contents of the current tag.
// 	len(OUT):			the length of the contents.
//
// Return Values:
// 0 if success. 1 if failed because the data are not complete
// (i.e., hit end of data). Otherwise, it returns -1.
//
int aos_xml_parser_next_named_tag(
			struct aos_omparser *parser, 
			const char * const tag, 
			int *start_index, 
			int *len, 
			aos_attr1_t ***attr, 
			int *num_attr)
{
	char local_tag[AOS_XML_LABEL_MAX_LEN+1];
	int tag_len = AOS_XML_LABEL_MAX_LEN;
	int ret;

	aos_assert_r(parser, -1);
	aos_assert_r(tag, -1);
	aos_assert_r(start_index, -1);
	aos_assert_r(len, -1);


	// 
	// Retrieve the tag
	//
	while (!(ret = parser->mf->next_tag(parser, local_tag, 
				tag_len, start_index, len, attr, num_attr))) 
	{
		if (strcmp(local_tag, tag) == 0)
		{
			// 
			// Found it
			//
			return 0;
		}
	}

	// 
	// Did not find it.
	//
	return ret;
}
		

// 
// Description:
// Attributes are in the form of:
// 	name="value" name="value" ... (can be single quotes). There may be spaces between 
// 	the equal sign. 
//
// This function will allocate the memory for all the attributes retrieved. The retrieved
// attributes are returned through 'attr'. This function assumes 'attr' holds no memory.
// The number of attributes retrieved is returned through 'num_attr'. 
//
// Returns:
// 0 if success
// 1 if incomplete
// -1 if errors
//
int aos_xml_parser_parse_attr(
		aos_omparser_t *parser, 
		aos_attr1_t ***attr, 
		int *num_attr) 
{
	aos_attr1_t *all_attrs[100];
	int total_attrs = 100;
	int attr_idx = 0;
	char *the_value = 0;
	int value_len = 0;
	char *the_name = 0;
	int name_len = 0;

	aos_assert_r(parser, -1);
	aos_assert_r(attr, -1);
	aos_assert_r(*attr == 0, -1);		// 'attr' holds nothing
	aos_assert_r(num_attr, -1);
	aos_assert_r(parser->crt_pos >= 0 && 
			parser->crt_pos < parser->data_len, -1);

	// Skip the white spaces
	int orig_pos = parser->crt_pos;
	parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);

	while (parser->buffer[parser->crt_pos] != '>' && 
		   parser->buffer[parser->crt_pos] != '/')
	{
		// Retrieve a name
		name_len = AOS_ATTR_NAME_MAX_LEN;
		aos_assert_g(!parser->mf->next_word(parser, 
				aos_parser_ws1, aos_parser_ws1_len, 
				" \t\r\n>=", 6, 
				&the_name, &name_len), restore);

		parser->mf->trim_ws(parser, aos_parser_ws1, 
				aos_parser_ws1_len);

		// Expecting '='
		aos_assert_gm(parser->mf->expect(parser, 0, "=", 1, 0, 1) == 1, 
			restore, "Expecting '=' but failed. \n%s", 
			parser->mf->dump(parser));
		
		parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);

		// Expecting either the single quote or double quote
		char quote = parser->buffer[parser->crt_pos++];
		aos_assert_g(quote == '\"' || quote == '\'', restore);

		// Retrieve value
		if (quote == '\"')
		{
			aos_assert_g(parser->mf->next_word(parser, aos_parser_ws1, 
				aos_parser_ws1_len, "\"", 1, 
				&the_value, &value_len) >= 0, restore);
		}
		else
		{
			aos_assert_g(!parser->mf->next_word(parser, aos_parser_ws1, 
				aos_parser_ws1_len, "\'", 1, 
				&the_value, &value_len) >= 0, restore);
		}
		parser->crt_pos++;

		// New an attribute
		aos_attr1_t *one_attr = aos_malloc(sizeof(aos_attr1_t));
		aos_assert_r(one_attr, -1);
		strncpy(one_attr->name, the_name, name_len);
		one_attr->value = aos_malloc(value_len+1);
		aos_assert_r(one_attr->value, -1);
		strncpy(one_attr->value, the_value, value_len);
		one_attr->name[name_len] = 0;
		one_attr->value[value_len] = 0;

		all_attrs[attr_idx++] = one_attr;
		aos_assert_gm(attr_idx < total_attrs, restore, "idx = %d, total: %d", attr_idx, total_attrs);

		if (parser->buffer[parser->crt_pos] == ',') parser->crt_pos++;
		parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);
	}

	if (attr_idx > 0)
	{
		*attr = aos_malloc(attr_idx * 4);
		aos_assert_r(*attr, -1);
		memcpy(*attr, all_attrs, attr_idx * 4);
	}
	*num_attr = attr_idx;
	return 0;

restore:
	if (parser->crt_pos == parser->data_len)
	{
		// The data is incomplete
		return 1;
	}

	parser->crt_pos = orig_pos;
	return -1;
}


// 
// Description:
// It assumes there is a tag starting from the current position. 
// It retrieves this tag. If successful, it moves the current position
// right after the end of the retrieved tag. If anything goes wrong, 
// it does not move the current position at all.
//
// Parameters:
// parser:			the parser
// tag(OUT):		the retrieved tag
// tag_len(OUT):	the length of the retrieved tag
// start_index(OUT): the start index of the contents of the retrieved tag
// len(OUT):		the length of the contents of the retrieved tag
//
// Return Values:
// 0 if successful. 
// 1 if the data is imcomplete
// 2 if no more tags
// -1 if failed. 
//
int aos_xml_parser_next_tag(
			struct aos_omparser *parser, 
			char *tag, 
			const int tag_len,
			int *start_index, 
			int *len, 
			aos_attr1_t ***attr, 
			int *num_attr) 
{
	int start, orig_pos, ret;
	int tl = tag_len;
	aos_assert_r(parser, -1);
	aos_assert_r(tag, -1);
	aos_assert_r(tag_len > 0, -1);
	aos_assert_r(start_index, -1);
	aos_assert_r(len, -1);

	*len = 0;
	orig_pos = parser->crt_pos;
	parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);

	// Expecting '<'
	ret = parser->mf->expect(parser, 0, "<", 1, 0, 1); 
	if (ret == 0)
	{
		// No more tags. If it is eof, return 1. 
		// Otherwise, return 2.
		return (parser->crt_pos == parser->data_len)?1:2;
	}

	aos_assert_r(ret == 1, ret);

	start = parser->crt_pos;

	// 
	// If the next character is '/', it means there is no next 
	// tag. return 1.
	//
	if (parser->buffer[parser->crt_pos] == '/')
	{
		// No more tags
		return 2;
	}

	// Retrieve the tag name
	aos_assert_g(!parser->mf->next_word_b(parser, "", 
				0, " \t\r\n>/", 6, tag, &tl), restore );

	if (parser->crt_pos == parser->data_len)
	{
		// Imcomplete
		return 1;
	}

	// Retrieve the attributes
	aos_assert_g(!parser->mf->parse_attr(parser, attr, 
			num_attr), restore);
	parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);

	if (parser->crt_pos == parser->data_len)
	{
		return 1;
	}

	// If it is '/>', it is the end of this tag. 
	if (parser->buffer[parser->crt_pos] == '/' &&
		parser->buffer[parser->crt_pos+1] == '>')
	{
		*len = 0;
		parser->crt_pos += 2;
		*start_index = parser->crt_pos;
		parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);
		return 0;
	}

	// Expecting ">"
	aos_assert_gm(parser->mf->expect(parser, 0, ">", 1, 0, 1) == 1, 
			restore, "Expecting '>' but failed. Tag = %s. Contents: %s. Current: %s", 
			tag, parser->buffer,
			&parser->buffer[parser->crt_pos]);
	*start_index = parser->crt_pos;

	// 
	// Now an opening tag has been retrieved. The next step is to 
	// retrieve the contents of the current tag, which is stopped
	// by the pattern "</tag_name>". 
	//
	aos_assert_gm(!parser->mf->move_to_close_tag(parser, tag, 
		start_index, len), restore, 
		"Expecting the close tag: but failed: %s", 
		parser->mf->dump(parser));

	parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);
	return 0;

restore:
	if (parser->crt_pos == parser->data_len)
	{
		return 1;
	}

	parser->crt_pos = orig_pos;
	return -1;
}
		

// 
// Description:
// It retrieves the current tag's first named child. If successful, 
// the current position points right after the found child. The 
// child must be the parent's immediate child. Otherwise, one should
// call aos_xml_parser_get_descendent(...).
//
// Assumption:
// The parser must have a valid 'parent_tag'. Otherwise, it is an error.
// The parent tag is set by calling parser->set_parent_tag(...). 
//
// Parameters:
// parser:			the parser
// tag(IN):			the name of the child
// start_index(OUT): the start index of contents of the retrieved child
// len(OUT):        the length of contents of the retrieved child
//
// Return Values:
// 0 if the named child is found.
// 1 if incomplete.
// 2 if not found.
// -1 if failed.
//
int aos_xml_parser_first_named_child(
			struct aos_omparser *parser, 
			const char * const tag, 
			int *start_index, 
			int *len, 
			aos_attr1_t ***attr, 
			int *num_attr)
{
	char ltag[50];
	int tt_len = 50;
	int saved_crt_pos;
	int ret;
	aos_assert_r(parser, -1);
	aos_assert_r(tag, -1);
	aos_assert_r(start_index, -1);
	aos_assert_r(len, -1);
	aos_assert_r(parser->parent_start >= 0 && 
				 parser->parent_start < parser->data_len, -1);

	parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);

	saved_crt_pos = parser->crt_pos;
	parser->crt_pos = parser->parent_start;
	while (!(ret = parser->mf->next_tag(parser, ltag, tt_len, 
				start_index, len, attr, num_attr)))
	{
		if (strcmp(ltag, tag) == 0)
		{
			// 
			// Found the tag. 
			//
			return 0;
		}
	}

	// 
	// Did not find it. 
	//
	if (ret == 1) return 1;

	parser->crt_pos = saved_crt_pos;
	return ret;
}
		

/*
// 
// Description:
// It retrieves the current tag's first child. If successful, 
// the current position points right after the found child. The 
// child must be the parent's immediate child. 
//
// Assumption:
// The parser must have a valid 'parent_tag'. Otherwise, it is an error.
// The parent tag is set by calling parser->set_parent_tag(...). 
//
// Parameters:
// parser:			the parser
// tag(OUT):		the name of the child
// tag_len(OUT):	the length of the tag.
// start_index(OUT): the start index of contents of the retrieved child
// len(OUT):        the length of contents of the retrieved child
//
// Return Values:
// 0 if the named child is found.
// 1 if not found.
// -1 if failed.
//
int aos_xml_parser_first_child(
			struct aos_omparser *parser, 
			char *tag, 
			const int tag_len,
			int *start_index, 
			int *len)
{
	int saved_crt_pos;
	int ret;
	aos_assert_r(parser, -1);
	aos_assert_r(tag, -1);
	aos_assert_r(start_index, -1);
	aos_assert_r(len, -1);
	aos_assert_r(parser->parent_start >= 0 && 
				 parser->parent_start < parser->data_len, -1);

	parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);
	saved_crt_pos = parser->crt_pos;
	if (!(ret = parser->mf->next_tag(parser, tag, tag_len, 
			start_index, len)))
	{
		// 
		// Found the first child. 
		//
		return 0;
	}

	// 
	// Did not find it.
	//
	if (ret == 1) return 1;

	parser->crt_pos = saved_crt_pos;
	return ret;
}
*/
		

// 
// Description:
// It starts from current position, search for the first sibling whose
// name matches 'tag'. If found, the start_index and len are returned.
// The parser's current position is right after the found tag. If not
// found, nothing changed to parser. 
//
// Parameters:
// parser:			the parser.
// tag(IN):			the name of the sibling to look for.
// start_index(OUT): the start index of contents of the first sibling.
// len(OUT):		the length of contents of the first sibling.
//
// Return Values:
// 0 if found. 
// 1 if not found.
// -1 if errors.
//
int aos_xml_parser_next_named_sibling(
			struct aos_omparser *parser, 
			const char * const tag, 
			int *start_index, 
			int *len, 
			aos_attr1_t ***attr, 
			int *num_attr)
{
	int saved_crt_pos;
	aos_assert_r(parser, -1);
	aos_assert_r(tag, -1);
	aos_assert_r(start_index, -1);
	aos_assert_r(len, -1);

	saved_crt_pos = parser->crt_pos;
	if (!parser->mf->next_named_tag(parser, tag, start_index, 
			len, attr, num_attr))
	{
		// 
		// Found the tag. 
		//
		return 0;
	}

	// 
	// Did not find it. 
	//
	parser->crt_pos = saved_crt_pos;
	return 1;
}
		

int aos_xml_parser_next_sibling(
			struct aos_omparser *parser, 
			char *tag, 
			const int tag_len,
			int *start_index, 
			int *len, 
			aos_attr1_t ***attr, 
			int *num_attr)
{
	int saved_crt_pos;
	int ret;
	aos_assert_r(parser, -1);
	aos_assert_r(tag, -1);
	aos_assert_r(start_index, -1);
	aos_assert_r(len, -1);

	saved_crt_pos = parser->crt_pos;
	if (!(ret = parser->mf->next_tag(parser, tag, tag_len, 
			start_index, len, attr, num_attr)))
	{
		// 
		// Found the tag. 
		//
		return 0;
	}

	// 
	// Did not find it. 
	//
	if (ret == 1) return 1;

	parser->crt_pos = saved_crt_pos;
	return ret;
}
		

int aos_xml_parser_get_descendent(
			struct aos_omparser *parser, 
			const char * const tag, 
			int *start_index, 
			int *len)
{
	aos_not_implemented_yet;
	return -1;
}


// 
// Description
// The parser should be at the beginning of an open tag
// whose name is "label". If it is, the parser stops right
// after the closing bracket ">". 
//
// If "*label != 0", the tag must match
// the label. If "label == 0", it means it
// matches any tag name and the tag name is returned through 
// 'label'. Note that in this case, this function will 
// allocate memory for 'label'.
//
// Returns
// 0 if the parser is at the beginning of the open tag. 
// 1 if incomplete. 
// negative if not or errors.
//
int aos_xml_parser_expect_open_tag(
		aos_omparser_t *parser, 
		char ** label, 
		aos_attr1_t ***attr, 
		int *num_attr) 
{
	int alloc_mem = 0;
	int location = 0;
	aos_assert_r(parser, -1);
	aos_assert_r(label, -1);

	aos_assert_r(!parser->mf->trim_ws1(parser), -1);

	// Expecting '<'
	if (parser->mf->expect(parser, 0, "<", 1, 1, 1) != 1)
	{
		location = 1;
		goto check_eof;
	} 

	if (*label)
	{
		// Expecting 'label'
		if (parser->mf->expect(parser, 0, *label, 
					strlen(*label), 1, 1) != 1)
		{
			location = 2;
			goto check_eof;
		}
	}
	else
	{
		int len;
		aos_assert_g(!parser->mf->next_word(parser,
			aos_parser_ws1, aos_parser_ws1_len,
			" \t\r\n>=", 6,
			label, &len), check_eof);
		alloc_mem = 1;
	}

	// Retrieve the attributes
	aos_assert_g(!parser->mf->parse_attr(
			parser, attr, num_attr), check_eof);

	// Expecting '>'
	if (parser->mf->expect(parser, 0, ">", 1, 1, 1) != 1)
	{
		location = 3;
		goto check_eof;
	}

	return 0;

check_eof:
	if (alloc_mem)
	{	
		aos_free(*label);
		*label = 0;
	}

	if (parser->crt_pos == parser->data_len)
	{
		return 1;
	}

	aos_alarm("Failed: %d", location);
	return -1;
}


