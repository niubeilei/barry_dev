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
// 02/21/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_event_event_h
#define aos_event_event_h

#include "event_c/types.h"
#include "util_c/dyn_array.h"
#include "util_c/list.h"


#define AOS_EVENT_MAGIC 623612465

struct aos_event;
struct aos_cond;
struct aos_scm_msg;
struct aos_xml_node;
struct aos_event;
struct aos_state;

typedef int (*aos_event_hold_t)(
		struct aos_event *event);
 
typedef int (*aos_event_put_t)(
		struct aos_event *event);
 
typedef int (*aos_event_release_memory_t)(
		struct aos_event *event);
 
typedef int (*aos_event_serialize_t)(
		struct aos_event *event, 
		struct aos_xml_node *parent);

typedef int (*aos_event_deserialize_t)(
		struct aos_event *event, 
		struct aos_xml_node *node);

typedef int (*aos_event_destroy_t)(
		struct aos_event *event);

typedef const char * (*aos_event_dump_t)(
		struct aos_event *event);

typedef int (*aos_event_identity_check_t)(
		struct aos_event *event);

typedef int (*aos_event_dump_to_t)(
		struct aos_event *event, 
		char *buff, 
		int *idx);

#define AOS_EVENT_MEMFUNC_DECL						\
	aos_event_hold_t				hold;			\
	aos_event_put_t					put;			\
	aos_event_identity_check_t		identity_check;	\
	aos_event_release_memory_t		release_memory;	\
	aos_event_serialize_t			serialize;		\
	aos_event_deserialize_t			deserialize;	\
	aos_event_destroy_t				destroy;		\
	aos_event_dump_t				dump;			\
	aos_event_dump_to_t				dump_to


// !!!!!!!! IMPORTANT: 'type' and 'match_data'
// MUST be the first member data. 
#define AOS_EVENT_MEMDATA_DECL						\
	aos_event_e						type;			\
	int								match_data;		\
	aos_list_head_t					link;			\
	int								event_id;		\
	int								magic;			\
	int								ref_count;		\
	int								is_destroying	

typedef struct aos_event_mf
{
	AOS_EVENT_MEMFUNC_DECL;
} aos_event_mf_t;


typedef struct aos_event
{
	aos_event_mf_t *mf;

	AOS_EVENT_MEMDATA_DECL;
} aos_event_t;

extern int aos_event_init(aos_event_t *proc);
extern aos_event_t *aos_event_factory(struct aos_xml_node *node);

extern int aos_event_hold(
		struct aos_event *event);
 
extern int aos_event_put(
		struct aos_event *event);
 
extern int aos_event_release_memory(
		struct aos_event *event);
 
extern int aos_event_serialize(
		struct aos_event *event, 
		struct aos_xml_node *parent);

extern int aos_event_deserialize(
		struct aos_event *event, 
		struct aos_xml_node *node);

extern int aos_event_dump_to(
		struct aos_event *event, 
		char *buff, 
		int *idx);

extern int aos_event_identity_check(
		struct aos_event *event);

extern int aos_event_create(
		aos_event_t **event, 
		const aos_event_e type, 
		const int match_data);
#endif

