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
// 03/09/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util2/clipboard.h"

#include "alarm_c/alarm.h"
#include "porting_c/mutex.h"
#include "util_c/list.h"
#include "util_c/types.h"
#include "util_c/hashtab_gen.h"
#include "util_c/magic.h"
#include "util_c/memory.h"
#include "util2/global_data.h"
#include "thread_c/named_var.h"


static int 					sg_clipboard_init = 0;
static aos_list_head_t 		sg_clipboard;
static aos_lock_t 			sg_lock;
static aos_hashtab_gen_t *	sg_hashtab = 0;


static u32 hash_func(
		aos_hashtab_gen_t *hashtab, 
		void *obj)
{
	u32 key = 0;
	aos_named_var_t *var = (aos_named_var_t *)obj;
	aos_assert_r(var->magic == AOS_NAMED_VAR_MAGIC, -1);

	key = var->tag;
	int idx = 0;
	int max = 10;
	char *buff = var->name;
	char c;
	while (idx < max && (c = buff[idx++])) key += c;
	return key;
}


static int key_cmp_func(
		aos_hashtab_gen_t *hashtab,
		void *key1, 
		void *key2)
{
	aos_assert_r(key1, -1);
	aos_assert_r(key2, -1);
	aos_named_var_t *var1 = (aos_named_var_t *)key1;
	aos_assert_r(var1->magic == AOS_NAMED_VAR_MAGIC, -1);
	aos_named_var_t *var2 = (aos_named_var_t *)key2;
	aos_assert_r(var2->magic == AOS_NAMED_VAR_MAGIC, -1);

	return (var1->tag == var2->tag && strcmp(var1->name, var2->name) == 0)?0:1;
}


int aos_clipboard_init()
{
	aos_global_lock();
	if (sg_clipboard_init) 
	{
		aos_global_unlock();
		return 0;
	}

	AOS_INIT_LIST_HEAD(&sg_clipboard);
	aos_init_lock(&sg_lock);
	sg_hashtab = aos_hashtab_gen_create(hash_func, key_cmp_func, 4096);
	aos_assert_g(sg_hashtab, cleanup);
	sg_clipboard_init = 1;
	aos_global_unlock();
	return 0;

cleanup:
	aos_global_unlock();
	return -1;
}


int aos_clipboard_set_ptr(
		const aos_data_type_e type, 
		const char * const name, 
		void *ptr, 
		const int override_flag)
{
	aos_assert_r(sg_clipboard_init, -1);

	aos_lock(&sg_lock);
	aos_named_var_t *var = aos_named_var_create_ptr(type, name, ptr);
	aos_assert_g(var, cleanup);
	aos_assert_g(!aos_hashtab_gen_insert(sg_hashtab, var, 0, 0, 0), cleanup);
	aos_unlock(&sg_lock);	
	return 0;

cleanup:
	if (var) aos_free(var);
	aos_unlock(&sg_lock);
	return -1;
}
	

int aos_clipboard_get_ptr(
		const aos_data_type_e type, 
		const char * const name, 
		void **ptr)
{
	aos_assert_r(sg_clipboard_init, -1);

	aos_lock(&sg_lock);
	aos_named_var_t var;
    aos_named_var_init_partial(&var, type, name);
	void *entry = aos_hashtab_gen_search(sg_hashtab, &var);
	aos_assert_g(entry, cleanup);
	aos_named_var_t *vv = (aos_named_var_t *)entry;
	aos_assert_g(!vv->value.mf->to_ptr(&vv->value, ptr), cleanup); 
	aos_unlock(&sg_lock);
	return 0;

cleanup:
	aos_unlock(&sg_lock);
	return -1;
}
	

int aos_clipboard_set_str(
		const aos_data_type_e type, 
		const char * const name, 
		char *value, 
		const int override_flag)
{
	aos_assert_r(sg_clipboard_init, -1);

	aos_lock(&sg_lock);
	aos_named_var_t *var = aos_named_var_create_str(type, name, value);
	aos_assert_r(var, -1);
	aos_assert_g(!aos_hashtab_gen_insert(sg_hashtab, var, 0, 0, 0), cleanup);
	aos_unlock(&sg_lock);
	return 0;

cleanup:
	aos_free(var);
	aos_unlock(&sg_lock);
	return -1;
}
	

int aos_clipboard_get_str(
		const aos_data_type_e type, 
		const char * const name, 
		char **str, 
		int *len)
{
	aos_assert_r(sg_clipboard_init, -1);

	aos_lock(&sg_lock);
	aos_named_var_t var;
    aos_assert_g(!aos_named_var_init_partial(&var, type, name), cleanup);
	void *entry = aos_hashtab_gen_search(sg_hashtab, &var);
	aos_assert_g(entry, cleanup);
	aos_named_var_t *vv = (aos_named_var_t *)entry;
	aos_assert_g(!vv->value.mf->to_str(&vv->value, str, len), cleanup); 
	aos_unlock(&sg_lock);
	return 0;

cleanup:
	aos_unlock(&sg_lock);
	return -1;
}
	

int aos_clipboard_set_int(
		const aos_data_type_e type, 
		const char * const name, 
		const int value, 
		const int override_flag)
{
	aos_assert_r(sg_clipboard_init, -1);

	aos_lock(&sg_lock);
	aos_named_var_t *var = aos_named_var_create_int(type, name, value);
	aos_assert_g(var, cleanup);
	aos_assert_g(!aos_hashtab_gen_insert(sg_hashtab, var, 0, 0, 0), cleanup);
	
	return 0;

cleanup:
	if (var) aos_free(var);
	aos_unlock(&sg_lock);
	return -1;
}
	

