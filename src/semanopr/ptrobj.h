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
#ifndef Aos_semanopr_ptrobj_h
#define Aos_semanopr_ptrobj_h

#include "aosUtil/Types.h"
#include "semanopr/ptrtype.h"

struct aos_ptrobj_t
{
	u32					start_addr; 
	u32					crt_addr;
	aos_ptrtype_e	ptr_type;
};

extern struct aos_ptrobj_t * aos_ptrobj_create(
									u32 start, 
									u32 addr, 
									aos_ptrtype_e type);

#endif // End of Include

