////////////////////////////////////////////////////////////////////////////
//
// Copyuniqueright (C) 2005
// Packet Engineering, Inc. All uniquerights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 03/16/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_rig_unique_h
#define aos_rvg_rig_unique_h

#include "rvg_c/rig.h"

struct aos_rig_unique;
struct aos_rig_uniquerange;

#define AOS_RIG_UNIQUE_MEMFUNC_DECL						\

#define AOS_RIG_UNIQUE_MEMDATA_DECL						\
	aos_dyn_array_t					ranges_array;		\
	struct aos_rig_uniquerange **	ranges;				\
	int 							total_weight;		\
	int 							crt_weight;			\
	int								first_full_range

typedef struct aos_rig_unique_xml_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RIG_MEMFUNC_DECL;
	AOS_RIG_UNIQUE_MEMFUNC_DECL;
} aos_rig_unique_mf_t;

typedef struct aos_rig_unique
{
	aos_rig_unique_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RIG_MEMDATA_DECL;
	AOS_RIG_UNIQUE_MEMDATA_DECL;
} aos_rig_unique_t;

extern aos_rig_unique_t *aos_rig_unique_create();
extern aos_rig_unique_t *aos_rig_unique_create_xml(struct aos_xml_node *node);
#endif

