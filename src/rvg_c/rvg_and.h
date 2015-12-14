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
// 03/17/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_rvg_and_h
#define aos_rvg_rvg_and_h

#include "rvg_c/rvg.h"
#include "util_c/dyn_array.h"

struct aos_rsg;
struct aos_xml_node;

#define AOS_RVG_AND_MEMFUNC_DECL						\

#define AOS_RVG_AND_MEMDATA_DECL			 			\
	aos_dyn_array_t					rsgs_array;			\
	struct aos_rsg **				rsgs;				\
	aos_domain_check_e				domain_check_type


typedef struct aos_rvg_and_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RVG_AND_MEMFUNC_DECL;
} aos_rvg_and_mf_t;

typedef struct aos_rvg_and
{
	aos_rvg_and_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RVG_AND_MEMDATA_DECL;
} aos_rvg_and_t;


extern int aos_rvg_and_init(aos_rvg_and_t *rvg); 
extern aos_rvg_and_t *aos_rvg_and_create_xml(struct aos_xml_node *);
#endif

