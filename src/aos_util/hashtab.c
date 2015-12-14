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
// 12/08/2007	Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "aos_util/hashtab.h"

#include "alarm_c/alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"

struct aos_hashtab *
aos_hashtab_create(aos_hash_func func, 
			   aos_hash_comp_func keycmp, 
			   const u32 size, 
			   const u32 max)
{
	struct aos_hashtab *p;
	u32 i;

	p = (struct aos_hashtab *)aos_malloc_atomic(sizeof(*p));
	if (p == NULL)
	{
		aos_alarm("Failed to create hash table: run out of memory");
		return p;
	}

	memset(p, 0, sizeof(*p));
	p->size = size;
	p->noe = 0;
	p->max = max;
	p->func = func;
	p->keycmp = keycmp;
	p->htable = (struct aos_hashtab_node **)aos_malloc_atomic(sizeof(*(p->htable)) * size);
	if (p->htable == NULL) 
	{
		aos_free(p);
		return NULL;
	}

	for (i = 0; i < size; i++)
		p->htable[i] = NULL;

	return p;
}

int aos_hashtab_insert(struct aos_hashtab *h, void *key, void *data)
{
	u32 hvalue;
	struct aos_hashtab_node *prev, *cur, *newnode;
	if (!h || h->noe >= h->max)
	{
		return -eAosRc_TooManyElem;
	}

	hvalue = h->func(h, key);
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur && h->keycmp(h, key, cur->key) > 0) 
	{
		prev = cur;
		cur = cur->next;
	}

	if (cur && (h->keycmp(h, key, cur->key) == 0))
	{
		return -eAosRc_ElemExist;
	}

	newnode = (struct aos_hashtab_node *)aos_malloc_atomic(sizeof(*newnode));
	if (newnode == NULL)
	{
		return -eAosRc_OutOfMemory;
	}

	memset(newnode, 0, sizeof(*newnode));
	newnode->key = key;
	newnode->data = data;
	if (prev) 
	{
		newnode->next = prev->next;
		prev->next = newnode;
	} else 
	{
		newnode->next = h->htable[hvalue];
		h->htable[hvalue] = newnode;
	}

	h->noe++;
	return 0;
}


int aos_hashtab_remove(struct aos_hashtab *h, void *key,
		   void (*destroy)(void *k, void *d, void *args),
		   void *args)
{
	u32 hvalue;
	struct aos_hashtab_node *cur, *last;

	aos_assert_r(h, -eAosRc_NullPointer);

	hvalue = h->func(h, key);
	last = NULL;
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(h, key, cur->key) > 0) 
	{
		last = cur;
		cur = cur->next;
	}

	if (cur == NULL || (h->keycmp(h, key, cur->key) != 0))
	{
		return -eAosRc_NotFound;
	}

	if (last == NULL)
	{
		h->htable[hvalue] = cur->next;
	}
	else
	{
		last->next = cur->next;
	}

	if (destroy)
	{
		destroy(cur->key, cur->data, args);
	}

	aos_free(cur);
	h->noe--;
	return 0;
}

int aos_hashtab_replace(struct aos_hashtab *h, 
					void *key, 
					void *data,
		    		void (*destroy)(void *k, void *d, void *args),
		    		void *args)
{
	u32 hvalue;
	struct aos_hashtab_node *prev, *cur, *newnode;

	aos_assert_r(h, -eAosRc_NullPointer);
	aos_assert_r(h->noe < h->max, -eAosRc_TooManyElem);

	hvalue = h->func(h, key);
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(h, key, cur->key) > 0) 
	{
		prev = cur;
		cur = cur->next;
	}

	if (cur && (h->keycmp(h, key, cur->key) == 0)) 
	{
		if (destroy)
		{
			destroy(cur->key, cur->data, args);
		}
		cur->key = key;
		cur->data = data;
	} 
	else 
	{
		newnode = (struct aos_hashtab_node *)aos_malloc_atomic(sizeof(*newnode));
		aos_assert_r(newnode, -eAosRc_OutOfMemory);
		memset(newnode, 0, sizeof(*newnode));
		newnode->key = key;
		newnode->data = data;
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
		h->noe++;
	}

	return 0;
}

void *aos_hashtab_search(struct aos_hashtab *h, void *key)
{
	u32 hvalue;
	struct aos_hashtab_node *cur;

	aos_assert_r(h, NULL);

	hvalue = h->func(h, key);
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(h, key, cur->key) > 0)
	{
		cur = cur->next;
	}

	if (cur == NULL || (h->keycmp(h, key, cur->key) != 0))
	{
		return NULL;
	}

	return cur->data;
}


void aos_hashtab_destroy(struct aos_hashtab *h)
{
	u32 i;
	struct aos_hashtab_node *cur, *temp;

	aos_assert(h);

	for (i = 0; i < h->size; i++) 
	{
		cur = h->htable[i];
		while (cur != NULL) 
		{
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
					  int (*apply)(void *key, 
						  		   void *data, 
								   void *args),
					  void *args)
{
	u32 i;
	int ret;
	struct aos_hashtab_node *cur;

	aos_assert_r(h, -eAosRc_NullPointer);

	for (i = 0; i < h->size; i++) 
	{
		cur = h->htable[i];
		while (cur != NULL) 
		{
			ret = apply(cur->key, cur->data, args);
			if (ret)
			{
				return ret;
			}
			cur = cur->next;
		}
	}
	return 0;
}


u32 aos_hashtab_map_remove_on_error(struct aos_hashtab *h,
                                 int (*apply)(void *k, void *d, void *args),
                                 void (*destroy)(void *k, void *d, void *args),
                                 void *args)
{
	u32 i;
	int ret;
	u32 count = 0;
	struct aos_hashtab_node *last, *cur, *temp;

	aos_assert_r(h, 0);

	for (i = 0; i < h->size; i++) 
	{
		last = NULL;
		cur = h->htable[i];
		while (cur != NULL) 
		{
			ret = apply(cur->key, cur->data, args);
			if (ret) 
			{
				if (last)
					last->next = cur->next;
				else
					h->htable[i] = cur->next;

				temp = cur;
				cur = cur->next;
				if (destroy)
					destroy(temp->key, temp->data, args);
				aos_free(temp);
				count++;
				h->noe--;
			} 
			else 
			{
				last = cur;
				cur = cur->next;
			}
		}
	}

	return count;
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

