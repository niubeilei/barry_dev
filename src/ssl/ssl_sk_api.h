////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ssl_sk_api.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __AOS_SSL_SK_API_H__
#define __AOS_SSL_SK_API_H__

#define SK_SSL_DATA(sk) (sk)->event[EVENT_LAYER_SSL].data
#define SK_SSL_FUNC(sk,num) (sk)->event[EVENT_LAYER_SSL].func[(num)]
#define SK_SSL_OUT_QUEUE(sk) (sk)->event[EVENT_LAYER_SSL].outputq
#define SK_SSL_IN_QUEUE(sk) (sk)->event[EVENT_LAYER_SSL].inputq

extern int ssl_tcp_init(void);
extern int ssl_app_data_rcv(struct sock *sk, struct sk_buff_head *rskbq);

#endif // #ifndef __AOS_SSL_SK_API_H__

