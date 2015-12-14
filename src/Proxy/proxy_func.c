////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_func.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


//#define AOS_DEBUG

//
// All the non-static functions in this file are used by wrappers
//
#include "proxy.h"
#include <net/tcp.h>


int AosProxy_sendFin( struct sock * sk )
{
	aos_tcp_shutdown( sk, SEND_SHUTDOWN );
	return 0;
}


int AosProxy_sockWritable( struct sock * sk )
{
	return ( sk->sk_state == TCP_ESTABLISHED ) 
		&& ( sk->sk_wmem_queued < sk->sk_sndbuf ); 
}


int AosProxy_connect( struct sock ** sk, __u32 ip, __u16 port )
{
	int		err;
	struct sockaddr_in sin;

	err=aos_inet_create( sk, SOCK_STREAM, IPPROTO_TCP );
	if(err<0)
	{
		aos_trace(KERN_EMERG "Proxy allocate inet sk failed\n");
		goto out;

	}
	sock_hold( *sk );
	(*sk)->sk_socket = 0;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ip;
	sin.sin_port = htons(port);

	aos_debug(KERN_INFO "Proxy sock will connect to %u.%u.%u.%u:%d\n",
			NIPQUAD(ip),
			port
			);
	err = tcp_v4_connect( *sk,(struct sockaddr*)&sin,sizeof(sin));
	
	//if( err || TCP_SYN_SENT!=(*sk)->sk_state )
	if( err )
	{
		aos_trace(KERN_INFO "Proxy back sock connect failed\n");
		err = -1;
		goto release_back_sk;
	}

	if( TCP_SYN_SENT!=(*sk)->sk_state )
	{
		proxy_alarm( "sent syn, but the current state is not TCP_SYN_SENT, is %d\n", (*sk)->sk_state );
	}
	aos_debug(KERN_INFO "Proxy send SYN to back success\n");

	return 0;

release_back_sk:
	tcp_close_aos( *sk,0 );
	//sk_free( *sk );
out:
	return err;
}

int
AosProxy_send(struct sock * sk, char * buffer, int len)
{
	struct msghdr 	msg;
	struct iovec	iov;
	mm_segment_t	oldfs;

	memset(&msg,0,sizeof(msg));
	msg.msg_iovlen=1;
	msg.msg_iov=&iov;
	msg.msg_flags=MSG_DONTWAIT;
	iov.iov_len=len;
	iov.iov_base=buffer;
	oldfs=get_fs();
	set_fs(KERNEL_DS);
	//len = sock_sendmsg(sock, &msg, len);
	len = tcp_sendmsg(0,sk, &msg, len);
	set_fs(oldfs);
	return len;
}

EXPORT_SYMBOL( AosProxy_send );
EXPORT_SYMBOL( AosProxy_connect );
EXPORT_SYMBOL( AosProxy_sockWritable  );
EXPORT_SYMBOL( AosProxy_sendFin  );


