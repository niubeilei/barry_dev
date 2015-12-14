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
// 02/05/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_ms_enum_h
#define aos_parser_ms_enum_h

#include "parser/ms.h"

struct aos_msg;
struct aos_cont_selector;

typedef struct aos_ms_enum_mf
{
	AOS_MS_MEMFUNC_DECL;
} aos_ms_enum_mf_t;


typedef struct aos_ms_enum
{
	// 
	// Member Functions
	//
	aos_ms_enum_mf_t	*mf;

	AOS_MS_MEMDATA_DECL;

	struct aos_cont_selector *	cont_selector;
	struct aos_field **			msg;
	int	*			  			index;
	int							msg_size;
	int							noe;
} aos_ms_enum_t;

extern aos_ms_enum_t * aos_ms_enum_create_xml(struct aos_xml_node *conf);
#endif

