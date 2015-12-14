////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: af_inet.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

//#include "KernelSimu/af_inet.h"

#include <KernelSimu/sock.h>
#include <KernelSimu/net.h>
#include <KernelSimu/errno.h>
#include <KernelSimu/in.h>
#include <KernelSimu/ip.h>
#include <KernelSimu/socket.h>

// #include <netinet/in.h>

#include "KernelUtil/KernelMem.h"


// 
// Create a socket
//
int inet_create(struct socket *sock, int protocol)
{
/*
	struct sock *sk;
//	struct list_head *p;
	struct inet_protosw *answer;
	struct inet_sock *inet;
//	unsigned char answer_flags = 0;
	char answer_no_check = 0;
	int err;

	sock->state = SS_UNCONNECTED;

	// Look for the requested type/protocol pair. 
	answer = NULL;

	// sock->ops = answer->ops;
	sock->ops = 0;

	err = -ENOBUFS;
	sk = (struct sock *)OmnKernelAlloc(sizeof(struct sock));
	if (sk == NULL)
		goto out;

	err = 0;
	sk->sk_prot = 0;
	sk->sk_no_check = answer_no_check;
	sk->sk_reuse = 1;

	inet = (struct inet_sock *)sk;

	if (SOCK_RAW == sock->type) 
	{
		inet->num = protocol;
		if (IPPROTO_RAW == protocol)
			inet->hdrincl = 1;
	}

	inet->pmtudisc = IP_PMTUDISC_DONT;

	inet->id = 0;

	sock_init_data(sock, sk);
//	sk_set_owner(sk, 0);

//	sk->sk_destruct	   = inet_sock_destruct;
	sk->sk_family	   = PF_INET;
	sk->sk_protocol	   = protocol;
//	sk->sk_backlog_rcv = sk->sk_prot->backlog_rcv;

	inet->uc_ttl	= -1;
	inet->mc_loop	= 1;
	inet->mc_ttl	= 1;
	inet->mc_index	= 0;
//	inet->mc_list	= NULL;

	if (inet->num) 
	{
		// It assumes that any protocol which allows
		// the user to assign a number at socket
		// creation time automatically
		// shares.
		//
		inet->sport = inet->num;
	}

out:
	return err;
*/
	return 0;
}

int inet_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	return 0;
}

int inet_listen(struct socket *sock, int backlog)
{
	return 0;
}

