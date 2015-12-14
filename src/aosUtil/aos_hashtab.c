//#include "KernelSimu/kernel.h"
//#include <linux/kernel.h>
//#include <linux/slab.h>

#include "aosUtil/aos_hashtab.h"

#include <linux/errno.h>
#include "aosUtil/Memory.h"

struct aos_hashtab *aos_hashtab_create(u32 (*hash_value)(struct aos_hashtab *h, void *key),
                               int (*keycmp)(struct aos_hashtab *h, void *key1, void *key2),
                               u32 size)
{
	struct aos_hashtab *p;
	u32 i;

	p = (struct aos_hashtab *)aos_malloc(sizeof(*p));
	if (p == NULL)
		return p;

	memset(p, 0, sizeof(*p));
	p->size = size;
	p->nel = 0;
	p->hash_value = hash_value;
	p->keycmp = keycmp;
	p->htable = (struct aos_hashtab_node **)aos_malloc(sizeof(*(p->htable)) * size);
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
	struct aos_hashtab_node *prev, *cur, *newnode;

	if (!h || h->nel == AOS_HASHTAB_MAX_NODES)
		return -EINVAL;

	hvalue = h->hash_value(h, key);
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur && h->keycmp(h, key, cur->key) > 0) {
		prev = cur;
		cur = cur->next;
	}

	if (cur && (h->keycmp(h, key, cur->key) == 0))
		return -EEXIST;

	newnode = (struct aos_hashtab_node *)aos_malloc(sizeof(*newnode));
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

	hvalue = h->hash_value(h, key);
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
	hvalue = h->hash_value(h, key);
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
		newnode = (struct aos_hashtab_node *)aos_malloc(sizeof(*newnode));
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

	hvalue = h->hash_value(h, key);
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(h, key, cur->key) > 0)
		cur = cur->next;

	if (cur == NULL || (h->keycmp(h, key, cur->key) != 0))
		return NULL;

	return cur->datum;
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
