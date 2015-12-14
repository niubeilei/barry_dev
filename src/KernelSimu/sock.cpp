////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sock.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelSimu/sock.h"

#include <KernelSimu/skbuff.h>
#include <KernelSimu/tcp.h>
#include <KernelSimu/socket.h>
#include <KernelSimu/net.h>


void sock_init_data(struct socket *sock, struct sock *sk)
{
//	skb_queue_head_init(&sk->sk_receive_queue);
//	skb_queue_head_init(&sk->sk_write_queue);
//	skb_queue_head_init(&sk->sk_error_queue);

//	sk->sk_send_head	=	NULL;
	
	sk->sk_allocation	=	GFP_KERNEL;
//	sk->sk_rcvbuf		=	sysctl_rmem_default;
//	sk->sk_sndbuf		=	sysctl_wmem_default;
	sk->sk_state		=	TCP_CLOSE;
	sk->sk_zapped		=	1;
	sk->sk_socket		=	sock;

	if(sock)
	{
		sk->sk_type		=	sock->type;
//		sk->sk_sleep	=	&sock->wait;
		sock->sk		=	sk;
	} else
//		sk->sk_sleep	=	NULL;

//	sk->sk_state_change	=	sock_def_wakeup;
//	sk->sk_data_ready	=	sock_def_readable;
//	sk->sk_write_space	=	sock_def_write_space;
//	sk->sk_error_report	=	sock_def_error_report;
//	sk->sk_destruct		=	sock_def_destruct;

//	sk->sk_sndmsg_page	=	NULL;
	sk->sk_sndmsg_off	=	0;

//	sk->sk_peercred.pid 	=	0;
//	sk->sk_peercred.uid	=	-1;
//	sk->sk_peercred.gid	=	-1;
	sk->sk_write_pending	=	0;
	sk->sk_rcvlowat		=	1;
//	sk->sk_rcvtimeo		=	MAX_SCHEDULE_TIMEOUT;
//	sk->sk_sndtimeo		=	MAX_SCHEDULE_TIMEOUT;
//	sk->sk_owner		=	NULL;

//	sk->sk_stamp.tv_sec     = -1L;
//	sk->sk_stamp.tv_usec    = -1L;
}

