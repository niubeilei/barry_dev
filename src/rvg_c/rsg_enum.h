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
// 03/16/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_rsg_enum_h
#define aos_rvg_rsg_enum_h

#include "rvg_c/rsg.h"
#include "rvg_c/types.h"
#include "util_c/dyn_array.h"

struct aos_rsg;
struct aos_enumrsg;
struct aos_xml_node;


#define AOS_RSG_ENUM_MEMFUNC_DECL							\

#define AOS_RSG_ENUM_MEMDATA_DECL							\
	char						unique;						\
	char						ordered;					\
	struct aos_rig *			noe_selector;				\
	struct aos_rsg *			separator;					\
	struct aos_rsg *			lead_spacer;				\
	struct aos_rsg *			trail_spacer;				\
	struct aos_rig *			selector;					\
	aos_rvg_value_entry_t **	white_entries;				\
	aos_dyn_array_t				white_entries_array;		\
	aos_rvg_value_entry_t **	black_entries;				\
	aos_dyn_array_t				black_entries_array
	

typedef struct aos_rsg_enum_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RSG_MEMFUNC_DECL;
	AOS_RSG_ENUM_MEMFUNC_DECL;
} aos_rsg_enum_mf_t;

typedef struct aos_enumrsg
{
	aos_rsg_enum_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RSG_MEMDATA_DECL;
	AOS_RSG_ENUM_MEMDATA_DECL;
} aos_rsg_enum_t;

extern int aos_rsg_enum_init(aos_rsg_enum_t *);
extern aos_rsg_enum_t *aos_rsg_enum_create_xml(struct aos_xml_node *);
#endif
