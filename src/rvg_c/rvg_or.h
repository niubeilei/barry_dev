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
// 03/18/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_rvg_or_h
#define aos_rvg_rvg_or_h

#include "rvg_c/rvg.h"
#include "util_c/dyn_array.h"

struct aos_rvg;
struct aos_rig;
struct aos_xml_node;

#define AOS_RVG_OR_MEMFUNC_DECL							\

#define AOS_RVG_OR_MEMDATA_DECL			 				\
	aos_dyn_array_t					rvgs_array;			\
	struct aos_rvg **				rvgs;				\
	struct aos_rig *				rvg_selector


typedef struct aos_rvg_or_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RVG_OR_MEMFUNC_DECL;
} aos_rvg_or_mf_t;

typedef struct aos_rvg_or
{
	aos_rvg_or_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RVG_OR_MEMDATA_DECL;
} aos_rvg_or_t;


extern int aos_rvg_or_init(aos_rvg_or_t *rvg); 
extern aos_rvg_or_t *aos_rvg_or_create_xml(struct aos_xml_node *);

#endif

