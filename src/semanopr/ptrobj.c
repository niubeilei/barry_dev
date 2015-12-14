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
#include "semanopr/ptrobj.h"

#include "alarm_c/alarm.h"
#include "aosUtil/ReturnCode.h"
#include "aosUtil/Memory.h"
#include <string.h>


static int aos_ptrobj_init(struct aos_ptrobj_t *obj)
{
	aos_assert_r(obj, -eAosRc_NullPointer);
	memset(obj, 0, sizeof(struct aos_ptrobj_t));
	return 0;
}

struct aos_ptrobj_t * aos_ptrobj_create(
									u32 start, 
									u32 addr, 
									aos_ptrtype_e type)
{
	struct aos_ptrobj_t *obj = aos_malloc(sizeof(struct aos_ptrobj_t));
	aos_assert_r(obj, NULL);
	aos_ptrobj_init(obj);
	obj->start_addr = start;
	obj->crt_addr = addr;
	obj->ptr_type = type;
	return obj;
}


