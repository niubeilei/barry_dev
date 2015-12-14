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
#ifndef aos_omni_action_action_h
#define aos_omni_action_action_h

#include "action_c/types.h"
#include "event_c/event.h"
#include "util_c/list.h"
#include "util_c/types.h"

struct aos_xml_node;
struct aos_action;
struct aos_gen_data;

#define AOS_ACTION_MAGIC 23847724

typedef int (*aos_action_serialize_t)(
		struct aos_action *action, 
		struct aos_xml_node *parent);

typedef int (*aos_action_deserialize_t)(
		struct aos_action *action, 
		struct aos_xml_node *node);

typedef int (*aos_action_destroy_t)(
		struct aos_action *action);

typedef int (*aos_action_release_mem_t)(
		struct aos_action *action);

typedef int (*aos_action_hold_t)(
		struct aos_action *action);

typedef int (*aos_action_put_t)(
		struct aos_action *action);

typedef int (*aos_action_run_t)(
		struct aos_action *action, 
		struct aos_gen_data *data);

typedef int (*aos_action_is_finished_t)(
		struct aos_action *action);

typedef int (*aos_action_finish_action_t)(
		struct aos_action *action);

typedef int (*aos_action_need_reg_event_t)(
		struct aos_action *action);

typedef int (*aos_action_is_blocking_t)(
		struct aos_action *action);

typedef int (*aos_action_reset_t)(
		struct aos_action *action);

#define AOS_ACTION_MEMFUNC_DECL						\
	aos_action_hold_t			hold;				\
	aos_action_put_t			put;				\
	aos_action_serialize_t		serialize;			\
	aos_action_deserialize_t	deserialize;		\
	aos_action_destroy_t		destroy;			\
	aos_action_release_mem_t	release_memory;		\
	aos_action_run_t			run;				\
	aos_action_is_finished_t	is_finished;		\
	aos_action_finish_action_t	finish_action;		\
	aos_action_need_reg_event_t need_reg_event;		\
	aos_action_is_blocking_t	is_blocking;		\
	aos_action_reset_t			reset


#define AOS_ACTION_MEMDATA_DECL						\
	aos_action_type_e 			type;				\
	int							action_id;			\
	char *						name;				\
	int							magic;				\
	int 						is_destroying;		\
	int 						ref_count;			\
	aos_act_finish_md_e         finish_method;		\
	int							finished;			\
	int							blocking;			\
	aos_list_head_t				link

typedef struct aos_action_mf
{
	AOS_ACTION_MEMFUNC_DECL;
} aos_action_mf_t;

typedef struct aos_action
{
	aos_action_mf_t *mf;

	AOS_ACTION_MEMDATA_DECL;
} aos_action_t;

extern int aos_action_init(aos_action_t *sm);

extern aos_action_t * aos_action_factory(
		struct aos_xml_node *node);

extern int aos_action_run_actions(
		aos_action_t **actions, 
		const int noe, 
		struct aos_gen_data *data, 
		aos_event_callback_t callback, 
		void *user_data,
		u32 *reg_id);

extern int aos_action_reset_action_list(aos_list_head_t *list);

extern int aos_action_run_action_list(
		aos_list_head_t *list, 
		struct aos_gen_data *data, 
		aos_event_callback_t callback, 
		void *user_data,
		u32 *reg_id);

extern int aos_action_serialize(
		struct aos_action *action, 
		struct aos_xml_node *parent);

extern int aos_action_deserialize(
		struct aos_action *action, 
		struct aos_xml_node *node);

extern int aos_action_release_memory(
		struct aos_action *action);

extern int aos_action_hold(
		struct aos_action *action);

extern int aos_action_put(
		struct aos_action *action);

extern int aos_action_is_finished(
		struct aos_action *action);

extern int aos_action_finish_action(
		struct aos_action *action);

extern int aos_action_need_reg_event(
		struct aos_action *action);

extern int aos_action_is_blocking(
		struct aos_action *action);

extern int aos_action_reset(
		struct aos_action *action);

#endif

