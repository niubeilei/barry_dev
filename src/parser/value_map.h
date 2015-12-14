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
#ifndef aos_omni_parser_value_map_h
#define aos_omni_parser_value_map_h

struct aos_xml_node;
struct aos_value_map;

typedef int (*aos_value_map_get_index_t)(
		struct aos_value_map *map, 
		const char * const str, 
		const int dft);

typedef int (*aos_value_map_serialize_t)(
		struct aos_value_map *map, 
		struct aos_xml_node *parent);

typedef int (*aos_value_map_deserialize_t)(
		struct aos_value_map *map, 
		struct aos_xml_node *node);

typedef int (*aos_value_map_hold_t)(
		struct aos_value_map *map); 

typedef int (*aos_value_map_put_t)(
		struct aos_value_map *map); 

#define AOS_VALUE_MAP_MEMFUNC_DECL						\
	aos_value_map_get_index_t			get_index;		\
	aos_value_map_serialize_t			serialize;		\
	aos_value_map_deserialize_t			deserialize;	\
	aos_value_map_hold_t				hold;			\
	aos_value_map_put_t					put


#define AOS_VALUE_MAP_MEMDATA_DECL						\
	int			noe;									\
	char **		str_values;								\
	int *		index;

typedef struct aos_value_map_mf
{
	AOS_VALUE_MAP_MEMFUNC_DECL;
} aos_value_map_mf_t;

typedef struct aos_value_map
{
	aos_value_map_mf_t *mf;

	AOS_VALUE_MAP_MEMDATA_DECL;
} aos_value_map_t;


aos_value_map_t * aos_value_map_create(struct aos_xml_node *node);
#endif

