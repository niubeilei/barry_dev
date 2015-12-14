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
// 03/06/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_action_set_value_h
#define aos_omni_action_set_value_h

#include "action_c/action.h"
#include "parser/types.h"
#include "porting_c/mutex.h"
#include "util_c/types.h"
#include "util_c/gen_data_tags.h"

struct aos_act_set_value;


#define AOS_ACT_SET_VALUE_MEMFUNC_DECL					\

#define AOS_ACT_SET_VALUE_MEMDATA_DECL					\
	aos_scope_e			lhs_scope;						\
	aos_obj_type_e		lhs_type;						\
	aos_gen_data_tag_e	lhs_tag;						\
	aos_scope_e			rhs_scope;						\
	aos_obj_type_e		rhs_type;						\
	aos_gen_data_tag_e	rhs_tag


typedef struct aos_act_set_value_mf
{
	AOS_ACTION_MEMFUNC_DECL;
	AOS_ACT_SET_VALUE_MEMFUNC_DECL;
} aos_act_set_value_mf_t;

typedef struct aos_act_set_value
{
	aos_act_set_value_mf_t *mf;

	AOS_ACTION_MEMDATA_DECL;
	AOS_ACT_SET_VALUE_MEMDATA_DECL;
} aos_act_set_value_t;

extern int aos_act_set_value_init(aos_act_set_value_t *sm);

extern aos_act_set_value_t *aos_act_set_value_create_xml(
		struct aos_xml_node *node);

#endif

