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
// 03/06/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_conditions_const_h
#define aos_conditions_const_h

#include "conditions_c/cond.h"


typedef struct aos_const_mf
{
	AOS_COND_MEMFUNC_DECL;
} aos_cond_const_mf_t;


typedef struct aos_cond_const
{
	aos_cond_const_mf_t *mf;

	AOS_COND_MEMDATA_DECL;
	int						status;
} aos_cond_const_t;

aos_cond_t *aos_cond_const_create_xml(struct aos_xml_node *node);

#endif

