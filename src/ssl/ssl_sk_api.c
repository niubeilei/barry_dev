////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ssl_sk_api.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "ssl/ssl_sk_api.h"
#include "ssl/SslWrapper.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"

#include "ssl/SslWrapper.h"
#include "ssl/ssl_skbuff.h"

#ifndef __KERNEL__
#include <stdlib.h>
#endif

#include "ssl/Tester/vpn/app_type.h"
#include "ssl/Tester/vpn/vossk.h"

static char *msg_addr="any";
static char *msg_port="7000";

static struct sk_buff *app_recv(struct sock *sk, int type)
{
	struct sk_buff *skb;

	if(sk==NULL)
		return(NULL);

	if(app_func_run(type,sk,FUNC_TYPE_RECV,NULL,(void **)&skb)==0)
	{
		return(skb);
	}
	return(NULL);
}		

static int app_send(struct sock *sk,struct sk_buff *skb, int type)
{
	if(sk==NULL||skb==NULL)
		return(-1);
	return(app_func_run(type,sk,FUNC_TYPE_SEND,skb,NULL));
}

////////////////////////////////////////////////////////////////////////////
// ssl skb function API, FUNC_TYPE_OPEN 
// 
int ssl_tcp_open(struct sock *sk,void *input,void **output)
{
	int ret;
	struct app_type *apptype;
	struct sock *sknode;
	struct argstr
	{
		int servflag;
		unsigned int addr;
		unsigned short port;
	}arg,*parg;

	apptype = app_type_search(APP_TYPE_SSL);
	if (!apptype)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "app_type_search error");
		return -1;
	}

	if (input)
	{
		parg = (struct argstr *)input;
	}
	else
	{
		parg = &arg;
		parg->servflag = 1;
		parg->addr = sk_inet_addr(msg_addr);
		parg->port = simple_strtoul(msg_port,NULL,10);
	}

	ret = app_event_open(APP_TYPE_TCP, (void *)parg, (void **)&sknode);
	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "app_event_open error");
		return -1;
	}

	sknode->event[EVENT_LAYER_SSL].app = apptype;
	if(output)
		*output = sknode;

	return 0;
}

////////////////////////////////////////////////////////////////////////////
// ssl skb function API, FUNC_TYPE_CLOSE 
//
static int ssl_tcp_close(struct sock *sk, void *input, void **ouput)
{
	aos_trace("%s", __FUNCTION__);
	if (!sk)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "ssl_tcp_close sk is null");
		return -1;
	}
	AosSslWrapper_release(SK_DATA(sk, EVENT_LAYER_SSL));
	SK_DATA(sk, EVENT_LAYER_SSL) = NULL;
	return app_func_run(EVENT_LAYER_SSL, sk, FUNC_TYPE_CLOSE, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////
// ssl skb event function API, FUNC_TYPE_CLOSE_READY 
//
static int ssl_tcp_close_ready(struct sock *sk,void *input,void **output)
{
	aos_trace("%s", __FUNCTION__);
	if (!sk)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "ssl_tcp_close_ready sk is null");
		return -1;
	}
	return ssl_tcp_close(sk, input, output);
}

////////////////////////////////////////////////////////////////////////////
// ssl skb function API, FUNC_TYPE_ACCEPT 
// 
int ssl_tcp_accept(struct sock *sk, void *input, void **output)
{
	struct sock *newsk;
	AosSslWrapperObject_t *swo;
	int ret;

	aos_assert1(sk);

	ret = app_func_run(EVENT_LAYER_SSL, sk, FUNC_TYPE_ACCEPT, NULL,(void **)&newsk);
	if (ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "app_func_run EVENT_LAYER_SSL faild %d", ret);
		return -1;
	}

	swo = (AosSslWrapperObject_t*)aos_malloc(sizeof(AosSslWrapperObject_t));
	if (!swo)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "faild to malloc AosSslWrapperObject");
		goto failed;
	}

	memset(swo, 0, sizeof(AosSslWrapperObject_t));


	// start SSL server
	ret = AosSslWrapper_serverInit(swo);
	if(ret) goto failed;

	swo->sk = newsk;
	SK_DATA(newsk, EVENT_LAYER_SSL) = (void*)swo;

	if (output)
		*output = newsk;
	return 0;	
failed:
	app_func_run(EVENT_LAYER_SSL, newsk, FUNC_TYPE_CLOSE, NULL, NULL);
	return -1;
}

int ssl_tcp_recv_ready(struct sock *sk,void *input,void **output)
{
	struct sk_buff *skb;
	
	aos_trace("%s", __FUNCTION__);

	aos_assert1(sk);

	skb = app_recv(sk, EVENT_LAYER_SSL);
	if (!skb) 
	{
		aos_trace("no skb received");	
		return 0;
	}
	aos_trace("%s before wrapper rcv", __FUNCTION__);	
	AosSslWrapper_rcv(SK_SSL_DATA(sk), skb); 

	return 1;
}

int ssl_app_data_rcv(struct sock *sk, struct sk_buff_head *rskbq)
{
	aos_trace("%s", __FUNCTION__);
	skbq_append(&(SK_SSL_IN_QUEUE(sk)), NULL, rskbq);
	app_event_run(EVENT_LAYER_SSL, sk, EVENT_TYPE_RECV_READY);
	return 0;
}

int ssl_tcp_recv(struct sock *sk,void *input,void **output)
{
	struct sk_buff_head *rskbq;
	struct sk_buff *skb;
	
	aos_trace("%s", __FUNCTION__);
	rskbq = &SK_SSL_IN_QUEUE(sk);

	aos_assert1(rskbq);
	skb = skb_dequeue(rskbq);

	*output = skb;

	return 0;
}

int ssl_tcp_send(struct sock *sk,void *input,void **output)
{
	int ret;
	struct sk_buff *skb;

	aos_trace("%s", __FUNCTION__);

	aos_assert1(sk);
	aos_assert1(input);
	
	skb = (struct sk_buff*)input;
	ret = AosSslWrapper_sendAppData((struct AosSslWrapperObject*)(SK_SSL_DATA(sk)), skb);
	if (ret == (int)skb->len)
		return 0;
	return -1;
}

struct app_type app_type_ssl=
{
	NULL,
	NULL,
	APP_TYPE_SSL,
	"SSL",
	NULL
};

int ssl_tcp_init(void)
{
	app_type_ssl.func[FUNC_TYPE_OPEN] = ssl_tcp_open;
	app_type_ssl.func[FUNC_TYPE_ACCEPT] = ssl_tcp_accept;
	app_type_ssl.func[EVENT_TYPE_RECV_READY] = ssl_tcp_recv_ready;
	app_type_ssl.func[FUNC_TYPE_CLOSE]= ssl_tcp_close;
	app_type_ssl.func[FUNC_TYPE_RECV]= ssl_tcp_recv;
	app_type_ssl.func[FUNC_TYPE_SEND]= ssl_tcp_send;
	//app_type_ssl.func[EVENT_TYPE_CLOSE_READY] = ssl_tcp_close_ready;
	//app_type_ssl.func[EVENT_TYPE_SEND_READY] = ssl_tcp_send_ready;
	app_type_register(&app_type_ssl);
	return(0);
}
