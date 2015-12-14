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
#ifndef aos_rvg_rig_uniquerange_h
#define aos_rvg_rig_uniquerange_h

#include "rvg_c/rig.h"

struct aos_rig_uniquerange;
struct aos_value;
struct aos_xml_node;

typedef int (*aos_rig_uniquerange_serialize_t)(
		struct aos_rig_uniquerange *entry, 
		struct aos_xml_node *node);

typedef int (*aos_rig_uniquerange_deserialize_t)(
		struct aos_rig_uniquerange *entry, 
		struct aos_xml_node *node);

typedef int (*aos_rig_uniquerange_is_full_t)(
		struct aos_rig_uniquerange *entry);

typedef int (*aos_rig_uniquerange_integrity_check_t)(
		struct aos_rig_uniquerange *entry);

typedef int (*aos_rig_uniquerange_release_memory_t)(
		struct aos_rig_uniquerange *entry);

typedef int (*aos_rig_uniquerange_destroy_t)(
		struct aos_rig_uniquerange *entry);

typedef int (*aos_rig_uniquerange_next_int_t)(
		struct aos_rig_uniquerange *entry, 
		int *value);

typedef int (*aos_rig_uniquerange_reset_unique_t)(
		struct aos_rig_uniquerange *entry);

#define AOS_RIG_UNIQUERANGE_MEMFUNC_DECL					\
	aos_rig_uniquerange_serialize_t			serialize;		\
	aos_rig_uniquerange_deserialize_t		deserialize;	\
	aos_rig_uniquerange_integrity_check_t	integrity_check;\
	aos_rig_uniquerange_release_memory_t	release_memory;	\
	aos_rig_uniquerange_destroy_t			destroy;		\
	aos_rig_uniquerange_is_full_t			is_full;		\
	aos_rig_uniquerange_next_int_t			next_int;		\
	aos_rig_uniquerange_reset_unique_t		reset_unique

#define AOS_RIG_UNIQUERANGE_MEMDATA_DECL			\
	int *			elements;						\
	int				start;							\
	int				end;							\
	int				weight;							\
	int				unused_size;					\


typedef struct aos_rig_uniquerange_mf
{
	AOS_RIG_UNIQUERANGE_MEMFUNC_DECL;
} aos_rig_uniquerange_mf_t;

typedef struct aos_rig_uniquerange
{
	aos_rig_uniquerange_mf_t *mf;

	AOS_RIG_UNIQUERANGE_MEMDATA_DECL;
} aos_rig_uniquerange_t;

extern aos_rig_uniquerange_t *aos_rig_uniquerange_create(
		const int start, 
		const int end, 
		const int weight);
extern aos_rig_uniquerange_t *aos_rig_uniquerange_create_xml(struct aos_xml_node *);
#endif

