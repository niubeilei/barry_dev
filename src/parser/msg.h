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
#ifndef aos_parser_msg_h
#define aos_parser_msg_h

#include "parser/field.h"
#include "parser/types.h"
#include "util_c/list.h"

struct aos_msg;
struct aos_xml_node;
struct aos_field;
struct aos_omparser;
struct aos_scm_msg;

// 
// Member Function Definitions
//
// typedef const char * (*aos_msg_to_str_t)(
// 		struct aos_msg *msg);



#define AOS_MSG_MEMFUNC_DECL				\

#define AOS_MSG_MEMDATA_DECL				\

typedef struct aos_msg_mf
{
	AOS_FIELD_MEMFUNC_DECL;
	AOS_MSG_MEMFUNC_DECL;
} aos_msg_mf_t;

typedef struct aos_msg
{
	aos_msg_mf_t	*mf;

	AOS_FIELD_MEMDATA_DECL;
	AOS_MSG_MEMDATA_DECL;
} aos_msg_t;


extern int aos_msg_init(aos_msg_t *msg); 
extern int aos_msg_serialize(
		struct aos_msg *msg, 
		struct aos_xml_node *parent);
extern int aos_msg_deserialize(
		struct aos_msg *msg, 
		struct aos_xml_node *node);
extern int aos_msg_destroy(aos_msg_t *msg);
extern aos_msg_t *aos_msg_factory(struct aos_xml_node *conf);

#endif

