////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: skbuff.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelSimu/skbuff.h"

#include "aosUtil/Memory.h"
#include <KernelSimu/dst.h>
#include <KernelSimu/kernel.h>
#include <KernelSimu/netdevice.h>
#include <KernelSimu/slab.h>
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/core.h>
#include <KernelSimu/stddef.h>
#include <KernelSimu/spinlock.h>
#include <KernelSimu/errno_base.h>

#include <string.h>


void __kfree_skb(struct sk_buff *skb)
{
	if (skb->list) 
	{
		aosAlarmStr(eAosAlarm, "Warning: kfree_skb passed an skb still "
				"on a list.\n");
	}

	dst_release(skb->dst);
	if(skb->destructor) 
	{
		skb->destructor(skb);
	}

	delete skb;
}


void skb_under_panic(struct sk_buff *skb, int sz, void *here)
{
	printk(KERN_INFO "skput:under: %p:%d put:%d dev:%s",
			here, skb->len, sz, skb->dev ? skb->dev->name : "<NULL>");
	aosAlarm(eAosAlarm);
}


struct sk_buff *skb_clone(struct sk_buff *skb, int gfp_mask)
{
	struct sk_buff *n = (sk_buff*)aos_malloc(2000);

	if (!n) 
	{
		aosAlarm(eAosAlarmMemErr);
		return NULL;
	}

#define C(x) n->x = skb->x

	n->next = n->prev = NULL;
	n->list = NULL;
	n->sk = NULL;
	//C(stamp);
	C(dev);
	C(real_dev);
	C(h);
	C(nh);
	C(mac);
	C(dst);
	dst_clone(skb->dst);
	C(sp);
	memcpy(n->cb, skb->cb, sizeof(skb->cb));
	C(len);
	C(data_len);
	C(csum);
	C(local_df);
	n->cloned = 1;
	C(pkt_type);
	C(ip_summed);
	C(priority);
	C(protocol);
	C(security);
	n->destructor = NULL;
	C(truesize);
	n->users = 1;
	C(head);
	C(data);
	C(tail);
	C(end);

	OmnMutexLock(skb_shinfo(skb)->datarefLock);
	(skb_shinfo(skb)->dataref)++;
	OmnMutexUnlock(skb_shinfo(skb)->datarefLock);
	skb->cloned = 1;

	return n;
}


struct sk_buff *alloc_skb(unsigned int size, int gfp_mask)
{
	struct sk_buff *skb;
	unsigned char *data;

	skb = (struct sk_buff *)kmalloc(sizeof(struct sk_buff), gfp_mask);
	if (!skb)
		goto out;

	/* Get the DATA. Size must match skb_add_mtu(). */
	// size = SKB_DATA_ALIGN(size);
	data = (unsigned char *)kmalloc(size + sizeof(struct skb_shared_info), gfp_mask);
	if (!data)
		goto nodata;

	memset(data, 0, size+sizeof(struct skb_shared_info));
	memset(skb, 0, offsetof(struct sk_buff, truesize));
	skb->truesize = size + sizeof(struct sk_buff);
	//aosKernelLock(skb->usersLock);
	skb->users = 1;
	//aosKernelUnlock(skb->usersLock);
	skb->head = data;
	skb->data = data;
	skb->tail = data;
	skb->end  = data + size;

	//aosKernelLock(skb_shinfo(skb)->datarefLock);
	skb_shinfo(skb)->dataref = 1;
	//aosKernelUnlock(skb_shinfo(skb)->datarefLock);
	skb_shinfo(skb)->nr_frags  = 0;
	skb_shinfo(skb)->tso_size = 0;
	skb_shinfo(skb)->tso_segs = 0;
	skb_shinfo(skb)->frag_list = NULL;
out:
	return skb;
nodata:
	aos_free(skb);
	skb = NULL;
	goto out;
}


unsigned int skb_checksum(const struct sk_buff *skb, int offset,
		int len, unsigned int csum)
{
	return 0;
}


void skb_release_data(struct sk_buff *skb)
{
	if (!skb->cloned || --(skb_shinfo(skb)->dataref) == 0) 
	{
		/*
		   if (skb_shinfo(skb)->nr_frags) 
		   {
		   int i;
		   for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
		   {
		   put_page(skb_shinfo(skb)->frags[i].page);
		   }
		   }

		   if (skb_shinfo(skb)->frag_list)
		   {
		   skb_drop_fraglist(skb);
		   }
		   */

		aos_free(skb->head);
	}
}

struct sk_buff *skb_dequeue(struct sk_buff_head *list)
{
	unsigned long flags;
	struct sk_buff *result;

	spin_lock_irqsave(&list->lock, flags);
	result = __skb_dequeue(list);
	spin_unlock_irqrestore(&list->lock, flags);
	return result;
}

void skb_queue_head(struct sk_buff_head *list, struct sk_buff *newsk)
{
	unsigned long flags;

	spin_lock_irqsave(&list->lock, flags);
	__skb_queue_head(list, newsk);
	spin_unlock_irqrestore(&list->lock, flags);
}

void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk)
{
	unsigned long flags;

	spin_lock_irqsave(&list->lock, flags);
	__skb_queue_tail(list, newsk);
	spin_unlock_irqrestore(&list->lock, flags);
}

/**
 *  skb_queue_purge - empty a list
 *  @list: list to empty
 * 
 *  Delete all buffers on an &sk_buff list. Each buffer is removed from
 *  the list and one reference dropped. This function takes the list
 *  lock and is atomic with respect to other list locking functions.
 **/
void skb_queue_purge(struct sk_buff_head *list)
{
	struct sk_buff *skb;
	while ((skb = skb_dequeue(list)) != NULL)
		kfree_skb(skb);
}
/* Copy some data bits from skb to kernel buffer. */

int skb_copy_bits(const struct sk_buff *skb, int offset, void *to, int len)
{
	int i, copy;
	int start = skb_headlen(skb);

	if (offset > (int)skb->len - len)
		goto fault;

	/* Copy header. */
	if ((copy = start - offset) > 0) {
		if (copy > len)
			copy = len;
		memcpy(to, skb->data + offset, copy);
		if ((len -= copy) == 0)
			return 0;
		offset += copy;
		// TBD 
		//(unsigned char*)to     += copy;
	}

	for (i = 0; i < (int)skb_shinfo(skb)->nr_frags; i++) {
		int end;

		//BUG_TRAP(start <= offset + len);

		end = start + skb_shinfo(skb)->frags[i].size;
		if ((copy = end - offset) > 0) {
			unsigned char *vaddr;

			if (copy > len)
				copy = len;

			// TBD 
			// vaddr = kmap_skb_frag(&skb_shinfo(skb)->frags[i]);
			memcpy(to,
					vaddr + skb_shinfo(skb)->frags[i].page_offset+
					offset - start, copy);
			// TBD
			// kunmap_skb_frag(vaddr);

			if ((len -= copy) == 0)
				return 0;
			offset += copy;
			// TBD 
			// (unsigned char*)to     += copy;
		}
		start = end;
	}

	if (skb_shinfo(skb)->frag_list) {
		struct sk_buff *list = skb_shinfo(skb)->frag_list;

		for (; list; list = list->next) {
			int end;

			//BUG_TRAP(start <= offset + len);

			end = start + list->len;
			if ((copy = end - offset) > 0) {
				if (copy > len)
					copy = len;
				if (skb_copy_bits(list, offset - start,
							to, copy))
					goto fault;
				if ((len -= copy) == 0)
					return 0;
				offset += copy;
				// TBD
				//(unsigned char*)to     += copy;
			}
			start = end;
		}
	}
	if (!len)
		return 0;

fault:
	return -EFAULT;
}

struct sk_buff *skb_copy_expand(const struct sk_buff *skb,
		int newheadroom, int newtailroom,
		gfp_t gfp_mask)
{
	/*
	 * 	 *	Allocate the copy buffer
	 * 	 	 */
	struct sk_buff *n = alloc_skb(newheadroom + skb->len + newtailroom,
			gfp_mask);
	int head_copy_len, head_copy_off;

	if (!n)
		return NULL;

	skb_reserve(n, newheadroom);

	/* Set the tail pointer and length */
	skb_put(n, skb->len);

	head_copy_len = skb_headroom(skb);
	head_copy_off = 0;
	if (newheadroom <= head_copy_len)
		head_copy_len = newheadroom;
	else
		head_copy_off = newheadroom - head_copy_len;

	/* Copy the linear header and data. */
	if (skb_copy_bits(skb, -head_copy_len, n->head + head_copy_off,
				skb->len + head_copy_len))
		;//BUG();

	//copy_skb_header(n, skb);

	return n;
}
static inline void skb_split_inside_header(struct sk_buff *skb,
		struct sk_buff* skb1,
		const u32 len, const int pos)
{
	int i;

	memcpy(skb_put(skb1, pos - len), skb->data + len, pos - len);

	/* And move data appendix as is. */
	for (i = 0; i < (int)skb_shinfo(skb)->nr_frags; i++)
		skb_shinfo(skb1)->frags[i] = skb_shinfo(skb)->frags[i];

	skb_shinfo(skb1)->nr_frags = skb_shinfo(skb)->nr_frags;
	skb_shinfo(skb)->nr_frags  = 0;
	skb1->data_len		   = skb->data_len;
	skb1->len		   += skb1->data_len;
	skb->data_len		   = 0;
	skb->len		   = len;
	skb->tail		   = skb->data + len;
}

static inline void skb_split_no_header(struct sk_buff *skb,
		struct sk_buff* skb1,
		const u32 len, int pos)
{
	int i, k = 0;
	const int nfrags = skb_shinfo(skb)->nr_frags;

	skb_shinfo(skb)->nr_frags = 0;
	skb1->len		  = skb1->data_len = skb->len - len;
	skb->len		  = len;
	skb->data_len		  = len - pos;

	for (i = 0; i < nfrags; i++) {
		int size = skb_shinfo(skb)->frags[i].size;

		if ((int)(pos + size) > (int)len) {
			skb_shinfo(skb1)->frags[k] = skb_shinfo(skb)->frags[i];

			if ((int)pos < (int)len) {
				/* Split frag.
				 * We have two variants in this case:
				 * 1. Move all the frag to the second
				 *    part, if it is possible. F.e.
				 *    this approach is mandatory for TUX,
				 *    where splitting is expensive.
				 * 2. Split is accurately. We make this.
				 */
				//get_page(skb_shinfo(skb)->frags[i].page);
				skb_shinfo(skb1)->frags[0].page_offset += len - pos;
				skb_shinfo(skb1)->frags[0].size -= len - pos;
				skb_shinfo(skb)->frags[i].size	= len - pos;
				skb_shinfo(skb)->nr_frags++;
			}
			k++;
		} else
			skb_shinfo(skb)->nr_frags++;
		pos += size;
	}
	skb_shinfo(skb1)->nr_frags = k;
}
/**
 * skb_split - Split fragmented skb to two parts at length len.
 * @skb: the buffer to split
 * @skb1: the buffer to receive the second part
 * @len: new length for skb
 **/
void skb_split(struct sk_buff *skb, struct sk_buff *skb1, const u32 len)
{
	int pos = skb_headlen(skb);

	if ((int)len < pos)	/* Split line is inside header. */
		skb_split_inside_header(skb, skb1, len, pos);
	else		/* Second chunk has no header, nothing to copy. */
		skb_split_no_header(skb, skb1, len, pos);
}

