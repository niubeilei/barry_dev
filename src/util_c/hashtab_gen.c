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
// Modification History:
// 03/09/2008: Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/hashtab_gen.h"

#include "alarm_c/alarm.h"
#include "util_c/memory.h"
#include "util_c/dyn_array.h"
#include <linux/errno.h>

static u32 sgSizes[] = 
{
	0xff,		// 255
	0x01ff,		// 511
	0x03ff,		// 1023
	0x07ff,		// 2047
	0x0fff,		// 4095
	0x1fff,		// 8191
	0x3fff,		// 16383
	0x7fff,		// 32767
	0xffff		// 65535
};
static u32 sgNumSizes = 9;

// 
// It creates a hashtab_genle. The table size (i.e., the number of buckets)
// must be power(2, n) - 1 (i.e., all 0's followed by all 1's).
// If not, it is set to the nearest correct size.
//
aos_hashtab_gen_t *
aos_hashtab_gen_create(aos_hashtab_gen_hash_func_t hash_func,
                   aos_hashtab_gen_key_cmp_func_t keycmp,
                   u32 size)
{
	struct aos_hashtab_gen *p;
	u32 i;

	// 
	// Set the size
	//
	for (i=0; i<sgNumSizes; i++)
	{
		if (size <= sgSizes[i])
		{
			size = sgSizes[i];
			break;
		}
	}

	if (size > sgSizes[sgNumSizes-1]) size = sgSizes[sgNumSizes-1];

	p = (struct aos_hashtab_gen *)malloc(sizeof(*p));
	aos_assert_r(p, 0);

	memset(p, 0, sizeof(*p));
	p->size = size;
	p->nel = 0;
	p->hash_func = hash_func;
	p->keycmp = keycmp;
	p->htable = (struct aos_hashtab_gen_node **)malloc(sizeof(*(p->htable)) * size);
	if (p->htable == NULL) 
	{
		aos_free(p);
		return NULL;
	}

	for (i = 0; i < size; i++)
	{
		p->htable[i] = NULL;
	}

	return p;
}


int aos_hashtab_gen_insert(
		struct aos_hashtab_gen *h, 
		void *obj, 
		const int override_flag, 
		int (*hold_func)(void *obj), 
		int (*put_func)(void *obj))
{
	u32 hvalue;
	aos_hashtab_gen_node_t *prev, *cur, *newnode;

	aos_assert_r(h, -1);
    aos_assert_r(h->nel < AOS_hashtab_gen_MAX_NODES, -1);

	hvalue = h->hash_func(h, obj) & h->size;
	prev = NULL;
	cur = h->htable[hvalue];
	int rslt;
	while (cur && (rslt = h->keycmp(h, obj, cur->obj)) > 0) 
	{
		prev = cur;
		cur = cur->next;
	}

	if (cur && rslt == 0)
	{
		aos_assert_r(override_flag, -1);
		if (put_func) put_func(cur->obj);
		if (hold_func) hold_func(obj);
		cur->obj = obj;
		return 0;
	}

	// Did not find it. Insert it.
	newnode = (struct aos_hashtab_gen_node *)malloc(sizeof(*newnode));
	aos_assert_r(newnode, -1);
	memset(newnode, 0, sizeof(*newnode));
	newnode->obj = obj;
	if (hold_func) hold_func(obj);
	if (prev) 
	{
		newnode->next = prev->next;
		prev->next = newnode;
	} 
	else 
	{
		newnode->next = h->htable[hvalue];
		h->htable[hvalue] = newnode;
	}

	h->nel++;
	return 0;
}


int aos_hashtab_gen_remove(
		aos_hashtab_gen_t *h, 
		void *obj,
		aos_hashtab_gen_destroy_func_t destroy,
		void *args)
{
	u32 hvalue;
	struct aos_hashtab_gen_node *cur, *last;

	aos_assert_r(h, -1);
	hvalue = h->hash_func(h, obj) & h->size;
	last = NULL;
	cur = h->htable[hvalue];
	int rslt;
	while (cur != NULL && (rslt = h->keycmp(h, obj, cur->obj)) > 0) 
	{
		last = cur;
		cur = cur->next;
	}

	aos_assert_r(cur && rslt == 0, -1);

	if (last == NULL)
		h->htable[hvalue] = cur->next;
	else
		last->next = cur->next;

	if (destroy) destroy(cur->obj, args);
	aos_free(cur);
	h->nel--;
	return 0;
}


int aos_hashtab_gen_replace(
		aos_hashtab_gen_t *h, 
		void *obj,
		aos_hashtab_gen_destroy_func_t destroy,
		void *args)
{
	u32 hvalue;
	struct aos_hashtab_gen_node *prev, *cur, *newnode;

	aos_assert_r(h, -1);
	aos_assert_r(obj, -1);

	hvalue = h->hash_func(h, obj) & h->size;
	prev = NULL;
	cur = h->htable[hvalue];
	int rslt;
	while (cur != NULL && (rslt = h->keycmp(h, obj, cur->obj)) > 0) 
	{
		prev = cur;
		cur = cur->next;
	}

	if (cur && rslt == 0)
	{
		if (destroy) destroy(cur->obj, args);
		cur->obj = obj;
	} 
	else 
	{
		aos_assert_r(h->nel < AOS_hashtab_gen_MAX_NODES, -1);
		newnode = (struct aos_hashtab_gen_node *)malloc(sizeof(*newnode));
		aos_assert_r(newnode, -1);
		memset(newnode, 0, sizeof(*newnode));
		newnode->obj = obj;
		if (prev) 
		{
			newnode->next = prev->next;
			prev->next = newnode;
		} else {
			newnode->next = h->htable[hvalue];
			h->htable[hvalue] = newnode;
		}
		h->nel++;
	}

	return 0;
}


void *aos_hashtab_gen_search(
		aos_hashtab_gen_t *h, 
		void *obj) 
{
	u32 hvalue;
	struct aos_hashtab_gen_node *cur;

	aos_assert_r(h, 0);

	hvalue = h->hash_func(h, obj) & h->size;
	cur = h->htable[hvalue];
	int rslt;
	while (cur && (rslt = h->keycmp(h, obj, cur->obj)) > 0) cur = cur->next;

	if (cur && rslt == 0) return cur->obj;

	return NULL;
}


// 
// Description
// Sometimes a hashtab_gen may be used to hold entries whose 
// keys may be the same. This function retrieves all the 
// entries that match the key.
//
int aos_hashtab_gen_search_multiple(
		aos_hashtab_gen_t *h, 
		void *obj, 
		aos_dyn_array_t *array)
{
	u32 hvalue;
	struct aos_hashtab_gen_node *cur;

	aos_assert_r(array, -1);
	aos_assert_r(h, -1);
	aos_assert_r(obj, -1);

	array->noe = 0;
	void **entries = (void **)array->buffer;
	hvalue = h->hash_func(h, obj) & h->size;
	cur = h->htable[hvalue];
	while (cur != NULL)
	{
		if (h->keycmp(h, obj, cur->obj) == 0)
		{
			aos_assert_r(!array->mf->add_element(array, 1, 
						(char **)&entries), -1);
			entries[array->noe++] = cur->obj;
		}
		cur = cur->next;
	}

	return array->noe;
}


void aos_hashtab_gen_destroy(
		aos_hashtab_gen_t *h, 
		aos_hashtab_gen_destroy_func_t destroy, 
		void *args)
{
	u32 i;
	struct aos_hashtab_gen_node *cur, *temp;

	aos_assert(h);

	for (i = 0; i < h->size; i++) 
	{
		cur = h->htable[i];
		while (cur != NULL) 
		{
			if (destroy) aos_assert(!destroy(cur->obj, args));
			temp = cur;
			cur = cur->next;
			aos_free(temp);
		}
		h->htable[i] = NULL;
	}

	aos_free(h->htable);
	h->htable = NULL;

	aos_free(h);
}

int aos_hashtab_gen_map(
		struct aos_hashtab_gen *h,
		int (*apply)(void *obj, void *args),
		void *args)
{
	u32 i;
	int ret;
	struct aos_hashtab_gen_node *cur;

	aos_assert_r(h, -1);
	aos_assert_r(apply, -1);

	for (i = 0; i < h->size; i++) 
	{
		cur = h->htable[i];
		while (cur != NULL) 
		{
			ret = apply(cur->obj, args);
			if (ret)
				return ret;
			cur = cur->next;
		}
	}
	return 0;
}


int aos_hashtab_gen_map_remove_on_error(
		aos_hashtab_gen_t *h,
        int (*apply)(void *d, void *args),
        aos_hashtab_gen_destroy_func_t destroy,
        void *args)
{
	u32 i;
	int ret;
	struct aos_hashtab_gen_node *last, *cur, *temp;

	aos_assert_r(h, -1);
	aos_assert_r(apply, -1);

	for (i = 0; i < h->size; i++) 
	{
		last = NULL;
		cur = h->htable[i];
		while (cur != NULL) 
		{
			ret = apply(cur->obj, args);
			if (ret) 
			{
				if (last)
					last->next = cur->next;
				else
					h->htable[i] = cur->next;

				temp = cur;
				cur = cur->next;
				if (destroy)
					destroy(temp->obj, args);
				aos_free(temp);
				h->nel--;
			} 
			else 
			{
				last = cur;
				cur = cur->next;
			}
		}
	}

	return 0;
}

int aos_hashtab_gen_stat(
		aos_hashtab_gen_t *h, 
		struct aos_hashtab_gen_info *info)
{
	u32 i, chain_len, slots_used, max_chain_len;
	struct aos_hashtab_gen_node *cur;

	slots_used = 0;
	max_chain_len = 0;
	for (slots_used = max_chain_len = i = 0; i < h->size; i++) 
	{
		cur = h->htable[i];
		if (cur) 
		{
			slots_used++;
			chain_len = 0;
			while (cur) {
				chain_len++;
				cur = cur->next;
			}

			if (chain_len > max_chain_len)
				max_chain_len = chain_len;
		}
	}

	info->slots_used = slots_used;
	info->max_chain_len = max_chain_len;
	return 0;
}

