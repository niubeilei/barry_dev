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
#ifndef aos_parser_cs_h
#define aos_parser_cs_h

#include "alarm_c/alarm.h"
#include "parser/types.h"


struct aos_scm_msg;
struct aos_omparser;
struct aos_cont_selector;
struct aos_xml_node;

// 
// Virtual Member Function Declarations
//

// 
// This function return a positive integer based on the selection
// logic. For example, it may return 0 if the current contents
// are "INVITE", 1 if "ACK", and 2 if "BYE". The selected string
// is returned by 'contents' and length is returned by 'len'. 
// The caller needs to free the memory.
//
typedef int (*aos_cs_select_w_conts_t)(
			struct aos_cont_selector *cs, 
			struct aos_omparser *parser, 
			char **contents, 
			int *len);

typedef int (*aos_cs_select_t)(
			struct aos_cont_selector *cs, 
			struct aos_omparser *parser); 

typedef int (*aos_cs_serialize_t)(
		struct aos_cont_selector *cs, 
		struct aos_xml_node *parent);

typedef int (*aos_cs_deserialize_t)(
		struct aos_cont_selector *cs, 
		struct aos_xml_node *node);

typedef char * (*aos_cs_get_selected_t)(
			struct aos_cont_selector *cs,
			struct aos_omparser *parser,
			char **contents,
			int *len);


#define AOS_CONT_SELECTOR_MEMFUNC_DECL			\
	aos_cs_select_w_conts_t	select_w_conts;		\
	aos_cs_select_t			select;				\
	aos_cs_get_selected_t	get_selected;		\
	aos_cs_serialize_t		serialize;			\
	aos_cs_deserialize_t	deserialize

#define AOS_CONT_SELECTOR_MEMFUNC_INIT			\
	aos_cs_select_w_conts,						\
	aos_cs_select,								\
	aos_cs_get_selected

#define AOS_CONT_SELECTOR_MEMDATA_DECL			\
	aos_cs_type_e	type

typedef struct aos_cont_selector_mf
{
	AOS_CONT_SELECTOR_MEMFUNC_DECL;
} aos_cont_selector_mf_t;

typedef struct aos_cont_selector
{
	// 
	// Member Functions
	//
	aos_cont_selector_mf_t *mf;

	AOS_CONT_SELECTOR_MEMDATA_DECL;
} aos_cont_selector_t;


extern int aos_cs_select_w_conts(
			struct aos_cont_selector *cs, 
			struct aos_omparser *parser, 
			char **contents, 
			int *len);

extern int aos_cs_select(
			struct aos_cont_selector *cs, 
			struct aos_omparser *parser); 

extern int aos_cs_serialize(
		struct aos_cont_selector *cs, 
		struct aos_xml_node *parent);

extern int aos_cs_deserialize(
		struct aos_cont_selector *cs, 
		struct aos_xml_node *node);

extern char * aos_cs_get_selected(
			struct aos_cont_selector *cs,
			struct aos_omparser *parser,
			char **contents,
			int *len);

extern int aos_cont_selector_init(aos_cont_selector_t *cs, 
		aos_cs_type_e type);
extern struct aos_cont_selector *aos_cont_selector_factory(
		struct aos_xml_node *conf);
#endif

