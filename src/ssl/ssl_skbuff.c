////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ssl_skbuff.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/skbuff.h"
#include "KernelSimu/gfp.h"
#include "KernelSimu/cache.h"

static inline struct sk_buff*  __aos_skb_cat(struct sk_buff *skb1, struct sk_buff *skb2, int gfp_mask)
{
	struct sk_buff *skb;

	skb = skb_copy_expand(skb1, skb_headroom(skb1), skb2->len, gfp_mask);
	if (!skb) return NULL;
	
	memcpy(__skb_put(skb, skb2->len), skb2->data, skb2->len);	
	return skb;
}

struct sk_buff *__aos_skb_expand(struct sk_buff *skb, int extend_head, int extend_tail, int gfp_mask)
{
	struct sk_buff *newskb;
	
	if ((skb_headroom(skb) > extend_head) && (skb_tailroom(skb) > extend_tail))
		return skb;
	newskb = skb_copy_expand(skb, skb_headroom(skb)+extend_head, extend_tail, gfp_mask);
	kfree_skb(skb);
	
	return newskb;
}

int __aos_skb_cat2(struct sk_buff *skb, struct sk_buff *skb2, int gfp_mask)
{
	int oldsize, newsize, data_len;
	int data_offset;
	u8 *buf, *oldhead;

	aos_assert1(skb_shared(skb) == 1);

	// all the data in skb/skb2 should be in a contiues buffer
	aos_assert1(skb->data_len == 0);
	aos_assert1(skb2->data_len == 0);

	if (skb_tailroom(skb) >= (int)skb2->len)
	{
		memcpy(__skb_put(skb, skb2->len), skb2->data, skb2->len);	
		return 0;
	}	

	data_len = skb->len + skb2->len;
	data_offset = skb->data - skb->head;

	oldsize = skb->end - skb->head; 
	newsize = oldsize + skb2->len;
	newsize = SKB_DATA_ALIGN(newsize);

	if (gfp_mask == GFP_ATOMIC)
	{
		buf = (u8*)aos_malloc_atomic(newsize + sizeof(struct skb_shared_info));
	}
	else
	{
		buf = (u8*)aos_malloc(newsize + sizeof(struct skb_shared_info));
	}

	if (!buf)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "failed to malloc skb buffer");
		return -1;
	}

	oldhead = skb->head;

	skb->truesize = newsize + sizeof(struct sk_buff);
	skb->head = buf;
	skb->data = buf+data_offset;
	skb->tail = buf+data_len;
	skb->end  = buf+newsize;

	memcpy(skb->head, oldhead, oldsize);
	memcpy(skb->head+oldsize, skb2->data, skb2->len);

	aos_free(oldhead);

	atomic_set(&(skb_shinfo(skb)->dataref), 1);
	skb_shinfo(skb)->nr_frags  = 0;
	skb_shinfo(skb)->tso_size = 0;
	skb_shinfo(skb)->tso_segs = 0;
	skb_shinfo(skb)->frag_list = NULL;
	return 0;

}
////////////////////////////////////////////////////////////////////////////
//
// 1. if the size of first skb in list is larger than size, then return size
// 2. if the size of first skb in list is little than size, then merge following skb 
//    skb in list to first skb until the size of first skb is larger than size
//    
int __aos_skbq_len_merge(struct sk_buff_head *list, int size)
{
	struct sk_buff *skb, *newskb, *skb2, *skb3;

	skb = skb_peek(list);
	if (!skb) return 0;

	if ((int)skb->len >= size) return size;

	if (skb_queue_len(list) <= 1) return skb->len;

	aos_trace("%s first skb is not large enough, size %d, skb->len %d", __FUNCTION__, size, skb->len);

	// there are at least two skb in queue
	skb = __skb_dequeue(list);
	while((skb2 = __skb_dequeue(list)))
	{
		if (int(skb->len+skb2->len) <= size)
		{
			newskb = __aos_skb_cat(skb, skb2, GFP_ATOMIC);
			if (!newskb)
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "failed to cat two skb");
				return -1;
			}
			kfree_skb(skb);
			kfree_skb(skb2);
			skb = newskb;
		}
		else
		{
			skb_split(skb2, skb3, size - skb->len);
			newskb = __aos_skb_cat(skb, skb2, GFP_ATOMIC);
			if (!newskb)
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "failed to cat two skb");
				return -1;
			}
			kfree_skb(skb);
			kfree_skb(skb2);
			skb = newskb;

			__skb_queue_head(list, skb3);
			break;
		}
	}

	__skb_queue_head(list, skb);

	return skb->len;
}

u8* __aos_skbq_data(struct sk_buff_head *list, int size)
{
	struct sk_buff *skb;
	int len;
	
	skb = skb_peek(list);
	if (!skb) return NULL;

	if ((int)skb->len >= size) return skb->data;

	len = __aos_skbq_len_merge(list, size);
	if (len < size) return NULL;

	skb = skb_peek(list); 
	if (skb) return skb->data;
	
	return NULL;
}

int __aos_skbq_len(struct sk_buff_head *queue)
{
	struct sk_buff *skb;
	int len = 0;
	if(queue == NULL) return -1;
	skb = skb_peek(queue);
	do
	{
		if (!skb) break;
		len += skb->len;
		skb = skb->next;
	} while (skb != (struct sk_buff *)queue);
	return len;
}

u8* __aos_skbq_getbuf(struct sk_buff_head *list, int size)
{
	struct sk_buff *skb;
	int len;
	if (size == 0)
		len = __aos_skbq_len(list);
	else
		len = size;
	__aos_skbq_len_merge(list, len);
	skb = skb_peek(list);

	if (skb) return skb->data;
	return NULL;
}
