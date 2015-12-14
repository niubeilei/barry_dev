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
// 02/14/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_rig_h
#define aos_rvg_rig_h

#include "rvg_c/rvg.h"
#include "util_c/dyn_array.h"

struct aos_rig;

typedef int (*aos_rig_reset_unique_t)(
		struct aos_rig *rig); 

typedef int (*aos_rig_next_int_t)(
		struct aos_rig *rig, 
		int *value);

typedef int (*aos_rig_add_range_t)(
		struct aos_rig *rig, 
		const int start, 
		const int end,
		const int weight);

#define AOS_RIG_MEMFUNC_DECL					\
		aos_rig_reset_unique_t	reset_unique;	\
		aos_rig_next_int_t		next_int;		\
		aos_rig_add_range_t		add_range

#define AOS_RIG_MEMDATA_DECL					\

typedef struct aos_rig_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RIG_MEMFUNC_DECL;
} aos_rig_mf_t;

typedef struct aos_rig
{
	aos_rig_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RIG_MEMDATA_DECL;
} aos_rig_t;


extern int aos_rig_serialize(
		aos_rvg_t *rig, 
		struct aos_xml_node *parent);

extern int aos_rig_deserialize(
		aos_rvg_t *rig, 
		struct aos_xml_node *node);

extern int aos_rig_integrity_check(aos_rvg_t *rig);
extern aos_rig_t * aos_rig_factory(struct aos_xml_node *node);
extern int aos_rig_reset_unique(struct aos_rig *rig); 

#endif

