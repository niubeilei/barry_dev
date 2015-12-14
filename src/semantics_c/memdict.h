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
#ifndef aos_semantics_mem_dict_h
#define aos_semantics_mem_dict_h

#include "aosUtil/Types.h"
struct aos_memblock_t;

extern int aos_memdict_init();
extern int aos_memdict_is_valid_addr(u32 addr);
extern int aos_memdict_add(struct aos_memblock_t *block);
extern int aos_memdict_remove(struct aos_memblock_t *block);

#endif // End of Include

