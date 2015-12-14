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
// 01/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_conditions_cond_h
#define aos_conditions_cond_h

#include "conditions_c/types.h"


struct aos_xml_node;
struct aos_cond;
struct aos_event;
struct aos_state;
struct aos_gen_data;

typedef int (*aos_cond_serialize_t)(
		struct aos_cond *cond, 
		struct aos_xml_node *parent);

typedef int (*aos_cond_deserialize_t)(
		struct aos_cond *cond, 
		struct aos_xml_node *node);

typedef int (*aos_cond_destroy_t)(
		struct aos_cond *cond);

typedef int (*aos_cond_hold_t)(
		struct aos_cond *cond); 

typedef int (*aos_cond_put_t)(
		struct aos_cond *cond); 

typedef int (*aos_cond_release_memory_t)(
		struct aos_cond *cond); 

typedef int (*aos_cond_evaluate_t)(
		struct aos_cond *cond, 
		struct aos_gen_data *data); 

typedef int (*aos_cond_evaluate_by_event_t)(
		struct aos_cond *cond, 
		struct aos_event *event, 
		struct aos_state *state, 
		struct aos_gen_data *data); 

#define AOS_COND_MEMFUNC_DECL							\
	aos_cond_hold_t					hold;				\
	aos_cond_put_t					put;				\
	aos_cond_serialize_t			serialize;			\
	aos_cond_deserialize_t			deserialize;		\
	aos_cond_destroy_t				destroy;			\
	aos_cond_release_memory_t		release_memory;		\
	aos_cond_evaluate_t				evaluate;			\
	aos_cond_evaluate_by_event_t	evaluate_by_event;	

#define AOS_COND_MEMDATA_DECL							\
	aos_cond_type_e					type;				\
	int								ref_count;			\
	int								is_destroying

typedef struct aos_cond_mf
{
	AOS_COND_MEMFUNC_DECL;
} aos_cond_mf_t;


typedef struct aos_cond
{
	aos_cond_mf_t *mf;

	AOS_COND_MEMDATA_DECL;
} aos_cond_t;

extern aos_cond_t *aos_cond_factory(struct aos_xml_node *node);

extern int aos_cond_serialize(
		struct aos_cond *cond, 
		struct aos_xml_node *parent);

extern int aos_cond_deserialize(
		struct aos_cond *cond, 
		struct aos_xml_node *node);

extern int aos_cond_destroy(struct aos_cond *cond);

extern int aos_cond_hold(struct aos_cond *cond); 

extern int aos_cond_put(struct aos_cond *cond); 

extern int aos_cond_release_memory(struct aos_cond *cond); 

extern int aos_cond_init(struct aos_cond *cond); 

#endif

