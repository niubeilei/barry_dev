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
#ifndef aos_omni_statemachine_state_h
#define aos_omni_statemachine_state_h

#include "statemachine/types.h"
#include "util/dyn_array.h"
#include "util/list.h"
#include "util/types.h"

#define AOS_STATE_MAX_ACTIONS 1000
#define AOS_STATE_MAX_RULES 1000
#define AOS_STATE_NULL_STATE_ID 0

struct aos_xml_node;
struct aos_state;
struct aos_statemachine;
struct aos_rule;
struct aos_event;
struct aos_gen_data;

typedef struct aos_state_branch
{
	aos_list_head_t 	link;
	struct aos_cond *	cond;
	int					state_id;
} aos_state_branch_t;

typedef int (*aos_state_serialize_t)(
		struct aos_state *state, 
		struct aos_xml_node *parent);

typedef int (*aos_state_deserialize_t)(
		struct aos_state *state, 
		struct aos_xml_node *node);

typedef int (*aos_state_destroy_t)(
		struct aos_state *state);

typedef int (*aos_state_release_mem_t)(
		struct aos_state *state);

typedef int (*aos_state_hold_t)(
		struct aos_state *state);

typedef int (*aos_state_put_t)(
		struct aos_state *state);

typedef int (*aos_state_set_data_t)(
		struct aos_state *state, 
		struct aos_gen_data *data);

typedef int (*aos_state_start_t)(
		struct aos_state *state, 
		struct aos_event *event, 
		struct aos_gen_data *data);

typedef int (*aos_state_finish_t)(
		struct aos_state *state);

typedef int (*aos_state_event_rcvd_t)(
		struct aos_state *state, 
		struct aos_event *event); 

typedef int (*aos_state_next_state_t)(
		struct aos_state *state, 
		int *state_id);

typedef int (*aos_state_clear_branches_t)(
		struct aos_state *state);

typedef int (*aos_state_is_finished_t)(
		struct aos_state *state);

typedef int (*aos_state_abort_state_t)(
		struct aos_state *state);


#define AOS_STATE_MEMFUNC_DECL						\
	aos_state_hold_t				hold;			\
	aos_state_put_t					put;			\
	aos_state_set_data_t			set_data;		\
	aos_state_clear_branches_t		clear_branches;	\
	aos_state_next_state_t			next_state;		\
	aos_state_serialize_t			serialize;		\
	aos_state_deserialize_t			deserialize;	\
	aos_state_destroy_t				destroy;		\
	aos_state_release_mem_t			release_memory;	\
	aos_state_start_t				start;			\
	aos_state_finish_t				finish;			\
	aos_state_event_rcvd_t			event_rcvd;		\
	aos_state_is_finished_t			is_finished;	\
	aos_state_abort_state_t			abort_state

#define AOS_STATE_MEMDATA_DECL						\
	aos_state_e					type;				\
	int							state_id;			\
	u32							life_timer;			\
	u32							life_timer_id;		\
	int							ignore_unknown_event;\
	int							magic;				\
	struct aos_statemachine *	statemachine;		\
	aos_list_head_t				actions;			\
	struct aos_gen_data *		data;				\
	struct aos_rule **			rules;				\
	struct aos_dyn_array		rules_array;		\
	aos_st_status_e				status;				\
	aos_list_head_t				branches;			\
	struct aos_gen_data *		branch_data;		\
	char						expect_events;		\
	int							is_destroying;		\
	int							ref_count;			\
	u32							reg_id;				\
	aos_list_head_t				sm_link


typedef struct aos_state_mf
{
	AOS_STATE_MEMFUNC_DECL;
} aos_state_mf_t;

typedef struct aos_state
{
	aos_state_mf_t *mf;

	AOS_STATE_MEMDATA_DECL;
} aos_state_t;

extern int aos_state_init(aos_state_t *sm);
extern aos_state_t *aos_state_factory(struct aos_xml_node *node);
extern int aos_state_is_atomic(const aos_state_e type);

extern int aos_state_serialize(
		struct aos_state *state, 
		struct aos_xml_node *parent);

extern int aos_state_deserialize(
		struct aos_state *state, 
		struct aos_xml_node *node);

extern int aos_state_release_mem(
		struct aos_state *state);

extern int aos_state_hold(
		struct aos_state *state);

extern int aos_state_put(
		struct aos_state *state);

extern int aos_state_set_data(
		struct aos_state *state, 
		struct aos_gen_data *data);

extern int aos_state_next_state(
		struct aos_state *state, 
		int *state_id);

extern int aos_state_clear_branches(
		struct aos_state *state);

extern int aos_state_finish(
		struct aos_state *state);

extern int aos_state_abort_state(
		struct aos_state *state);

extern int aos_state_is_finished(
		struct aos_state *state);

extern int aos_state_start(
		struct aos_state *state, 
		struct aos_event *event, 
		struct aos_gen_data *data);

#endif

