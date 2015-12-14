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
#ifndef aos_parser_ms_fixed_h
#define aos_parser_ms_fixed_h

#include "parser/ms.h"

struct aos_field;

typedef struct aos_ms_fixed_mf
{
	AOS_MS_MEMFUNC_DECL;
} aos_ms_fixed_mf_t;

typedef struct aos_ms_fixed
{
	// 
	// Member Functions
	//
	aos_ms_fixed_mf_t *	mf;

	AOS_MS_MEMDATA_DECL;
	struct aos_field *	msg;
} aos_ms_fixed_t;

extern aos_ms_fixed_t * aos_ms_fixed_create_xml(struct aos_xml_node *conf);
#endif

