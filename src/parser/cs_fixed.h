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
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_cs_fixed_h
#define aos_parser_cs_fixed_h

#include "parser/cs.h"
#include "parser/types.h"
#include "util_c/types.h"


typedef struct aos_cs_fixed_mf
{
	AOS_CONT_SELECTOR_MEMFUNC_DECL;
} aos_cs_fixed_mf_t;

typedef struct aos_cs_fixed
{
	// 
	// Member Functions
	//
	aos_cs_fixed_mf_t	*mf;

	AOS_CONT_SELECTOR_MEMDATA_DECL;

	int					offset;
	char				offset_type;	// 0 absolute, 1: relative
	int					len;
	char *				expected;
	int					expected_len;
	aos_content_type_e	cont_type;
} aos_cs_fixed_t;


extern aos_cs_fixed_t * aos_cs_fixed_create(
		const int offset, 
		const aos_parser_offset_type_e offset_type, 
		const int len);

extern aos_cs_fixed_t * aos_cs_fixed_create_xml(
		struct aos_xml_node *node);

#endif

