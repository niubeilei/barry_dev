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
// 12/12/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semantics_mem_dict_priv_h
#define aos_semantics_mem_dict_priv_h

#include "aosUtil/Types.h"


struct aos_memblock_t
{
	u64		start_addr; 
	u32		length;
};


extern int aos_memblock_is_addressable(struct aos_memblock_t *block, 
								u32 addr);

#endif // End of Include

