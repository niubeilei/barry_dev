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
#ifndef aos_omni_domain_domain_h
#define aos_omni_domain_domain_h

#include "domain/types.h"

struct aos_domain;
struct aos_value;
struct aos_xml_node;

typedef int (*aos_domain_integrity_check_t)(
		struct aos_domain *domain); 

typedef int (*aos_domain_serialize_t)(
		struct aos_domain *domain, 
		struct aos_xml_node *parent);

typedef int (*aos_domain_deserialize_t)(
		struct aos_domain *domain, 
		struct aos_xml_node *node);

typedef int (*aos_domain_destroy_t)(
		struct aos_domain *domain);

typedef int (*aos_domain_release_memory_t)(
		struct aos_domain *domain);

typedef int (*aos_domain_hold_t)(
		struct aos_domain *domain);

typedef int (*aos_domain_put_t)(
		struct aos_domain *domain);

typedef int (*aos_domain_check_t)(
		struct aos_domain *domain, 
		struct aos_value *data);


#define AOS_DOMAIN_MEMFUNC_DECL							\
	aos_domain_integrity_check_t	integrity_check;	\
	aos_domain_hold_t				hold;				\
	aos_domain_put_t				put;				\
	aos_domain_serialize_t			serialize;			\
	aos_domain_deserialize_t		deserialize;		\
	aos_domain_destroy_t			destroy;			\
	aos_domain_release_memory_t		release_memory;		\
	aos_domain_check_t				check


#define AOS_DOMAIN_MEMDATA_DECL							\
	aos_domain_e 				type;					\
	char *						name;					\
	int							magic;					\
	int 						is_destroying;			\
	int 						ref_count

typedef struct aos_domain_mf
{
	AOS_DOMAIN_MEMFUNC_DECL;
} aos_domain_mf_t;

typedef struct aos_domain
{
	aos_domain_mf_t *mf;

	AOS_DOMAIN_MEMDATA_DECL;
} aos_domain_t;

extern int aos_domain_init(aos_domain_t *sm);

extern aos_domain_t * aos_domain_factory(
		struct aos_xml_node *node);

extern int aos_domain_integrity_check(
		struct aos_domain *domain);

extern int aos_domain_serialize(
		struct aos_domain *domain, 
		struct aos_xml_node *parent);

extern int aos_domain_deserialize(
		struct aos_domain *domain, 
		struct aos_xml_node *node);

extern int aos_domain_release_memory(
		struct aos_domain *domain);

extern int aos_domain_hold(
		struct aos_domain *domain);

extern int aos_domain_put(
		struct aos_domain *domain);

#endif

