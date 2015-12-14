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
// 01/17/2008: Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/hashtab.h"

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
// It creates a hashtable. The table size (i.e., the number of buckets)
// must be power(2, n) - 1 (i.e., all 0's followed by all 1's).
// If not, it is set to the nearest correct size.
//
aos_hashtab_t *
aos_hashtab_create(aos_hash_func_t hash_func,
                   aos_key_cmp_func_t keycmp,
                   u32 size)
{
	struct aos_hashtab *p;
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

	p = (struct aos_hashtab *)malloc(sizeof(*p));
	if (p == NULL)
		return p;

	memset(p, 0, sizeof(*p));
	p->size = size;
	p->nel = 0;
	p->hash_func = hash_func;
	p->keycmp = keycmp;
	p->htable = (struct aos_hashtab_node **)malloc(sizeof(*(p->htable)) * size);
	if (p->htable == NULL) {
		aos_free(p);
		return NULL;
	}

	for (i = 0; i < size; i++)
		p->htable[i] = NULL;

	return p;
}

int aos_hashtab_insert(struct aos_hashtab *h, void *key, void *datum)
{
	u32 hvalue;
	aos_hashtab_node_t *prev, *cur, *newnode;

	if (!h || h->nel == AOS_HASHTAB_MAX_NODES)
		return -EINVAL;

	hvalue = h->hash_func(h, key) & h->size;
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur && h->keycmp(h, key, cur->key) > 0) {
		prev = cur;
		cur = cur->next;
	}

	if (cur && (h->keycmp(h, key, cur->key) == 0))
		return -EEXIST;

	newnode = (struct aos_hashtab_node *)malloc(sizeof(*newnode));
	if (newnode == NULL)
		return -ENOMEM;
	memset(newnode, 0, sizeof(*newnode));
	newnode->key = key;
	newnode->datum = datum;
	if (prev) {
		newnode->next = prev->next;
		prev->next = newnode;
	} else {
		newnode->next = h->htable[hvalue];
		h->htable[hvalue] = newnode;
	}

	h->nel++;
	return 0;
}

int aos_hashtab_remove(struct aos_hashtab *h, void *key,
		   void (*destroy)(void *k, void *d, void *args),
		   void *args)
{
	u32 hvalue;
	struct aos_hashtab_node *cur, *last;

	if (!h)
		return -EINVAL;

	hvalue = h->hash_func(h, key) & h->size;
	last = NULL;
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(h, key, cur->key) > 0) {
		last = cur;
		cur = cur->next;
	}

	if (cur == NULL || (h->keycmp(h, key, cur->key) != 0))
		return -ENOENT;

	if (last == NULL)
		h->htable[hvalue] = cur->next;
	else
		last->next = cur->next;

	if (destroy)
		destroy(cur->key, cur->datum, args);
	aos_free(cur);
	h->nel--;
	return 0;
}

int aos_hashtab_replace(struct aos_hashtab *h, void *key, void *datum,
		    void (*destroy)(void *k, void *d, void *args),
		    void *args)
{
	u32 hvalue;
	struct aos_hashtab_node *prev, *cur, *newnode;

	if (!h || h->nel == AOS_HASHTAB_MAX_NODES)
	hvalue = h->hash_func(h, key) & h->size;
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(h, key, cur->key) > 0) {
		prev = cur;
		cur = cur->next;
	}

	if (cur && (h->keycmp(h, key, cur->key) == 0)) {
		if (destroy)
			destroy(cur->key, cur->datum, args);
		cur->key = key;
		cur->datum = datum;
	} else {
		newnode = (struct aos_hashtab_node *)malloc(sizeof(*newnode));
		if (newnode == NULL)
			return -ENOMEM;
		memset(newnode, 0, sizeof(*newnode));
		newnode->key = key;
		newnode->datum = datum;
		if (prev) {
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

void *aos_hashtab_search(struct aos_hashtab *h, void *key)
{
	u32 hvalue;
	struct aos_hashtab_node *cur;

	if (!h)
		return NULL;

	hvalue = h->hash_func(h, key) & h->size;
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(h, key, cur->key) > 0)
		cur = cur->next;

	if (cur == NULL || (h->keycmp(h, key, cur->key) != 0))
		return NULL;

	return cur->datum;
}


// 
// Description
// Sometimes a hashtab may be used to hold entries whose 
// keys may be the same. This function retrieves all the 
// entries that match the key.
//
int aos_hashtab_search_multiple(
		aos_hashtab_t *h, 
		void *key, 
		aos_dyn_array_t *array)
{
	u32 hvalue;
	struct aos_hashtab_node *cur;

	aos_assert_r(array, -1);
	aos_assert_r(h, -1);

	array->noe = 0;
	void **entries = (void **)array->buffer;
	hvalue = h->hash_func(h, key) & h->size;
	cur = h->htable[hvalue];
	while (cur != NULL)
	{
		if (h->keycmp(h, key, cur->key) == 0)
		{
			aos_assert_r(!array->mf->add_element(array, 1, 
						(char **)&entries), -1);
			entries[array->noe++] = cur->datum;
		}
		cur = cur->next;
	}

	return array->noe;
}


void aos_hashtab_destroy(struct aos_hashtab *h)
{
	u32 i;
	struct aos_hashtab_node *cur, *temp;

	if (!h)
		return;

	for (i = 0; i < h->size; i++) {
		cur = h->htable[i];
		while (cur != NULL) {
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

int aos_hashtab_map(struct aos_hashtab *h,
		int (*apply)(void *k, void *d, void *args),
		void *args)
{
	u32 i;
	int ret;
	struct aos_hashtab_node *cur;

	if (!h)
		return 0;

	for (i = 0; i < h->size; i++) {
		cur = h->htable[i];
		while (cur != NULL) {
			ret = apply(cur->key, cur->datum, args);
			if (ret)
				return ret;
			cur = cur->next;
		}
	}
	return 0;
}


void aos_hashtab_map_remove_on_error(struct aos_hashtab *h,
                                 int (*apply)(void *k, void *d, void *args),
                                 void (*destroy)(void *k, void *d, void *args),
                                 void *args)
{
	u32 i;
	int ret;
	struct aos_hashtab_node *last, *cur, *temp;

	if (!h)
		return;

	for (i = 0; i < h->size; i++) {
		last = NULL;
		cur = h->htable[i];
		while (cur != NULL) {
			ret = apply(cur->key, cur->datum, args);
			if (ret) {
				if (last)
					last->next = cur->next;
				else
					h->htable[i] = cur->next;

				temp = cur;
				cur = cur->next;
				if (destroy)
					destroy(temp->key, temp->datum, args);
				aos_free(temp);
				h->nel--;
			} else {
				last = cur;
				cur = cur->next;
			}
		}
	}
	return;
}

void aos_hashtab_stat(struct aos_hashtab *h, struct aos_hashtab_info *info)
{
	u32 i, chain_len, slots_used, max_chain_len;
	struct aos_hashtab_node *cur;

	slots_used = 0;
	max_chain_len = 0;
	for (slots_used = max_chain_len = i = 0; i < h->size; i++) {
		cur = h->htable[i];
		if (cur) {
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
}


int aos_hashtab_insert_w_check(
		aos_hashtab_t *h, 
		void *key, 
		void *datum, 
		const int override_flag)
{
	u32 hvalue;
	aos_hashtab_node_t *prev, *cur, *newnode;

	aos_assert_r(h, -1);
	aos_assert_r(h->nel <= AOS_HASHTAB_MAX_NODES, -1);

	hvalue = h->hash_func(h, key) & h->size;
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur && h->keycmp(h, key, cur->key) > 0) {
		prev = cur;
		cur = cur->next;
	}

	if (cur && (h->keycmp(h, key, cur->key) == 0))
	{
		if (!override_flag) return -EEXIST;

		// Need to override the object
		aos_assert_r(h->override_func, -1);
		aos_assert_r(!h->override_func(h, datum, cur->datum), -1);
		return 0;
	}

	newnode = (struct aos_hashtab_node *)malloc(sizeof(*newnode));
	aos_assert_r(newnode, -1);
	memset(newnode, 0, sizeof(*newnode));
	newnode->key = key;
	newnode->datum = datum;
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

