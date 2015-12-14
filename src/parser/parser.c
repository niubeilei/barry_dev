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
// The parser buffers contents in parser->buffer. For simplicity, 
// the parser will read in all contents before doing any parsing. 
// This means that if a file is too big, it may blow up the application. 
// Future version may consider retrieving contents as needed.
//
// To protect programs, this parser limits the file to be no bigger
// than 300M (defined by AOS_OMPARSER_BUFF_MAX_SIZE).
//
// Modification History:
// 01/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/parser.h"

#include "alarm_c/alarm.h"
#include "parser/types.h"
#include "parser/scm_field.h"
#include "parser/msg.h"
#include "porting_c/file.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util2/value.h"
#include "util2/global_data.h"
#include "util_c/tracer.h"
#include "util_c/buffer.h"
#include "util_c/rc_obj.h"
#include <string.h>
#include <stdio.h>

AOS_DECLARE_RCOBJ_LOCK;

char aos_parser_default_ws[aos_parser_default_ws_len] = {' ', '\t'};
char aos_parser_ws1[aos_parser_ws1_len] = {' ', '\t', '\r', '\n'};

static int aos_omparser_hold(aos_omparser_t *parser)
{
	AOS_RC_OBJ_HOLD(parser);
}


static int aos_omparser_put(aos_omparser_t *parser)
{
	aos_omparser_t *obj = parser;
	aos_assert_r(obj, -1);                                      
	aos_assert_r(obj->is_destroying == 0, -1);                  
	aos_assert_r(sg_rcobj_lock, -1);                            
	aos_lock(sg_rcobj_lock);                                   
	obj->ref_count--;                                          
	if (obj->ref_count == 0)                                  
	{                                                        
		obj->is_destroying = 1;                             
		aos_unlock(sg_rcobj_lock);                         
		obj->mf->destroy(obj);                            
		return 0;                                        
	}                                                   
	aos_unlock(sg_rcobj_lock);                         
	return 0;

	// AOS_RC_OBJ_PUT(parser);
}


static int aos_omparser_release_memory(aos_omparser_t *parser)
{
	aos_assert_r(parser, -1);
	if (parser->schema) parser->schema->mf->put(parser->schema);
	parser->schema = 0;

	if (parser->buffer) aos_free(parser->buffer);
	parser->buffer = 0;
	memset(parser, 0, sizeof(*parser));
	return 0;
}


static int aos_omparser_destroy(aos_omparser_t *parser)
{
	aos_assert_r(!aos_omparser_release_memory(parser), -1);
	aos_free(parser);
	return 0;
}


aos_field_t * aos_omparser_parse_first_msg(
		aos_omparser_t *parser, 
		int *is_eof)
{
	aos_assert_r(parser, 0);
	parser->crt_pos = 0;
	return parser->mf->parse_next_msg(parser, is_eof);
}


aos_field_t * aos_omparser_parse_next_msg(
		aos_omparser_t *parser, 
		int *is_eof)
{
	aos_assert_r(parser, 0);
	aos_assert_r(parser->schema, 0);
	aos_assert_r(is_eof, 0);

	aos_scm_field_t *schema = parser->schema->mf->select_msg(
			parser->schema, parser, is_eof);
	if (!schema && *is_eof) return 0;

	aos_assert_r(schema, 0);
	schema->mf->hold(schema);

	aos_field_t *msg = schema->mf->parse_new(schema, parser, is_eof);
	if (!msg && is_eof) return 0;

	aos_assert_rm(msg, 0, "Failed to parse: %s", parser->mf->dump(parser));
	return msg;
}


int aos_omparser_set_schema(struct aos_omparser *parser, 
			aos_scm_field_t *schema)
{
	aos_assert_r(parser, -1);
	aos_assert_r(schema, -1);
	aos_rc_obj_assign(parser->schema, schema);
	return 0;
}


int aos_omparser_get_str_w_stops(
	struct aos_omparser *parser,
	const int offset, 
	const int offset_type,
	const int min_len, 
	const char * const stop_chars,
	char **contents,
	int *len)
{
	aos_not_implemented_yet;
	return -1;
}


char *aos_omparser_dump(struct aos_omparser *parser)
{
	int index = 0;
	int buff_len;
	char *buff = aos_get_global_str1(&buff_len);
	aos_assert_r(buff, "Error");

	aos_buff_append_line_int(buff, buff_len, 
			&index, "Data Length: ", parser->data_len);
	aos_buff_append_line_int(buff, buff_len, 
			&index, "Crt Pos: ", parser->crt_pos);
	aos_buff_append_line_int(buff, buff_len, 
			&index, "Parent: ", parser->parent_start);

	int contents_len = parser->data_len - parser->crt_pos;
	aos_buff_append_line_str_l(buff, buff_len, &index, "Contents:\n", 
			&parser->buffer[parser->crt_pos], contents_len);

	return buff;
}



// 
// Description
// 	Member function to trim the leading white spaces. By default, 
// 	' ' and '\t' are considered white spaces. This can be overridden
// 	by the parameter 'ws'. That is, if 'ws' is not null, it will 
// 	skip all the characters that are in 'ws' until the first character
// 	that is not in 'ws'.
//
// Return Values:
// 	The function always returns 0 unless errors.
//
static int aos_omparser_trim_ws(
			struct aos_omparser *parser, 
			const char * const ws, 
			const int ws_len)
{
	int crt_pos, data_len, ret;
	char *buffer;
	aos_assert_r(parser, -1);
	crt_pos = parser->crt_pos;
	data_len = parser->data_len;
	buffer  = parser->buffer;
	aos_assert_r(crt_pos <= data_len, -1);
	aos_assert_r(buffer, -1);

	while (crt_pos < data_len)
	{
		if (ws)
		{
			ret = aos_str_in(ws, ws_len, buffer[crt_pos]);
			aos_assert_r(ret >= 0, -1);
			if (ret == 0) break;
		}
		else
		{
			if (buffer[crt_pos] != ' ' && buffer[crt_pos] != '\t') break;
		}
		crt_pos++;
	}
			
	parser->crt_pos = crt_pos;
	return 0;
}


// 
// Description
// It skips the following: 
// 		space, '\t', '\r', and '\n'.
//
static int aos_omparser_trim_ws1(aos_omparser_t *parser)
{
	return parser->mf->trim_ws(parser, aos_parser_ws1, 
			aos_parser_ws1_len);
}

// 
// Description:
// It expects the value 'value' from the position 'start_pos' relative
// to the current position. It can be case sensitive or insensitive. 
// If the parser matches the expected, whether it moves the current 
// position right after the matched contents or not is controlled 
// by 'move'. If it is moved, the parser stops right after the matched
// string.
// 
// Parameters:
// parser:					the parser
// start_pos(IN):			the start position relative to current position
// value(IN):				the expected value
// value_len(IN):			the length of the expected value
// case_sensitivity(IN):	case sensitivity (1) or insensitive (0)
//
// Return Values:
// 1 if the parser matches the expected
// 0 if the parser does not match the expected
// -1 if errors.
//
static int aos_omparser_expect(
					struct aos_omparser *parser, 
					const int start_pos, 
					const char * const value, 
					const u32 value_len, 
					const char case_sensitive, 
					const u8 move)
{
	int crt_pos;
	aos_assert_r(parser, -1);
	aos_assert_r(value, -1);
	crt_pos = parser->crt_pos;

	// 
	// Check the starting position 
	//
	if (crt_pos + start_pos >= parser->data_len) return 0;

	// 
	// Check whether there are enough contents.
	//
	if (crt_pos + start_pos + value_len >= parser->data_len) return 0;

	if (case_sensitive)
	{
		if (memcmp(&parser->buffer[crt_pos + start_pos], value, value_len) == 0)
		{
			if (move)
			{
				parser->crt_pos += start_pos + value_len;
			}
			return 1;
		}
	}
	else
	{
		if (strncasecmp(&parser->buffer[crt_pos + start_pos], value, value_len) == 0)
		{
			if (move)
			{
				parser->crt_pos += start_pos + value_len;
			}
			return 1;
		}
	}

	return 0;
}
					

// 
// Description
// It retrieves the next word. The word starts from the current
// position and ends by any character defined in 'terminators'. 
// Upon return, the current position points to the first terminating
// character. The retrieved word is pointed to by 'word'. Its length
// is stored in 'word_len'. 
//
// Note that the function does not allocate memory. If the caller
// wants to keep the memory, it needs to allocate the memory.
//
// Parameters:
// parser:				the parser
// terminators(IN):		the terminating characters. If null, ' ' and '\t'
// 						are the terminators.
// term_len(IN):		the length of 'terminators'.
// word(OUT):			the word retrieved.
// word_len(OUT):		the length of the word retrieved.
//
// Returns
// 0 if success. 
// 1 if there is no more contents.
// 2 if the current is already a terminating character.
// -1 errors.
//
static int aos_omparser_next_word(
				struct aos_omparser *parser, 
				const char * const lws, 
				const int lws_len,
				const char * const terminators,
				const int term_len, 
				char **word, 
				int *word_len)
{
	int crt_pos, data_len, start;
	char *buff;
	int tlen = (terminators)?term_len:aos_parser_default_ws_len;

	aos_assert_r(parser, -1);
	aos_assert_r(word, -1);
	aos_assert_r(word_len, -1);
	aos_assert_r(terminators, -1);
	aos_assert_r(term_len > 0, -1);

	crt_pos = parser->crt_pos;
	data_len = parser->data_len;
	if (crt_pos >= data_len)
	{
		// 
		// There is no more contents.
		//
		return 1;
	}

	buff = parser->buffer;
	start = crt_pos;
	while (crt_pos < data_len)
	{
		if (aos_str_in(terminators, tlen, buff[crt_pos]))
		{
			// 
			// Hit the first terminating character. 
			//
			parser->crt_pos = crt_pos;
			break;
		}
		crt_pos++;
	}

	*word = &buff[start];
	*word_len = crt_pos - start;
	if (*word_len == 0) return 2;
	return 0;
}


// 
// Description
// It retrieves the next word, starting from the current position. 
// The word will not stop until it hits one of the characters 
// defined in 'terminators'. Before parsing the word, it will 
// skip all the characters defined in 'lws'. If the word is found, 
// the word is copied into the memory 'word', whose length is 
// 'word_len'. If the word is longer than 'word_len', it is an 
// error. 
//
// Returns:
// The function should always return 0 (unless wieard things 
// happened.
//
static int aos_omparser_next_word_b(
				struct aos_omparser *parser, 
				const char * const lws, 
				const int lws_len,
				const char * const terminators,
				const int term_len, 
				char *word, 
				int *word_len)
{
	int start, idx, len;

	if (lws)
	{
		aos_assert_r(!parser->mf->trim_ws(parser, lws, lws_len), -1);
	}

	start = parser->crt_pos;
	idx = start;
	while (idx < parser->data_len)
	{
		if (aos_str_in(terminators, term_len, parser->buffer[idx]) == 1)
		{
			// Found a terminating character. 
			break;
		}
		idx++;
	}

	if (idx == start)
	{
		*word_len = 0;
		return 0;
	}

	len = idx - start;

	aos_assert_r(len <= *word_len-1, -1);
	strncpy(word, &parser->buffer[start], len);
	word[len] = 0;
	*word_len = len;
	parser->crt_pos = idx;
	return 0;
}


// 
// Description
// It retrieves a string from the buffer. The starting of the contents
// is determined by offset. If offset_type == eAosParserOffset_Relative, 
// it is crt_pos + offset. If offset_type == eAosParserOffset_Abs, 
// it is the offset from the beginning of the buffer. The contents
// length is 'len'. If contents are found, memory is allocated and
// contents are copied to 'contents'. 
//
// Parameters:
// parser:			the parser
// offset(IN):		the offset
// offset_type		the offset type
// len(IN):			the length of the string
// contents(OUT):	the contents to be retrieved.
//
// Return Values
// 0 if success. 
// -1 if errors.
//
int aos_omparser_get_str(
		aos_omparser_t *parser, 
		const int offset, 
		const int offset_type,
		const int len,
		char **contents)
{
	int start_pos;
	aos_assert_r(parser, -1);
	aos_assert_r(!aos_offset_type_check(offset_type), -1);
	aos_assert_r(contents, -1);

	switch (offset_type)
	{
	case eAosOffsetType_Relative:
		 start_pos = parser->crt_pos + offset;
		 aos_assert_r(start_pos >= 0 && start_pos < parser->data_len, -1);
		 break;


	case eAosOffsetType_Absolute:
		 start_pos = 0;
		 break;

	default:
		 aos_alarm("Unrecognized offset_type: %d", offset_type);
		 return -1;
	}

	 aos_assert_r(start_pos + len <= parser->data_len, -1);
	 *contents = aos_malloc(len+1);
	 aos_assert_r(*contents, -1);
	 memcpy(*contents, &parser->buffer[start_pos], len);
	 return 0;
}


// 
// Description
// It retrieves a value based on the 'data_type'. If found, 
// an instance of aos_value_t is created. It assumes the 
// parser stops right at the closing bracket ">". 
//
// Returns
// 0 if success. 
// error codes otherwise. 
//
int aos_omparser_expect_value(
		struct aos_omparser *parser, 
		const aos_data_type_e data_type, 
		aos_value_t **value) 
{
	aos_assert_r(parser, -1);
	aos_assert_r(value, -1);
	aos_assert_r(parser->crt_pos >= 0, -1);

	char *str;
	int str_len;
	aos_assert_r(!parser->mf->next_word(parser, 
				" \t\r\n", 4, "<", 1, &str, &str_len), -1);
	
	*value = aos_value_factory(data_type, str, str_len);
	if (!value) return -1;
	return 0;
}


char aos_omparser_peek_next_char(struct aos_omparser *parser)
{
	aos_assert_r(parser, -1);
	aos_assert_r(parser->buffer, -1);
	aos_assert_r(parser->crt_pos + 1 < parser->data_len, -1);
	return parser->buffer[parser->crt_pos+1];
}


// 
// Description
// It searches the 'word' from the current position. If found, 
// it returns 1 and the parser stops right after the searched
// word. If 'terminators' are not empty, the word must be 
// stopped by one of the characters in 'terminators'. If the 
// word can be stopped by EOF, the corresponding terminating
// character is 0. If it is empty, the first substring that matches
// the 'word' is considered a match. 
//
// If not found, the cursor does not move.
//
// Returns
// 1 if found. 
// 0 if not found.
// negative if errors.
//
int aos_omparser_search_word(
		struct aos_omparser *parser, 
		const char * const word, 
		const int word_len, 
		const char * const terminators,
		const int term_len, 
		const char case_sensitive)
{
	int idx;
	char *substr;
	aos_assert_r(parser, -1);
	aos_assert_r(word, -1);
	aos_assert_r(word_len > 0, -1);

	idx = parser->crt_pos;
	while (idx + word_len < parser->data_len)
	{
		if (case_sensitive)
		{
			substr = strstr(&parser->buffer[idx], word);
		}
		else
		{
			substr = strcasestr(&parser->buffer[idx], word);
		}

		if (!substr)
		{
			// 
			// Did not find
			//
			return 0;
		}

		// 'idx' should be the index right after the searched word
		idx = substr - parser->buffer + word_len;
		aos_assert_r(idx <= parser->data_len, -1);

		if (idx == parser->data_len)
		{
			// This is the end of the buffer. Check whether
			// it can terminate the word
			if (!terminators || 
				aos_str_in(terminators, term_len, 0) == 1)
			{
				parser->crt_pos = idx;
				return 1;
			}

			// Not found
			return 0;
		}

		if (!terminators || aos_str_in(terminators, term_len, 
					parser->buffer[idx]) == 1)
		{
			parser->crt_pos = idx;
			return 1;
		}
	}

	return 0;
}


// 
// Description
// This function moves the parser to the closing tag. If successful, 
// the new position of the parser is right after the closing bracket
// of the closing tag. Note that the same tag name may be embedded
// inside a tag. 
//
// If the contents start with '<![CDATA[', it is CDATA content. 
// The contents are retrieved as:
// <tag><![CDATA[the_contents]]></tag>. 
// 'start' is set right after '<![CDATA[' and length is the length
// of 'the_contents'.
//
// The function assumes there shall be no white spaces between 
// "<", ">" and the tag name. 
//
// Returns
// 0 if success
// 1 if incomplete
// negative if failed
//
int aos_omparser_move_to_close_tag(
				struct aos_omparser *parser, 
				const char * const tag, 
				int *start, 
				int *len) 
{
	int tag_len, ret;
	int subtag_count = 0;
	aos_assert_r(parser, -1);
	aos_assert_r(tag, -1);

	tag_len = strlen(tag);
	aos_assert_r(tag_len > 0 && tag_len <AOS_XML_LABEL_MAX_LEN, -1);

	parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);

	// 
	// Check whether it is '<![CDATA[' tag. If yes, the closing tag should 
	// be right after ']]>'. The content's start should be right after 
	// '<![CDATA[' and end right before ']]>'. 
	//
	if (strncmp(&parser->buffer[parser->crt_pos], "<![CDATA[", 9) == 0)
	{
		// 
		// Found the '<![CDATA[' tag. Move it all the way to ']]>'. 
		//
		parser->crt_pos += 9;
		*start = parser->crt_pos;
		char *p = strstr(&parser->buffer[parser->crt_pos], "]]>");
		aos_assert_r(p, -1);
		*len = (int)p - (int)&parser->buffer[parser->crt_pos];
		parser->crt_pos += *len + 3;
	
		// 
		// Expecting the closing tag. 
		//
		parser->mf->trim_ws(parser, aos_parser_ws1, aos_parser_ws1_len);
		int idx = parser->crt_pos;
		int tag_len = strlen(tag);
		aos_assert_r(parser->buffer[idx] == '<' && parser->buffer[idx+1] == '/' &&
				strncmp(&parser->buffer[idx+2], tag, tag_len) == 0 &&
				parser->buffer[idx + 2 + tag_len] == '>', -1);
		parser->crt_pos += tag_len + 3;
		return 0;
	}

	*start = parser->crt_pos;
	ret = parser->mf->search_word(parser, tag, tag_len, ">", 1, 1);
	while (ret == 1)
	{
		// 
		// Found the substring that matches 'tag'. Need to check 
		// whether it is a beginning of a new tag or not. 
		//
		aos_assert_rm(parser->crt_pos - tag_len > 0, -1, "%d:%d:%s", 
				parser->crt_pos, tag_len, tag);
		char c = parser->buffer[parser->crt_pos-tag_len-1];
		if (c == '<')
		{
			// 
			// It is the openning of another subtag. 
			//
			subtag_count++;
		}
		else if (c == '/')
		{
			aos_assert_r(parser->crt_pos - tag_len - 1 > 0, -1);
			if (parser->buffer[parser->crt_pos-tag_len-2] == '<')
			{
				// 
				// It is a closing tag. 
				//
				subtag_count--;
				if (subtag_count == -1)
				{
					// 
					// It is the expected closing tag.
					//
					*len = parser->crt_pos - tag_len - 2 - *start;
					parser->crt_pos++;
					return 0;
				}
			}
		}

		ret = parser->mf->search_word(parser, tag, tag_len, ">", 1, 1);
	}

	if (parser->crt_pos == parser->data_len)
	{
		// Did not find the tag because it hits the eof. 
		return 1;
	}

	// 
	// Did not find the closing tag.
	//
	aos_alarm("Did not find the closing tag: tag=%s. Parser:%s", 
			tag, parser->mf->dump(parser));
	return -1;
}


static aos_omparser_mf_t sg_mf = 
{
	aos_omparser_hold,							
	aos_omparser_put,							
	aos_omparser_release_memory,							
	aos_omparser_destroy,							
	aos_omparser_trim_ws1,							
	aos_omparser_trim_ws,						
	aos_omparser_expect,					
	aos_omparser_expect_value,				
	aos_omparser_next_word,				
	aos_omparser_next_word_b,
	aos_omparser_set_schema,		
	aos_omparser_parse_first_msg,
	aos_omparser_parse_next_msg,
	aos_omparser_get_str,		
	aos_omparser_get_str_w_stops,
	aos_omparser_dump,				
	aos_omparser_peek_next_char,
	aos_omparser_move_to_close_tag,
	aos_omparser_search_word,

	aos_xml_parser_next_named_tag,								
	aos_xml_parser_next_tag,									
	aos_xml_parser_first_named_child,							
	aos_xml_parser_next_named_sibling,					
	aos_xml_parser_next_sibling,					
	aos_xml_parser_get_descendent,				
	aos_xml_parser_expect_open_tag,
	aos_xml_parser_parse_attr
	// AOS_OMPARSER_XML_MEMFUNC_INIT
};


// 
// Description
// This is the function that actually reads the file and constructs
// the parser. What it does is to open the file and read all the 
// contents into the parser's buffer. Note that to simplify the
// implementation, the parser will read all contents into buffer
// before parsing. This can be a problem if the file is too big.
// We leave this issue to future enhancements.
//
static int aos_omparser_read_file(
		struct aos_omparser *parser, 
		const char * const filename)
{
	aos_assert_r(filename, -1);
	aos_assert_r(strlen(filename)>0, -1);

	FILE *file = fopen(filename, "r");
	if (!file)
	{
		aos_alarm("Failed to read file: %s", filename);
		return -1;
	}

	int64_t size = aos_file_get_size(filename);
	if (size <= 0)
	{
		aos_alarm("Either file is empty or some errors: %s", 
				filename);
		return -1;
	}

	if (size > AOS_OMPARSER_BUFF_MAX_SIZE)
	{
		aos_alarm("File too big to parse. Maximum allowed: %d", 
				AOS_OMPARSER_BUFF_MAX_SIZE);
		return -1;
	}
	parser->data_len = size;

	parser->buffer = aos_malloc(size);
	if (!parser->buffer)
	{
		aos_alarm("Failed to allocate memory for the parser: %s", 
				filename);
		return -1;
	}

	size_t ss = fread(parser->buffer, 1, parser->data_len, file);
	aos_assert_r(ss == parser->data_len, -1); 

	return 0;
}


// 
// Description
// It creates a parser from a file. The filename cannot be null.
//
aos_omparser_t * aos_omparser_create_from_file(
		const char * const filename)
{
	aos_omparser_t *parser = aos_malloc(sizeof(aos_omparser_t));
	aos_assert_r(parser, 0);
	memset(parser, 0, sizeof(aos_omparser_t));

	parser->mf = &sg_mf;

	aos_assert_g(filename && strlen(filename) > 0, cleanup);
	aos_assert_g(!aos_omparser_read_file(parser, filename), cleanup);
	parser->mf->hold(parser);
	return parser;

cleanup:
	aos_free(parser);
	return 0;
}


// 
// Description
// It creates a parser from a string. 
//
aos_omparser_t * aos_omparser_create_from_str(
		const char * const str, 
		const int len)
{
	aos_assert_r(str, 0);
	aos_assert_r(len >= 0, 0);

	aos_omparser_t *parser = aos_malloc(sizeof(aos_omparser_t));
	aos_assert_r(parser, 0);
	memset(parser, 0, sizeof(aos_omparser_t));
	parser->mf = &sg_mf;

	parser->buffer = aos_malloc(len+1);
	aos_assert_g(parser->buffer, cleanup);
	memcpy(parser->buffer, str, len);
	parser->buffer[len] = 0;
	parser->data_len = len;
	parser->all_contents_read = 1;
	return parser;

cleanup:
	if (parser->buffer) aos_free(parser->buffer);
	aos_free(parser);
	return 0;
}


int aos_omparser_init(
		aos_omparser_t *parser, 
		char * contents, 
		const int len)
{
	aos_assert_r(parser, -1);
	aos_assert_r(len >= 0, -1);
	aos_assert_t(len == 0, !contents, -1);
	aos_assert_t(len > 0, contents, -1);

	memset(parser, 0, sizeof(aos_omparser_t));
	parser->mf = &sg_mf;

	if (len > 0)
	{
		parser->buffer = contents;
	}
	else
	{
		parser->buffer = aos_malloc(1);
		aos_assert_r(parser->buffer, -1);
		parser->buffer[0] = 0;
	}
	parser->data_len = len;

	return 0;
}
