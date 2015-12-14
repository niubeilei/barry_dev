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
// 02/20/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_statemachine_sm_msg_h
#define aos_omni_statemachine_sm_msg_h

#include "statemachine/statemachine.h"


#define AOS_SM_MSG_MEMFUNC_DECL				\

#define AOS_SM_MSG_MEMDATA_DECL				\
	struct aos_msg *			msg

typedef struct aos_sm_msg_mf
{
	AOS_STATEMACHINE_MEMFUNC_DECL;
	AOS_SM_MSG_MEMFUNC_DECL
} aos_sm_msg_mf_t;

typedef struct aos_sm_msg
{
	aos_sm_msg_mf_t *mf;

	AOS_STATEMACHINE_MEMDATA_DECL;
	AOS_SM_MSG_MEMDATA_DECL
} aos_sm_msg_t;

extern int aos_sm_msg_init(aos_statemachine_t *sm);
extern aos_sm_msg_t *aos_sm_msg_create_xml(struct aos_xml_node *node);

#endif

