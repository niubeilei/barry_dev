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
// 02/12/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/value_map.h"

#include "alarm_c/alarm.h"
#include "util_c/memory.h"

int aos_value_map_get_index(
		struct aos_value_map *map, 
		const char * const str, 
		const int dft)
{
	int i;
	aos_assert_r(map, dft);
	aos_assert_r(str, dft);

	for (i=0; i<map->noe; i++)
	{
		if (strcmp(str, map->str_values[i]) == 0) return map->index[i];
	}

	return dft;	
}


int aos_value_map_serialize(
		struct aos_value_map *map, 
		struct aos_xml_node *parent)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_value_map_deserialize(
		struct aos_value_map *map, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_value_map_hold(struct aos_value_map *map)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_value_map_put(struct aos_value_map *map)
{
	aos_not_implemented_yet;
	return -1;
}


static aos_value_map_mf_t sg_mf = 
{
	aos_value_map_get_index, 
	aos_value_map_serialize, 
	aos_value_map_deserialize, 
	aos_value_map_hold, 
	aos_value_map_put
};




aos_value_map_t * aos_value_map_create(struct aos_xml_node *node)
{
	aos_assert_r(node, 0);

	aos_value_map_t *map = aos_malloc(sizeof(aos_value_map_t));
	aos_assert_r(map, 0);
	memset(map, 0, sizeof(aos_value_map_t));
	map->mf = &sg_mf;

	aos_assert_g(!map->mf->deserialize(map, node), cleanup);
	return map;

cleanup:
	aos_free(map);
	return 0;
}


