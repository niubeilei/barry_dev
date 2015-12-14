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
// 03/24/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_domain_domain_int_h
#define aos_omni_domain_domain_int_h

#include "domain/domain.h"

#define AOS_DOMAIN_INT_MEMFUNC_DECL						\

#define AOS_DOMAIN_INT_MEMDATA_DECL							\

typedef struct aos_domain_int_mf
{
	AOS_DOMAIN_MEMFUNC_DECL;
	AOS_DOMAIN_INT_MEMFUNC_DECL;
} aos_domain_int_mf_t;

typedef struct aos_domain_int
{
	aos_domain_nt_mf_t *mf;

	AOS_DOMAIN_MEMDATA_DECL;
	AOS_DOMAIN_INT_MEMDATA_DECL;
} aos_domain_int_t;

extern int aos_domain_int_init(aos_domain_int_t *sm);

extern aos_domain_int_t * aos_domain_int_create_xml(
		struct aos_xml_node *node);

#endif

