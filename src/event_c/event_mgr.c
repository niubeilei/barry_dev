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
// 03/03/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "event_c/event_mgr.h"

#include "alarm_c/alarm.h"
#include "event_c/event.h"
#include "porting_c/mutex.h"
#include "porting_c/cond_var.h"
#include "thread_c/thread.h"
#include "util_c/memory.h"
#include "util_c/tracer.h"
#include "util_c/hashtab.h"
#include "util_c/global_data.h"


#define AOS_EVENTMGR_MAX_RCD_PER_EVENT 1000

static aos_lock_t sg_lock;
static aos_list_head_t sg_event_queue;
static aos_condvar_t sg_condvar;
static aos_thread_t *sg_thread;
static aos_hashtab_t *sg_hashtab = 0;
static u32 sg_reg_id = 100;
static int sg_init_flag = 0;

#include "net/read_thrd.h"

static u32 hash_func(aos_hashtab_t *tab, void *key)
{
	aos_assert_r(tab, 0);
	aos_assert_r(key, 0);
	aos_event_rcd_t *rcd = (aos_event_rcd_t*)key;

	u32 k = rcd->type + rcd->match_data;
	return k;
}


static int key_comp_func(
		aos_hashtab_t *tab, 
		void *key1, 
		void *key2)
{
	aos_assert_r(tab, -1);
	aos_assert_r(key1, -1);
	aos_assert_r(key2, -1);

	aos_event_rcd_t *rcd1 = (aos_event_rcd_t *)key1;
	aos_event_rcd_t *rcd2 = (aos_event_rcd_t *)key2;
	return (rcd1->type == rcd2->type &&
			rcd1->match_data == rcd2->match_data)?0:1;
}


static int return_pending_rcd(aos_pending_rcd_t *record)
{
	aos_assert_r(record, -1);
	aos_free(record);
	return 0;
}


static int return_event_rcd(aos_event_rcd_t *entry)
{
	aos_assert_r(entry, -1);
	aos_free(entry);
	return 0;
}


static int put_event_rcd(aos_event_rcd_t *rcd)
{
	aos_lock(&sg_lock);
	rcd->ref_count--;
	if (rcd->ref_count == 0) return_event_rcd(rcd);
	aos_unlock(&sg_lock);
	return 0;
}


static int destroy_event_rcd(aos_event_rcd_t *rcd)
{
	aos_assert_r(rcd, -1);
	aos_free(rcd);
	return 0;
}


static void hashtab_put_event_rcd(void *key, void *data, void *args)
{
	// 
	// This function is used when to remove an event registry.
	// 'data' is the entry to be removed. It should be the 
	// type 'aos_event_entry_t".
	//
	// When this function is called, the lock was locked.
	//
	aos_assert(data);
	aos_event_rcd_t *entry = (aos_event_rcd_t *)data;
	entry->ref_count--;
	if (entry->ref_count <= 0) destroy_event_rcd(entry);
	return;
}


static int proc_event(aos_event_t *event)
{
	int i;
	aos_assert_r(event, -1);
	aos_assert_r(aos_event_check(event->type) == 1, -1);

aos_trace("Process event: %d:%d", event->type, event->match_data);
	aos_lock(&sg_lock);
	aos_dyn_array_t array;
	aos_dyn_array_init(&array, 0, 4, 5, 
			AOS_EVENTMGR_MAX_RCD_PER_EVENT);
	int entries = aos_hashtab_search_multiple(sg_hashtab, event, &array);

	if (entries < 0)
	{
		aos_alarm("Failed to search");
		aos_unlock(&sg_lock);
		return -1;
	}

	if (entries == 0)
	{
		aos_unlock(&sg_lock);
		aos_trace("No callbacks!");
		return 0;
	}

	// Hold the records and then unlock
	aos_event_rcd_t **records = (aos_event_rcd_t **)array.buffer;
	for (i=0; i<array.noe; i++)
	{
		records[i]->ref_count++;
	}
	aos_unlock(&sg_lock);

	for (i=0; i<array.noe; i++)
	{
		records[i]->callback(event, records[i]->user_data);
		put_event_rcd(records[i]);
	}	

	return 0;
}


static void aos_eventmgr_thrd_func(aos_thread_t *thread)
{
	aos_assert(thread);
	aos_pending_rcd_t *entry;

	while (1)
	{
		aos_lock(&sg_lock);
		if (aos_list_empty(&sg_event_queue))
		{
			aos_condvar_wait(&sg_condvar, &sg_lock);
			aos_unlock(&sg_lock);
			continue;
		}

		entry = (aos_pending_rcd_t *)sg_event_queue.next;
		aos_list_del(&entry->link);
		aos_unlock(&sg_lock);
		aos_trace("Received an event");

		proc_event(entry->event);
		return_pending_rcd(entry);		
	}
}


static int aos_event_mgr_init()
{
	aos_global_lock();
	if (sg_init_flag)
	{
		aos_global_unlock();
		return 0;
	}

	aos_init_lock(&sg_lock);
	aos_condvar_init(&sg_condvar);
	AOS_INIT_LIST_HEAD(&sg_event_queue);
	sg_thread = aos_thread_create("eventmgr", 0, 
			aos_eventmgr_thrd_func, 
			0, 1, 1, 0);
	aos_assert_g(sg_thread, cleanup);
	sg_hashtab = aos_hashtab_create(hash_func, key_comp_func, 4095);
	aos_assert_g(sg_hashtab, cleanup);
	sg_init_flag = 1;
	aos_global_unlock();
	return 0;

cleanup:
	aos_global_unlock();
	return -1;
}


static aos_event_rcd_t *create_event_rcd(
		const aos_event_e event_type,
		aos_event_callback_t callback,
		const int match_data, 
		void *user_data, 
		u32 *reg_id)
{
	aos_assert_r(reg_id, 0);
	aos_event_rcd_t *entry = aos_malloc(sizeof(aos_event_rcd_t));
	aos_assert_r(entry, 0);
	entry->type = event_type;
	entry->callback = callback;
	entry->user_data = user_data;
	entry->match_data = match_data;
	AOS_INIT_LIST_HEAD(&(entry->link));
	entry->ref_count = 0;
	if (*reg_id == 0)
	{
		aos_lock(&sg_lock);
		entry->reg_id = sg_reg_id++;
		*reg_id = entry->reg_id;
		aos_unlock(&sg_lock);
	}
	else
	{
		entry->reg_id = *reg_id;
	}
	return entry;
}


static aos_pending_rcd_t *create_pending_rcd(aos_event_t *event) 
{
	aos_pending_rcd_t *entry = aos_malloc(sizeof(*entry));
	aos_assert_r(entry, 0);
	memset(entry, 0, sizeof(*entry));
	entry->event = event;
	event->mf->hold(event);
	entry->ref_count = 0;
	AOS_INIT_LIST_HEAD(&(entry->link));
	return entry;
}


// 
// Description
// It registers the event 'event_type' with the match data 
// 'match_data' and 'user_data'. If *reg_id == 0, it will 
// assign a unique registration id. This id can be used to 
// cancel the registration. If *reg_id != 0, it will use
// the registration id instead of assigning a new one. 
//
int aos_register_event(
		const aos_event_e event_type,
		aos_event_callback_t callback,
		const int match_data, 
		void *user_data, 
		u32 *reg_id)
{
	if (!sg_init_flag) 
	{
		aos_assert_r(!aos_event_mgr_init(), -1);
	}
aos_trace("To register event: %d:%d", event_type, match_data);
	aos_assert_r(reg_id, -1);
	aos_assert_r(aos_event_check(event_type) == 1, -1);
	aos_assert_r(callback, -1);

	aos_event_rcd_t *entry = create_event_rcd(event_type, callback, 
			match_data, user_data, reg_id);
	aos_assert_r(entry, -1);

	aos_lock(&sg_lock);
	entry->ref_count++;
	aos_hashtab_insert(sg_hashtab, entry, entry);
	aos_unlock(&sg_lock);
	return 0;
}


int aos_unregister_event(
		const aos_event_e event_type,
		aos_event_callback_t callback, 
		const int match_data, 
		const u32 reg_id)
{
	aos_event_rcd_t entry;

	if (!sg_init_flag) 
	{
		aos_assert_r(!aos_event_mgr_init(), -1);
	}

aos_trace("To unregister event: %d:%d", event_type, match_data);
	entry.type = event_type;
	entry.match_data = match_data;
	entry.reg_id = reg_id;
	int ret = aos_hashtab_remove(sg_hashtab, &entry, 
			hashtab_put_event_rcd, 0);
	aos_lock(&sg_lock);

	if (ret == -ENOENT) 
	{
		aos_alarm("Not found");
		return -1;
	}

	return 0;
}


int aos_add_event(aos_event_t *event)
{
	if (!sg_init_flag) 
	{
		aos_assert_r(!aos_event_mgr_init(), -1);
	}

	aos_pending_rcd_t *entry = create_pending_rcd(event);
	aos_assert_r(entry, -1);

	aos_lock(&sg_lock);
	aos_list_add(&entry->link, &sg_event_queue);
	aos_condvar_signal(&sg_condvar);
	aos_unlock(&sg_lock);

aos_trace("Event added: %d:%d", event->type, event->event_id);
	return 0;
}	

