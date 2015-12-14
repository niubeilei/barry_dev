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
#ifndef aos_omni_statemachine_statemachine_h
#define aos_omni_statemachine_statemachine_h

#include "event/types.h"
#include "porting/cond_var.h"
#include "porting/mutex.h"
#include "statemachine/types.h"
#include "util/types.h"
#include "util/value.h"
#include "util/dyn_array.h"
#include "util/named_var.h"
#include "util/list.h"

struct aos_xml_node;
struct aos_statemachine;
struct aos_state;
struct aos_field;
struct aos_event;
struct aos_gen_data;
struct aos_thread;

#define AOS_STATEMACHINE_MAGIC 23639736
#define AOS_STATEMACHINE_MAX_STATES 10000


typedef int (*aos_statemachine_serialize_t)(
		struct aos_statemachine *sm, 
		struct aos_xml_node *parent);

typedef int (*aos_statemachine_deserialize_t)(
		struct aos_statemachine *sm, 
		struct aos_xml_node *parent);

typedef int (*aos_statemachine_destroy_t)(
		struct aos_statemachine *sm);

typedef int (*aos_statemachine_hold_t)(
		struct aos_statemachine *sm);

typedef int (*aos_statemachine_put_t)(
		struct aos_statemachine *sm);

typedef int (*aos_statemachine_release_memory_t)(
		struct aos_statemachine *sm);

typedef int (*aos_statemachine_add_state_t)(
		struct aos_statemachine *sm, 
		struct aos_state *st);

typedef int (*aos_statemachine_state_finished_t)(
		struct aos_statemachine *sm, 
		struct aos_state *st);

typedef int (*aos_statemachine_msg_rcvd_t)(
		struct aos_statemachine *sm, 
		struct aos_field *msg);

typedef int (*aos_statemachine_event_rcvd_t)(
		struct aos_statemachine *sm, 
		struct aos_event *event);

typedef int (*aos_statemachine_start_t)(
		struct aos_statemachine *sm);

typedef int (*aos_statemachine_finish_t)(
		struct aos_statemachine *sm);

typedef int (*aos_statemachine_add_var_t)(
		struct aos_statemachine *sm, 
		const aos_gen_data_tag_e tag,
		char * value, 
		const int allo_mem);

typedef int (*aos_statemachine_del_var_t)(
		struct aos_statemachine *sm, 
		aos_gen_data_tag_e tag);

typedef int (*aos_statemachine_get_var_t)(
		struct aos_statemachine *sm, 
		const aos_gen_data_tag_e tag,
		struct aos_value **var);

typedef int (*aos_statemachine_get_var_str_t)(
		struct aos_statemachine *sm, 
		const aos_gen_data_tag_e tag,
		char **var);

typedef int (*aos_statemachine_var_exist_t)(
		struct aos_statemachine *sm, 
		const aos_gen_data_tag_e tag);

typedef int (*aos_statemachine_trans_state_t)(
		struct aos_statemachine *sm, 
		const int from, 
		const int to);

typedef int (*aos_statemachine_set_init_state_t)(
		struct aos_statemachine *sm, 
		struct aos_state *state);

typedef int (*aos_statemachine_add_event_t)(
		struct aos_statemachine *sm, 
		struct aos_event *event);

typedef int (*aos_statemachine_signal_t)(
		struct aos_statemachine *sm, 
		const aos_event_e event_id,
		struct aos_state *state);

typedef int (*aos_statemachine_proc_event_t)(
		struct aos_statemachine *sm, 
		struct aos_event *event);

typedef int (*aos_statemachine_run_state_t)(
		struct aos_statemachine *sm, 
		struct aos_state *state);

typedef struct aos_state *(*aos_statemachine_get_state_t)(
		struct aos_statemachine *sm, 
		const int state_id);

typedef int (*aos_statemachine_release_paired_obj_t)(
		struct aos_statemachine *sm);

#define AOS_STATEMACHINE_MEMFUNC_DECL				\
	aos_statemachine_hold_t			hold;			\
	aos_statemachine_put_t			put;			\
	aos_statemachine_release_paired_obj_t	release_paired_obj;\
	aos_statemachine_set_init_state_t	set_init_state;	\
	aos_statemachine_add_var_t		add_var;		\
	aos_statemachine_del_var_t		del_var;		\
	aos_statemachine_get_var_t		get_var;		\
	aos_statemachine_get_var_str_t	get_var_str;	\
	aos_statemachine_var_exist_t	var_exist;		\
	aos_statemachine_release_memory_t	release_memory;	\
	aos_statemachine_add_state_t	add_state;		\
	aos_statemachine_serialize_t	serialize;		\
	aos_statemachine_deserialize_t	deserialize;	\
	aos_statemachine_destroy_t		destroy;		\
	aos_statemachine_msg_rcvd_t		msg_rcvd;		\
	aos_statemachine_event_rcvd_t	event_rcvd;		\
	aos_statemachine_start_t		start;			\
	aos_statemachine_start_t		stop;			\
	aos_statemachine_finish_t		finish;			\
	aos_statemachine_trans_state_t	trans_state;	\
	aos_statemachine_add_event_t	add_event;		\
	aos_statemachine_signal_t		signal;			\
	aos_statemachine_proc_event_t	proc_event;		\
	aos_statemachine_state_finished_t	state_finished;\
	aos_statemachine_run_state_t	run_state;		\
	aos_statemachine_get_state_t	get_state


#define AOS_STATEMACHINE_MEMDATA_DECL				\
	char *							name;			\
	struct aos_state *				init_state;		\
	aos_list_head_t					states;			\
	int								is_destroying;	\
	int								ref_count;		\
	int								min_ref_count;	\
	struct aos_gen_data *			machine_data;	\
	int								magic;			\
	aos_list_head_t					init_actions;	\
	aos_list_head_t					exit_actions;	\
	struct aos_thread *				thread;			\
	aos_lock_t *					lock;			\
	aos_condvar_t *					condvar;		\
	aos_list_head_t					events;			\
	aos_event_e						event_id;		\
	struct aos_state *				crt_state;		\
	aos_sm_status_e					status;			\
	u32								reg_id


typedef struct aos_statemachine_mf
{
	AOS_STATEMACHINE_MEMFUNC_DECL;
} aos_statemachine_mf_t;

typedef struct aos_statemachine
{
	aos_statemachine_mf_t *mf;

	AOS_STATEMACHINE_MEMDATA_DECL;
} aos_statemachine_t;

extern int aos_statemachine_init(aos_statemachine_t *sm);
extern aos_statemachine_t * aos_statemachine_create_xml(
		struct aos_xml_node *node);

#endif

