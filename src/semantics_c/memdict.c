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
// Memory Dictionary maintains a dictionary about memory blocks. Each
// memory block is defined as:
// 	(start_address, length). 
// For each memory block, some of its internal addresses may be referenced.
// For an array, for instance, the beginning address of each element is
// addressable. Some are not addressable unless it is specially marked. 
// For instance, for an array of integers, addressable addresses are:
// 			start_address, 
// 			start_address + 4, 
// 			start_address + 8, 
// 			...
// 			start_address + 4 * (num_elem - 1)
//
// Modification History:
// 12/12/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semantics_c/memdict.h"

