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
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_ms_h
#define aos_parser_ms_h

#include "parser/types.h"

struct aos_xml_node;
struct aos_omparser;
struct aos_msg_selector;
struct aos_field;
struct aos_xml_node;

// 
// Virtual Member Function Declarations
//
typedef int (*aos_msg_selector_hold_t)(
			struct aos_msg_selector *ms);

typedef int (*aos_msg_selector_put_t)(
			struct aos_msg_selector *ms);

typedef struct aos_field *(*aos_msg_selector_select_msg_t)(
			struct aos_msg_selector *ms, 
			struct aos_omparser *parser);

typedef int (*aos_msg_selector_serialize_t)(
		struct aos_msg_selector *ms, 
		struct aos_xml_node *parent);

typedef int (*aos_msg_selector_deserialize_t)(
		struct aos_msg_selector *ms, 
		struct aos_xml_node *node);

typedef int (*aos_msg_selector_destroy_t)(
		struct aos_msg_selector *ms);

typedef int (*aos_msg_selector_integrity_check_t)(
		struct aos_msg_selector *ms);

typedef int (*aos_msg_selector_dump_to_t)(
		struct aos_msg_selector *ms, 
		char *buff, 
		int *len);

typedef const char * (*aos_msg_selector_dump_t)(
		struct aos_msg_selector *ms);


#define AOS_MS_MEMFUNC_DECL									\
	aos_msg_selector_hold_t				hold;				\
	aos_msg_selector_put_t				put;				\
	aos_msg_selector_select_msg_t		select_msg;			\
	aos_msg_selector_serialize_t		serialize;			\
	aos_msg_selector_deserialize_t		deserialize;		\
	aos_msg_selector_destroy_t			destroy;			\
	aos_msg_selector_integrity_check_t	integrity_check;	\
	aos_msg_selector_dump_to_t			dump_to;			\
	aos_msg_selector_dump_t				dump

#define AOS_MS_MEMFUNC_INIT								\
	aos_msg_selector_hold,								\
	aos_msg_selector_put,								\
	aos_msg_selector_select_msg

#define AOS_MS_MEMDATA_DECL								\
	aos_msg_selector_type_e	type;						\
	int						ref_count

typedef struct aos_ms_mf
{
	AOS_MS_MEMFUNC_DECL;
} aos_ms_mf_t;

typedef struct aos_msg_selector
{
	// 
	// Member Functions
	//
	aos_ms_mf_t *mf;

	AOS_MS_MEMDATA_DECL;
} aos_msg_selector_t;


extern int aos_msg_selector_hold(aos_msg_selector_t *msg);
extern int aos_msg_selector_put(aos_msg_selector_t *msg);


extern int aos_msg_selector_init(aos_msg_selector_t *ms, 
		const aos_msg_selector_type_e type);
extern aos_msg_selector_t * aos_msg_selector_factory(
		struct aos_xml_node *);
#endif

