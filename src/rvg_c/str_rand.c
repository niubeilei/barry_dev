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
// 02/14/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/str_rand.h"

#if 0
// 
// Description
// This function randomly generates string pairs in a structure shown
// below:
//    root:
//       comp_child:
//           child:
//           child:
//           ...
//       comp_child:
//           ...
//       atomic_child value
//       ...
// There are composite children and atomic children. Atomic children 
// is defined as a pair (name, value). Composite children is defined
// as (name, list of children), where children can be either atomic
// children or composite children. The root is the top level. 
//
// Parameters
// total_max_item:	The total number of items allowed. -1 means no max
// max_children:	The max children of a node. -1 means no max.
// max_level:		The max levels of the tree.
//
//
int aos_str_rand_strpairs(
		const int total_max_item,
		const int max_children,
		const int max_level, 
		aos_strpair_t *node)
{
	aos_assert_r(total_max_item == -1 || total_max_item > 0, -1);
	aos_assert_r(max_children == -1 || max_children > 0, -1);
	aos_assert_r(max_level == -1 || max_level > 0, -1);
	aos_assert_r(node, -1);

	int total_nodes = aos_next_int(1, total_max_item);
	int noe = 0;
}

#endif

