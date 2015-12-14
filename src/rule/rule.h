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
#ifndef aos_rule_rule_h
#define aos_rule_rule_h

#include "util/dyn_array.h"
#include "util/types.h"


struct aos_rule;
struct aos_cond;
struct aos_scm_msg;
struct aos_xml_node;
struct aos_event;
struct aos_state;
struct aos_gen_data;

#define AOS_RULE_MAX_ACTIONS 1000

typedef int (*aos_rule_hold_t)(
		struct aos_rule *rule);
 
typedef int (*aos_rule_put_t)(
		struct aos_rule *rule);
 
typedef int (*aos_rule_serialize_t)(
		struct aos_rule *rule, 
		struct aos_xml_node *parent);

typedef int (*aos_rule_deserialize_t)(
		struct aos_rule *rule, 
		struct aos_xml_node *node);

typedef int (*aos_rule_destroy_t)(
		struct aos_rule *rule);

typedef const char * (*aos_rule_dump_t)(
		struct aos_rule *rule);

typedef int (*aos_rule_dump_to_t)(
		struct aos_rule *rule, 
		char *buff, 
		int *idx);

typedef int (*aos_rule_event_t)(
		struct aos_rule *rule, 
		struct aos_event *event, 
		struct aos_state *state);

#define AOS_RULE_MEMFUNC_DECL					\
	aos_rule_hold_t				hold;			\
	aos_rule_put_t				put;			\
	aos_rule_serialize_t		serialize;		\
	aos_rule_deserialize_t		deserialize;	\
	aos_rule_destroy_t			destroy;		\
	aos_rule_dump_t				dump;			\
	aos_rule_dump_to_t			dump_to;		\
	aos_rule_event_t			event


#define AOS_RULE_MEMDATA_DECL					\
	struct aos_cond *		cond;				\
	struct aos_action **	actions;			\
	struct aos_dyn_array	actions_array;		\
	int						magic;				\
	int						success_rc;			\
	int						result_int;			\
	struct aos_gen_data *	data;				\
	u32						reg_id

typedef struct aos_rule_mf
{
	AOS_RULE_MEMFUNC_DECL;
} aos_rule_mf_t;


typedef struct aos_rule
{
	aos_rule_mf_t *mf;

	AOS_RULE_MEMDATA_DECL;
} aos_rule_t;

extern int aos_rule_init(aos_rule_t *proc);
extern aos_rule_t *aos_rule_factory(struct aos_xml_node *node);

#endif

