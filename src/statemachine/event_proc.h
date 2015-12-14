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
#ifndef aos_statemachine_event_proc_h
#define aos_statemachine_event_proc_h

struct aos_cond;
struct aos_scm_msg;
struct aos_xml_node;
struct aos_event_proc;

typedef int (*aos_event_proc_serialize_t)(
		struct aos_event_proc *proc, 
		struct aos_xml_node *parent);

typedef int (*aos_event_proc_deserialize_t)(
		struct aos_event_proc *proc, 
		struct aos_xml_node *node);

typedef int (*aos_event_proc_destroy_t)(
		struct aos_event_proc *proc);

#define AOS_EVENT_PROC_MEMFUNC_DECL					\
	aos_event_proc_serialize_t		serialize;		\
	aos_event_proc_deserialize_t	deserialize;	\
	aos_event_proc_destroy_t		destroy


#define AOS_EVENT_PROC_MEMDATA_DECL				\
	struct aos_cond *		cond;				\
	struct aos_scm_msg *	msg_send;			\
	int 					next_state;

typedef struct aos_event_proc_mf
{
	AOS_EVENT_PROC_MEMFUNC_DECL;
} aos_event_proc_mf_t;


typedef struct aos_event_proc
{
	aos_event_proc_mf_t *mf;

	AOS_EVENT_PROC_MEMDATA_DECL;
} aos_event_proc_t;

extern int aos_event_proc_init(aos_event_proc_t *proc);
extern aos_event_proc_t *aos_event_proc_create_xml(struct aos_xml_node *node);

#endif

