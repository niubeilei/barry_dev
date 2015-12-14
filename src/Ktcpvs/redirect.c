////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: redirect.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
/*
 * KTCPVS       An implementation of the TCP Virtual Server daemon inside
 *              kernel for the LINUX operating system. KTCPVS can be used
 *              to build a moderately scalable and highly available server
 *              based on a cluster of servers, with more flexibility.
 *
 * redirect.c: redirect requests to other server sockets
 *
 * Version:     $Id: redirect.c,v 1.2 2015/01/06 08:57:50 andy Exp $
 *
 * Authors:     Wensong Zhang <wensong@linuxvirtualserver.org>
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/tcp.h>

#include "Ktcpvs/tcp_vs.h"
#include "KernelPorting/Sock.h"

/* Note: most code of redirecting to local socket is taken from TUX */

static void
dummy_destructor(struct open_request *req)
{
}

static struct or_calltable dummy = {
	0,
	NULL,
	NULL,
	&dummy_destructor,
	NULL
};


int
redirect_to_local(struct tcp_vs_conn *conn, __u32 addr, __u16 port)
{
	struct socket *sock;
	struct open_request *tcpreq;
	struct sock *sk, *oldsk;
	int rc = 0;

	sock = conn->csock;

	/* search for local listening user-space socket */
	local_bh_disable();
	sk = tcp_v4_lookup_listener(addr, ntohs(port), 0);
	local_bh_enable();

	/* No socket found */
	if (!sk) {
		TCP_VS_ERR_RL("no server found\n");
		rc = -1;
		goto out;
	}

	oldsk = sock->sk;
	lock_sock(sk);

	if (aosSock_State(sk) != TCP_LISTEN) {
		rc = -1;
		goto out_unlock;
	}

	tcpreq = tcp_openreq_alloc();
	if (!tcpreq) {
		rc = -1;
		goto out_unlock;
	}

	sock->sk = NULL;
	sock->state = SS_UNCONNECTED;

	tcpreq->class = &dummy;
	write_lock_irq(&aosSock_CallbackLock(oldsk));
	aosSock_Socket(oldsk) = NULL;
	aosSock_Sleep(oldsk) = NULL;
	write_unlock_irq(&aosSock_CallbackLock(oldsk));

	tcp_acceptq_queue(sk, tcpreq, oldsk);

	aosSock_Nonagle(sk) = 0;
	aosSock_DataReady(sk)(sk, 0);

      out_unlock:
	release_sock(sk);
	sock_put(sk);
      out:
	return rc;
}
