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
// Object Dictionary keeps track of objects. Objects are identified
// by their starting addresses. 
//   
//
// Modification History:
// 12/14/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semantics_c/objdict.h"

#include "alarm_c/alarm.h"
#include "aos_util/hashtab.h"
#include "debug_c/rc.h"
#include "porting_c/mutex.h"
#include "semanobj/so.h"
#include "semanobj/so_util.h"
#include "semantics_c/objdict_priv.h"
#include "semantics_c/objentry.h"
#include <stdio.h>

static struct aos_hashtab * sg_objtab = 0;
static int sg_objdict_init_flag = 0;
static aos_lock_t sg_lock = PTHREAD_MUTEX_INITIALIZER;

static u32 
aos_objdict_hashfunc(struct aos_hashtab *table, void *key)
{
	return (u32)key;
}


static int 
aos_objdict_keycmp(struct aos_hashtab *table, 
				 void *key1, 
				 void *key2)
{
	if (key1 < key2) return -1; 
	if (key1 == key2) return 0;
	return 1;
}


int 
aos_objdict_init()
{
	int ret = 0;

	if (sg_objdict_init_flag) return 0;

	aos_lock(&sg_lock);
	if (!sg_objdict_init_flag)
	{
		sg_objtab = aos_hashtab_create(aos_objdict_hashfunc, 
						aos_objdict_keycmp, 
						AOS_OBJDICT_TABLE_SIZE, 
						AOS_OBJDICT_TABLE_MAX);
		if (!sg_objtab)
		{
			printf("********** Failed to allocate memory");
			ret = -eAosRc_OutOfMemory;
		}
		else
		{
			sg_objdict_init_flag = 1;
		}
	}
	aos_unlock(&sg_lock);
	return ret;
}


aos_objentry_t *aos_objdict_add(const char * const filename, 
				    const int lineno, 
				    void *key, 
					void *user_data,
					const char * const name,
					const aos_entry_type_e entry_type,
				    const aos_data_type_e data_type, 
				    aos_mat_t alloc_type, 
				    const u32 size)
{
	int ret = 0;
	AOS_OBJDICT_CHECK_INIT;
	aos_assert_r(filename, 0);
	aos_assert_r(key, 0);
	aos_assert_r(sg_objtab, 0);
	aos_assert_r(size, 0);

	aos_objentry_t *entry = aos_objentry_create(filename, lineno, key, 
			user_data, name, entry_type, data_type, alloc_type, size);
	aos_assert_r(entry, 0);

	ret = aos_hashtab_insert(sg_objtab, key, entry);
	if (ret)
	{
		aos_free(entry);
		aos_alarm("Failed to add so: %d", ret);
		return 0;
	}

	return entry;	
}


int 
aos_objdict_remove(void *ptr)
{
	int ret = 0;
	AOS_OBJDICT_CHECK_INIT;
	aos_assert_r(ptr, -eAosRc_NullPtr);

	ret = aos_hashtab_remove(sg_objtab, ptr, 0, 0);
	if (ret)
	{
		aos_alarm("Failed to remove so: %d", ret);
	}

	return ret;
}


// 
// This function retrieves the entry identified by the object's
// starting address. 
//
aos_objentry_t * aos_objdict_get(void *key)
{
	return (aos_objentry_t *)aos_hashtab_search(sg_objtab, key);
}


