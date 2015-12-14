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
// 01/31/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_scm_msg_h
#define aos_parser_scm_msg_h

#include "parser/scm_field.h"

struct aos_scm_msg;
struct aos_omparser;
struct aos_scm_field;
struct aos_xml_node;
struct aos_msg;

// 
// Virtual member functions
//

#define AOS_SCM_MSG_MEMFUNC_DECL					\

#define AOS_SCM_MSG_MEMFUNC_INIT					\


#define AOS_SCM_MSG_MEMDATA_DECL					\

typedef struct 
{
	AOS_SCM_FIELD_MEMFUNC_DECL;
	AOS_SCM_MSG_MEMFUNC_DECL;
} aos_scm_msg_mf_t;

typedef struct aos_scm_msg
{
	aos_scm_msg_mf_t 	*mf;

	AOS_SCM_FIELD_MEMDATA_DECL;
	AOS_SCM_MSG_MEMDATA_DECL;

} aos_scm_msg_t;

extern int aos_scm_msg_init(aos_scm_msg_t *msg); 
extern int aos_scm_msg_destroy(aos_scm_field_t *msg);

#endif

