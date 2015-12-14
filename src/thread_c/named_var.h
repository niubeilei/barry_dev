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
// 03/05/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_thread_named_var_h
#define aos_thread_named_var_h

#include "util_c/list.h"
#include "util_c/gen_data_tags.h"
#include "util2/value.h"


struct aos_value;
struct aos_xml_node;
struct aos_named_var;


typedef int (*aos_named_var_serialize_t)(
		struct aos_named_var *var, 
		struct aos_xml_node *parent);

typedef int (*aos_named_var_deserialize_t)(
		struct aos_named_var *var, 
		struct aos_xml_node *parent);

typedef int (*aos_named_var_hold_t)(
		struct aos_named_var *var);

typedef int (*aos_named_var_put_t)(
		struct aos_named_var *var);

typedef int (*aos_named_var_release_memory_t)(
		struct aos_named_var *var);

typedef int (*aos_named_var_destroy_t)(
		struct aos_named_var *var);


#define AOS_NAMED_VAR_MAX_NAME_LEN 16

#define AOS_NAMED_VAR_MEMFUNC_DECL						\
	aos_named_var_hold_t				hold;			\
	aos_named_var_put_t					put;			\
	aos_named_var_serialize_t			serialize;		\
	aos_named_var_deserialize_t			deserialize;	\
	aos_named_var_release_memory_t		release_memory;	\
	aos_named_var_destroy_t				destroy


#define AOS_NAMED_VAR_MEMDATA_DECL						\
	aos_list_head_t			link;						\
	int						tag;						\
	int						magic;						\
	char 					name[AOS_NAMED_VAR_MAX_NAME_LEN+1];\
	struct aos_value 		value;						\
	int						ref_count;					\
	int						is_destroying

typedef struct aos_named_var_mf
{
	AOS_NAMED_VAR_MEMFUNC_DECL;
} aos_named_var_mf_t;

typedef struct aos_named_var
{
	aos_named_var_mf_t *mf;

	AOS_NAMED_VAR_MEMDATA_DECL;
} aos_named_var_t;

extern int aos_named_var_init_partial(
		aos_named_var_t *var, 
		const aos_data_type_e tag, 
		const char * const name);

extern aos_named_var_t *aos_named_var_create(
		const int tag, 
		const char * const name);

extern aos_named_var_t *aos_named_var_create_str(
		const aos_gen_data_tag_e tag, 
		const char * const name,
		char * value);

extern aos_named_var_t *aos_named_var_create_ptr(
		const aos_gen_data_tag_e tag, 
		const char * const name,
		void * ptr);

extern aos_named_var_t *aos_named_var_create_int(
		const aos_gen_data_tag_e tag, 
		const char * const name,
		const int value);

extern aos_named_var_t *aos_named_var_create_xml(struct aos_xml_node *node);

#endif // End of Include

