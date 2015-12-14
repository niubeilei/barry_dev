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
// 01/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rvg_rvg_h
#define aos_rvg_rvg_h

#include "rvg_c/types.h"

struct aos_xml_node;
struct aos_rvg;
struct aos_value;
struct aos_buffer;

typedef int (*aos_rvg_serialize_t)(
		struct aos_rvg *rvg, 
		struct aos_xml_node *parent);

typedef int (*aos_rvg_deserialize_t)(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node);

typedef int (*aos_rvg_hold_t)(
		struct aos_rvg *rvg); 

typedef int (*aos_rvg_put_t)(
		struct aos_rvg *rvg); 

typedef int (*aos_rvg_integrity_check_t)(
		struct aos_rvg *rvg); 

typedef int (*aos_rvg_next_value_t)(
		struct aos_rvg *rvg, 
		struct aos_value *value);

typedef int (*aos_rvg_domain_check_t)(
		struct aos_rvg *rvg, 
		struct aos_value *value);

typedef int (*aos_rvg_release_memory_t)(
		struct aos_rvg *rvg);

typedef int (*aos_rvg_destroy_t)(
		struct aos_rvg *rvg);

typedef char *(*aos_rvg_get_keywords_t)(
		struct aos_rvg *rvg);

typedef char *(*aos_rvg_get_description_t)(
		struct aos_rvg *rvg);

#define AOS_RVG_MEMFUNC_DECL							\
	aos_rvg_hold_t				hold;					\
	aos_rvg_put_t				put;					\
	aos_rvg_get_keywords_t		get_keywords;			\
	aos_rvg_get_description_t	get_description;		\
	aos_rvg_integrity_check_t	integrity_check;		\
	aos_rvg_next_value_t 		next_value;				\
	aos_rvg_domain_check_t 		domain_check;			\
	aos_rvg_serialize_t			serialize;				\
	aos_rvg_deserialize_t		deserialize;			\
	aos_rvg_release_memory_t	release_memory;			\
	aos_rvg_destroy_t			destroy

#define AOS_RVG_MEMFUNC_INIT							\
	aos_rvg_hold, 										\
	aos_rvg_put,										\
	aos_rvg_get_keywords,								\
	aos_rvg_get_description

#define AOS_RVG_MEMDATA_DECL				 			\
	aos_rvg_type_e 		type;							\
	char *				name;							\
	int					is_destroying;					\
	int					ref_count;						\
	char *				keywords;						\
	char *				description;


typedef struct aos_rvg_mf
{
	AOS_RVG_MEMFUNC_DECL;
} aos_rvg_mf_t;

typedef struct aos_rvg
{
	aos_rvg_mf_t *mf;

	AOS_RVG_MEMDATA_DECL;
} aos_rvg_t;


#ifdef __cplusplus
extern "C" {
#endif

extern aos_rvg_t *aos_rvg_factory_str(const char * const contents);
extern aos_rvg_t *aos_rvg_factory_xml(struct aos_xml_node *node);
extern int aos_rvg_hold(struct aos_rvg *rvg); 
extern int aos_rvg_put(struct aos_rvg *rvg); 
extern int aos_rvg_init(struct aos_rvg *rvg);
extern int aos_rvg_integrity_check(struct aos_rvg *rvg);

extern int aos_rvg_release_memory(
		struct aos_rvg *rvg);

extern struct aos_xml_node *aos_rvg_serialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *parent);

extern int aos_rvg_deserialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node);

extern char *aos_rvg_get_keywords(
		struct aos_rvg *rvg);

extern char *aos_rvg_get_description(
		struct aos_rvg *rvg);

#ifdef __cplusplus
}
#endif

#endif

