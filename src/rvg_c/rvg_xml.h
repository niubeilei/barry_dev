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
#ifndef aos_rvg_rvg_xml_h
#define aos_rvg_rvg_xml_h

#include "rvg_c/rvg.h"

struct aos_dyn_array;

#define AOS_RVG_XML_MEMFUNC_DECL					\

#define AOS_RVG_XML_MEMDATA_DECL					\
	struct aos_rsg *			tag_name_rsg;		\
	struct aos_rig *			integer_value_rig;	\
	struct aos_rsg *			str_value_rsg;		\
	struct aos_rsg *			addr_value_rsg;		\
	struct aos_rig *			atomic_type_rig;	\
	struct aos_rig *			num_child_rig;		\
	struct aos_rig *			atomic_child_rig;	\
	struct aos_rig *			total_node_rig


typedef struct aos_rvg_xml_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RVG_XML_MEMFUNC_DECL;
} aos_rvg_xml_mf_t;

typedef struct aos_rvg_xml
{
	aos_rvg_xml_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RVG_XML_MEMDATA_DECL;
} aos_rvg_xml_t;

extern aos_rvg_xml_t *aos_rvg_xml_create_xml(struct aos_xml_node *node);
#endif

