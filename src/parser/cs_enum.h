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
// 02/01/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_cs_enum_h
#define aos_parser_cs_enum_h

#include "parser/cs.h"
#include "parser/types.h"
#include "util_c/types.h"

struct aos_cs_enum;


typedef int (*aos_cs_enum_add_entry_t)(
		struct aos_cs_enum *cs, 
		char * contents, 
		const int len, 
		const int index, 
		const int keep_flag);

typedef struct aos_cs_enum_mf
{
	AOS_CONT_SELECTOR_MEMFUNC_DECL;
	aos_cs_enum_add_entry_t		add_entry;
} aos_cs_enum_mf_t;

#define AOS_CS_ENUM_MEMFUNC_INIT				\
	aos_cs_enum_serialize,						\
	aos_cs_enum_deserialize,					\
	aos_cs_enum_add_entry

typedef struct aos_csenum_entry
{
	char *	contents;
	int		len;
	int		index;
} aos_csenum_entry_t;

typedef struct aos_cs_enum
{
	// 
	// Member Functions
	//
	aos_cs_enum_mf_t	*mf;

	AOS_CONT_SELECTOR_MEMDATA_DECL;
	aos_csenum_entry_t * entries;		// content array
	int					entry_size;		// # of slots allocated for 'entries'
	int					noe;			// number of elements
	int					offset;
	char				offset_type;	// 0 absolute, 1: relative
	char *				stop_chars;		// characters to stop contents
	int					stop_char_len;
	aos_content_type_e	content_type;	// the content type
	int					min_len;		// -1: no minimum. 
} aos_cs_enum_t;


extern aos_cs_enum_t * aos_cs_enum_create(
		const int offset, 
		const aos_parser_offset_type_e offset_type, 
		const aos_content_type_e content_type,
		const char * const stop_chars, 
		const int stop_char_len, 
		const int min_len);

extern aos_cs_enum_t * aos_cs_enum_create_xml(
		struct aos_xml_node *conf);
#endif

