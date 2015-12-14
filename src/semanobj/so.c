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
// 12/11/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semanobj/so.h"

#include "alarm_c/alarm.h"
#include "aosUtil/Memory.h"
#include "porting_c/mutex.h"


struct aos_so_class aos_so_class_func = 
{
	aos_so_lock,
	aos_so_unlock, 
	aos_so_get_type
};


struct aos_so *
aos_so_constructor(void *inst, enum aos_so_type_e type)
{
	struct aos_so *obj = (struct aos_so *)aos_malloc(sizeof(struct aos_so));
	aos_assert_r(obj, 0);

//	aos_init_lock(&(obj->lock));
	obj->inst = inst;
	obj->ref = 0;
	obj->type = type;
	obj->member_funcs = &aos_so_class_func;

	return 0;
}


int
aos_so_lock(struct aos_so *obj)
{
	return aos_lock(&obj->lock);
	return 0;
}	


int
aos_so_unlock(struct aos_so *obj)
{
//	return aos_unlock(&obj->lock);
	return 0;
}


enum aos_so_type_e
aos_so_get_type(struct aos_so *obj)
{
	return obj->type;
}


