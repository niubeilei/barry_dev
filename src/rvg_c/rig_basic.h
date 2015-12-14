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
#ifndef aos_rvg_rig_basic_h
#define aos_rvg_rig_basic_h

#include "rvg_c/rig.h"
#include "util_c/dyn_array.h"

struct aos_rig_basic;

typedef struct aos_rig_basic_range
{
	int 	start;
	int		end;
	int		weight;
} aos_rig_basic_range_t;

#define AOS_RIG_BASIC_MAX_WEIGHT 1000
#define AOS_RIG_BASIC_MAX_RANGES 1000

typedef int (*aos_rig_basic_reset_unique_t)(
		struct aos_rig_basic *rig); 

typedef int (*aos_rig_basic_next_int_t)(
		struct aos_rig_basic *rig, 
		int *value);

typedef int (*aos_rig_basic_add_range_t)(
		struct aos_rig_basic *rig, 
		const int start, 
		const int end,
		const int weight);

#define AOS_RIG_BASIC_MEMFUNC_DECL						\

#define AOS_RIG_BASIC_MEMDATA_DECL						\
	struct aos_dyn_array 		ranges_array;			\
	aos_rig_basic_range_t **	ranges;					\
	struct aos_dyn_array		index_array;			\
	int *						index

typedef struct aos_rig_basic_xml_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RIG_MEMFUNC_DECL;
	AOS_RIG_BASIC_MEMFUNC_DECL;
} aos_rig_basic_mf_t;

typedef struct aos_rig_basic
{
	aos_rig_basic_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RIG_MEMDATA_DECL;
	AOS_RIG_BASIC_MEMDATA_DECL;
} aos_rig_basic_t;

extern aos_rig_basic_t *aos_rig_basic_create();
extern aos_rig_basic_t *aos_rig_basic_create_xml(struct aos_xml_node *);

#endif

