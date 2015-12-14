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
#ifndef aos_rvg_rsg_h
#define aos_rvg_rsg_h

#include "rvg_c/rvg.h"
#include "util_c/dyn_array.h"

struct aos_xml_node;
struct aos_buffer;
struct aos_rsg;

typedef int (*aos_rsg_domain_check_len_t)(
			struct aos_rsg *rsg, 
			struct aos_value *value,
			int *idx, 
			const aos_domain_check_e type);

#define AOS_RSG_MEMFUNC_DECL								\
	aos_rsg_domain_check_len_t		domain_check_len

#define AOS_RSG_MEMDATA_DECL								\
	aos_domain_check_e				domain_check_type


typedef struct aos_rsg_xml_mf
{
	AOS_RVG_MEMFUNC_DECL;
	AOS_RSG_MEMFUNC_DECL;
} aos_rsg_mf_t;

typedef struct aos_rsg
{
	aos_rsg_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
	AOS_RSG_MEMDATA_DECL;
} aos_rsg_t;

extern aos_rsg_t *aos_rsg_create_xml(struct aos_xml_node *node);

extern struct aos_xml_node *aos_rsg_serialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *parent);

extern int aos_rsg_deserialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node);

extern int aos_rsg_serialize_valuelist(
		struct aos_xml_node *node,
		aos_rvg_value_entry_t **entries, 
		const int noe, 
		const char * const tagname);

extern int aos_rsg_deserialize_valuelist(
		struct aos_xml_node *node, 
		aos_rvg_value_entry_t ***entries,
		aos_dyn_array_t *array,
		const char * const tagname);
#endif

