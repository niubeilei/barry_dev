////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ssl_skbuff.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __AOS_SSL_SK_BUFF_H__
#define __AOS_SSL_SK_BUFF_H__

extern int __aos_skbq_len_merge(struct sk_buff_head *list, int size);
extern u8* __aos_skbq_data(struct sk_buff_head *list, int size);
extern int __aos_skbq_len(struct sk_buff_head *queue);
extern u8* __aos_skbq_getbuf(struct sk_buff_head *list, int size);

extern int skbq_len(struct sk_buff_head *queue);
extern struct sk_buff_head *skbq_data2(struct sk_buff_head *queue,int len);
extern int skbq_append(struct sk_buff_head *queue1,struct sk_buff *old,struct sk_buff_head *queue2);
extern int skbq_clear(struct sk_buff_head *queue,int len);	
extern struct sk_buff *skb_dequeue(struct sk_buff_head *list);
extern struct sk_buff *__aos_skb_expand(struct sk_buff *skb, int extend_head, int extend_tail, int gfp_mask);
#endif // #ifndef __AOS_SSL_SK_BUFF_H__

