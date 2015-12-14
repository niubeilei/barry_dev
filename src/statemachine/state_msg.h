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
#ifndef aos_omni_statemachine_state_msg_h
#define aos_omni_statemachine_state_msg_h

#include "statemachine/state.h"

struct aos_msg;


#define AOS_STATE_MSG_MEMFUNC_DECL						\

#define AOS_STATE_MSG_MEMDATA_DECL						\
	struct aos_msg *		msg_to_send;


typedef struct aos_state_msg_mf
{
	AOS_STATE_MEMFUNC_DECL;
	AOS_STATE_MSG_MEMFUNC_DECL
} aos_state_msg_mf_t;

typedef struct aos_state_msg
{
	aos_state_msg_mf_t *mf;

	AOS_STATE_MEMDATA_DECL;
	AOS_STATE_MSG_MEMDATA_DECL
} aos_state_msg_t;

extern int aos_state_msg_init(aos_state_msg_t *sm);
extern aos_state_msg_t *aos_state_msg_create_xml(
		struct aos_xml_node *node);

#endif

