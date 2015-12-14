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
#include "semanobj/so_mgr.h"

#include "alarm_c/alarm.h"
#include "aos_util/hashtab.h"
#include "aosUtil/ReturnCode.h"
#include "porting_c/mutex.h"
#include "semanobj/so.h"
#include "semanobj/so_util.h"
#include <stdio.h>

static struct aos_hashtab * sg_objtab = 0;
static int sg_somgr_init_flag = 0;
static aos_lock_t sg_lock = PTHREAD_MUTEX_INITIALIZER;

static u32 
aos_somgr_hashfunc(struct aos_hashtab *table, void *key)
{
	return (u32)key;
}


static int 
aos_somgr_keycmp(struct aos_hashtab *table, 
				 void *key1, 
				 void *key2)
{
	if (key1 < key2) return -1; 
	if (key1 == key2) return 0;
	return 1;
}


int 
aos_somgr_init()
{
	int ret = 0;

	if (sg_somgr_init_flag) return 0;

	aos_lock(&sg_lock);
	if (!sg_somgr_init_flag)
	{
		sg_objtab = aos_hashtab_create(aos_somgr_hashfunc, 
						aos_somgr_keycmp, 
						AOS_SO_MGR_TABLE_SIZE, 
						AOS_SO_MGR_TABLE_MAX);
		if (!sg_objtab)
		{
			printf("********** Failed to allocate memory");
			ret = -eAosRc_OutOfMemory;
		}
		else
		{
			sg_somgr_init_flag = 1;
		}
	}
	aos_unlock(&sg_lock);
	return ret;
}


int 
aos_somgr_add_so(struct aos_so *ptr)
{
	aos_assert_r(ptr, -eAosRc_NullPtr);
	aos_somgr_init();

	aos_assert_r(sg_objtab, -eAosRc_NullPtr);
	int ret = aos_hashtab_insert(sg_objtab, ptr->inst, ptr);
	if (ret)
	{
		aos_alarm("Failed to add so: %d", ret);
	}

	aos_hold_so(ptr);
	return ret;	
}


int 
aos_somgr_remove_so(struct aos_so *ptr)
{
	aos_assert_r(ptr, -eAosRc_NullPtr);
	aos_somgr_init();

	int ret = aos_hashtab_remove(sg_objtab, ptr->inst, 0, 0);
	if (ret)
	{
		aos_alarm("Failed to remove so: %d", ret);
	}

	aos_put_so(ptr);
	return ret;
}

