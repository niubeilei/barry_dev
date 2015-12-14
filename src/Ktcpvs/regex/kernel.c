////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: kernel.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
/*
 * The wrapper for allocating/releasing dynamic memory inside the kernel
 *
 * Version:     $Id: kernel.c,v 1.2 2015/01/06 08:57:50 andy Exp $
 *
 * Authors:	Wensong Zhang <wensong@linux-vs.org>
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>                 /* for list_head */
#include <linux/slab.h>


typedef struct memblock_s
{
	struct list_head	list;
	void			*ptr;
	size_t			size;
} memblock_t;


/*
 * The list of memory blocks (one simple list)
 */
static LIST_HEAD(memblock_list);
static rwlock_t memblock_lock = RW_LOCK_UNLOCKED;

static int memblock_add(void *ptr, size_t size)
{
	memblock_t *m;

	if (!(m = kmalloc(sizeof(memblock_t), GFP_ATOMIC)))
		return -ENOMEM;
	m->ptr = ptr;
	m->size = size;

	write_lock(&memblock_lock);
	list_add(&m->list, &memblock_list);
	write_unlock(&memblock_lock);

	return 0;
}

static void memblock_del(memblock_t *m)
{
	write_lock(&memblock_lock);
	list_del(&m->list);
	write_unlock(&memblock_lock);
	kfree(m);
}

static memblock_t *memblock_get(void *ptr)
{
	struct list_head *l;
	memblock_t *m;

	read_lock(&memblock_lock);
	list_for_each(l, &memblock_list) {
		m = list_entry(l, memblock_t, list);
		if (m->ptr == ptr) {
			/* HIT */
			read_unlock(&memblock_lock);
			return m;
		}
	}
	read_unlock(&memblock_lock);
	return NULL;
}


void *malloc(size_t size)
{
	void *ptr;

	if (size == 0)
		return NULL;

	if (!(ptr = kmalloc(size, GFP_ATOMIC)))
		return NULL;
	if (memblock_add(ptr, size)) {
		kfree(ptr);
		return NULL;
	}
	return ptr;
}

void free(void *ptr)
{
	memblock_t *m;

	if (!ptr)
		return;
	if (!(m = memblock_get(ptr))) {
		printk(KERN_ERR "bug: free non-exist memory\n");
		return;
	}
	memblock_del(m);
	kfree(ptr);
}

void *realloc(void *ptr, size_t size)
{
	memblock_t *m;
	void *new = NULL;

	if (ptr) {
		if (!(m = memblock_get(ptr))) {
			printk(KERN_ERR "bug: realloc non-exist memory\n");
			return NULL;
		}

		if (size == m->size)
			return ptr;
		if (size != 0) {
			if (!(new = kmalloc(size, GFP_ATOMIC)))
				return NULL;
			memmove(new, ptr, min(size, m->size));
			if (memblock_add(new, size)) {
				kfree(new);
				return NULL;
			}
		}

		memblock_del(m);
		kfree(ptr);
	} else {
		if (size != 0) {
			if (!(new = kmalloc(size, GFP_ATOMIC)))
				return NULL;
			if (memblock_add(new, size)) {
				kfree(new);
				return NULL;
			}
		}
	}

	return new;
}
