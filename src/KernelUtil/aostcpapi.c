////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aostcpapi.c
// Description:
//	These APIs provide the way to make tcp connection, 
//  receive and send data through tcp in kernel.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/net.h>
#include <linux/socket.h>
#include <linux/rwsem.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/skbuff.h>
#include <linux/smp_lock.h>	//* for lock_kernel() 
#include <net/tcp.h>
#include <linux/wait.h>     // for struct wait_queue 
#include <asm/atomic.h>
#include <linux/kernel.h>   // NIPQUAD

#include "KernelPorting/Sock.h"
#include "KernelPorting/Daemonize.h"
#include "KernelPorting/Current.h"
#include "aos/aosReturnCode.h"
#include "KernelUtil/aostcpapi.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"

static struct list_head tcp_conn_list = LIST_HEAD_INIT(tcp_conn_list);
static struct list_head tcp_do_conn_list = LIST_HEAD_INIT(tcp_do_conn_list);
static struct list_head tcp_do_rcv_list = LIST_HEAD_INIT(tcp_do_rcv_list);
static struct list_head tcp_err_list = LIST_HEAD_INIT(tcp_err_list); 		// used for error callback
static struct list_head tcp_wait_list = LIST_HEAD_INIT(tcp_wait_list); 		// waitting on conndata will be added into list
DECLARE_RWSEM(tcp_conn_list_sem);
static int start_tcpapi = 0; 
static struct aos_tcpapi_statistics tcpapi_stats = {0,0,0,0,0,0};


struct aos_tcpapi_data*
conndata_put(struct aos_tcpapi_data *conndata)
{
	if(!atomic_dec_and_test(&conndata->refcnt))
	{
		return conndata;
	}
	if (conndata->sock)
	{ 
		sock_release(conndata->sock);
	}
	kfree(conndata);
	return NULL;
}

struct aos_tcpapi_data*
conndata_get(struct aos_tcpapi_data *conndata)
{
	atomic_inc(&conndata->refcnt);
	return conndata;
}

int aos_tcp_client_create( 
	uint32_t remote_addr,
	uint16_t remote_port,
	struct aos_tcpapi_data **pconndata,
	void *userdata,
	int timeout,
	aos_tcpapi_callback rcvcb)
{
	struct aos_tcpapi_data *conndata = NULL;
	aos_debug_log(eAosMD_Platform, "aos tcp api: aos_tcp_client_create entry, remoteaddr = 0x%02x remote_port = %u userdata = 0x%x timeout = %d", remote_addr, remote_port, userdata, timeout);
	if (!rcvcb) 
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("aos tcp api: rcvcb is null"));
		return eAosRc_Failed;
	}
	if (!pconndata)
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("aos tcp api: pconndata is null"));
		return eAosRc_Failed;
	}
	conndata = (struct aos_tcpapi_data*)kmalloc(sizeof(struct aos_tcpapi_data), GFP_KERNEL);
	if (!conndata)
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("aos tcp api: malloc aos_tcpapi_data failed"));
		return eAosRc_Nomem;
	}
	
	memset(conndata, 0, sizeof(struct aos_tcpapi_data));
	conndata->remote_addr = remote_addr;
	conndata->remote_port = htons(remote_port);
	conndata->rcvcb = rcvcb;
	conndata->userdata = userdata;
	conndata->timeout = timeout*HZ;
	atomic_set(&conndata->refcnt, 1);

	init_waitqueue_entry(&conndata->wait, current);
aos_trace("aos tcp create conndata 0x%p", conndata);
	down_write(&tcp_conn_list_sem);

	list_add_tail(&conndata->list, &tcp_conn_list);

	up_write(&tcp_conn_list_sem);
	
	*pconndata = conndata;
	return eAosRc_Success; 
}

int aos_tcp_close(struct aos_tcpapi_data *conndata)
{
	struct aos_tcpapi_data *conndata_this, *conndata_next;
	aos_debug_log(eAosMD_Platform, "aos tcp close conndata is 0x%p", conndata);
	if (!conndata) 
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("aos tcp api: aos_tcp_close, conndata is invalid"));
		return eAosRc_InvalidSocket;
	}

	down_write(&tcp_conn_list_sem);

	list_for_each_entry_safe(conndata_this, conndata_next, &tcp_conn_list, list) 
	{
		if ( conndata_this == conndata ) 
		{
			list_del(&conndata_this->list);
			conndata_put(conndata);
			up_write(&tcp_conn_list_sem);
			return eAosRc_Success;
		}
	}
	if(conndata)
		conndata_put(conndata);

	up_write(&tcp_conn_list_sem);

	return eAosRc_InvalidSocket;
}

int aos_tcp_send_data(struct aos_tcpapi_data *conndata, char *data, int data_len)
{
	struct msghdr msg;
	mm_segment_t oldfs;
	struct iovec iov;
	int len;

	aos_debug_log(eAosMD_Platform, "aos tcp send data, conndata 0x%p, data=0x%p, data_len=%d", conndata, data, data_len);

	if (!conndata) 
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("aos tcp api: aos_tcp_send_data, conndata is invalid"));
		return -eAosRc_InvalidSocket;
	}
	if (!data) 
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("aos tcp api: aos_tcp_send_data, data is null"));
		return -eAosRc_InvalidData;
	}
	if (data_len < 0 || data_len > 65535 ) 
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, "aos tcp api: aos_tcp_send_data, data length error, lenght is %d\n", data_len);
		return -eAosRc_InvalidDataLen;
	}

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	//msg.msg_flags = MSG_NOSIGNAL;
	msg.msg_flags = 0;
	msg.msg_iov->iov_base = (void *) data;
	msg.msg_iov->iov_len = data_len;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	conndata->sock->sk->sk_sndtimeo = tcpapi_send_timeout;
	len = sock_sendmsg(conndata->sock, &msg, data_len);
	//aos_debug_log(eAosMD_Platform, "sock send msg len is %d, conndata->sock %x", len, conndata->sock);
	set_fs(oldfs);
		
	if (len <= 0) 
	{
		aos_alarm(eAosMD_Platform, eAosAlarm_ProgErr,"aos_tcp_send_data failed len %d", len);
		return -eAosRc_FailedToSend;
	}
	conndata->lastupdated = jiffies;
	return len;
}


static int aos_tcp_recvbuffer(struct socket *sock, char *buffer,
		  const size_t buflen, unsigned long flags)
{
	struct msghdr msg;
	struct iovec iov;
	int len;
	mm_segment_t oldfs;


	/* Receive a packet */
	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;
	//msg.msg_flags = MSG_WAITALL;

	iov.iov_base = buffer;
	iov.iov_len = (size_t) buflen;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	//sock->sk->sk_rcvtimeo= 1;
	len = sock_recvmsg(sock, &msg, buflen, flags|msg.msg_flags);
	set_fs(oldfs);

	return len;
}

int aos_tcp_sendbuffer(struct socket *sock, const char *buffer, const size_t length)
{
	struct msghdr msg;
	mm_segment_t oldfs;
	struct iovec iov;
	int len;

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	//msg.msg_flags = MSG_CONFIRM | MSG_DONTWAIT;//MSG_NOSIGNAL;
	msg.msg_flags = 0;
	//msg.msg_flags = MSG_NOSIGNAL;
	msg.msg_iov->iov_base = (void *) buffer;
	msg.msg_iov->iov_len = length;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	sock->sk->sk_sndtimeo = tcpapi_send_timeout;
	len = sock_sendmsg(sock, &msg, length);
	set_fs(oldfs);

	return len;
}

int do_real_connect(struct aos_tcpapi_data *conndata)
{
	struct sockaddr_in sin;
	int error;
	
	error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &conndata->sock);
	if (error < 0) 
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("aos tcp api: sock create failed"));
		conndata->rcvcb(conndata, NULL, 0, conndata->userdata, eAosRc_ConnectFailed);
		return eAosRc_ConnectFailed;
	}
	
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = conndata->remote_addr;
	sin.sin_port = conndata->remote_port;

	aos_debug_log(eAosMD_Platform, "do real connect, conndata 0x%p, conndata->sock 0x%p", conndata, conndata->sock);	
/*
	error = security_socket_connect(conndata->sock, (struct sockaddr*)&sin, sizeof(sin));
	if (error < 0 )
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, "aos tcp api: failed to connect %d.%d.%d.%d:%d, error %d \n", 
					NIPQUAD(conndata->remote_addr), ntohs(conndata->remote_port),error);
		sock_release(conndata->sock);
		conndata->sock = NULL;
		conndata->rcvcb(conndata, NULL, 0, conndata->userdata, eAosRc_ConnectFailed);
		return eAosRc_ConnectFailed;
	} 
*/
	error = conndata->sock->ops->connect(conndata->sock, (struct sockaddr*)&sin, sizeof(sin), 0);
	if (error < 0 )
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, "aos tcp api: failed to connect %d.%d.%d.%d:%d, error %d \n", 
					NIPQUAD(conndata->remote_addr), ntohs(conndata->remote_port),error);
		sock_release(conndata->sock);
		conndata->sock = NULL;
		conndata->rcvcb(conndata, NULL, 0, conndata->userdata, eAosRc_ConnectFailed);
		return eAosRc_ConnectFailed;
	} 
	aos_debug_log(eAosMD_Platform, "aos tcp api: successful connected to %d.%d.%d.%d:%d\n", 
					NIPQUAD(conndata->remote_addr), ntohs(conndata->remote_port));
	conndata->rcvcb(conndata, NULL, 0, conndata->userdata, eAosRc_ConnSuccess);
	return eAosRc_Success;
}

void do_real_rcv(struct socket *sock, struct aos_tcpapi_data *conndata)
{
//	struct sk_buff *skb;
	unsigned char *buf;
	int len, size=5000;

	buf = (unsigned char *)aos_malloc(size);
	if (buf == NULL)
	{
		aos_alarm(eAosMD_Platform, eAosAlarm_ProgErr, "alloc memeory error!");
		return;
	}
	if (aosSock_State(sock->sk) != TCP_ESTABLISHED && aosSock_State(sock->sk) != TCP_CLOSE_WAIT)
	{
		aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, "aos tcp api:sock->sk %x is no established yet", sock);
		aos_free(buf);
		return;
	}
	while (!skb_queue_empty(&(aosSock_ReceiveQueue(sock->sk)))) 
	{
/*
		lock_sock(sock->sk);
		skb = skb_dequeue(&aosSock_ReceiveQueue(sock->sk));
		release_sock(sock->sk);
		if(!skb ) 
		{
			aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("do real rcv, skb is null"));	
			conndata->rcvcb(conndata, skb->data, skb->len, conndata->userdata, eAosRc_SocketError);
			return;
		}

		if (skb->len == 0) 
		{
			//
			// Chen Ding, 07/17/2005
			
			// Note that we are dealing with TCP here. For all TCP packets, 
			// the length should never be 0 (am I right?!). In other word,
			// if len == 0, it is an error. Note that 'return 0' has a big
			// impact on the processing. The caller (tcp_vs_conn_handle(...))
			// will terminate when this function returns 0. We need to be
			// more careful about this.
			//
			kfree_skb(skb);
			return;
		}
		if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0) 
		{
			aos_warn(eAosMD_Platform, eAosAlarm_ProgErr, ("socket receive data error, skb can not be linearized"));
			kfree_skb(skb);
			continue;
		}
		conndata->rcvcb(conndata, skb->data, skb->len, conndata->userdata, eAosRc_DataRead);
		kfree_skb(skb);	
		conndata->lastupdated = jiffies;		
*/
		len = aos_tcp_recvbuffer(sock, buf, size, 0);
aos_trace("aos tcp rcv conndata 0x%p, len %d", conndata, len);
		if (len > 0)
		{
			conndata->rcvcb(conndata, buf, len, conndata->userdata, eAosRc_DataRead);
			conndata->lastupdated = jiffies;		
		}

	}
	aos_free(buf);
	return;
}

void handle_connect(void)
{
	struct aos_tcpapi_data *conndata, *conndata_next;
	
	list_for_each_entry(conndata, &tcp_do_conn_list, list)
	{
		do_real_connect(conndata);
	}
	down_write(&tcp_conn_list_sem);
	list_for_each_entry_safe(conndata, conndata_next, &tcp_do_conn_list, list)
	{
		list_del(&conndata->list);
		if(conndata_put(conndata))
			list_add_tail(&conndata->list, &tcp_conn_list);
	}
	up_write(&tcp_conn_list_sem);
}

void handle_rcv(void)
{
	struct aos_tcpapi_data *conndata, *conndata_next;
	list_for_each_entry(conndata, &tcp_do_rcv_list, list)
	{
		do_real_rcv(conndata->sock, conndata);
	}

	down_write(&tcp_conn_list_sem);
	list_for_each_entry_safe(conndata, conndata_next, &tcp_do_rcv_list, list)
	{
		list_del(&conndata->list);
		if(conndata_put(conndata))
			list_add_tail(&conndata->list, &tcp_conn_list);
	}
	up_write(&tcp_conn_list_sem);
}

void handle_error(void)
{
	struct aos_tcpapi_data *conndata, *conndata_next;
	
	list_for_each_entry(conndata, &tcp_err_list, list)
	{
		conndata->rcvcb(conndata, NULL, 0, conndata->userdata, conndata->return_code);
	}

	down_write(&tcp_conn_list_sem);
	list_for_each_entry_safe(conndata, conndata_next, &tcp_err_list, list)
	{
		list_del(&conndata->list);
		if(conndata_put(conndata))
			list_add_tail(&conndata->list, &tcp_conn_list);
	}
	up_write(&tcp_conn_list_sem);
}

void handle_wait(void)
{
	struct aos_tcpapi_data *conndata, *conndata_next;
	down_write(&tcp_conn_list_sem);
	list_for_each_entry_safe(conndata, conndata_next, &tcp_wait_list, list)
	{
		remove_wait_queue(aosSock_Sleep(conndata->sock->sk), &conndata->wait);
		list_del(&conndata->list);
		if(conndata_put(conndata))
			list_add_tail(&conndata->list, &tcp_conn_list);
	}
	up_write(&tcp_conn_list_sem);
}

static int rcv_thread(void *param)
{
	struct aos_tcpapi_data *conndata, *conndata_next;
	
	aos_min_log(eAosMD_Platform, "aos tcp api: rcv_thread started");
	lock_kernel();
	aosDaemonize("rcv_thread");
	// Block all signals except SIGKILL and SIGSTOP
#ifdef RH_NPTL_KLUDGE
	spin_lock_irq(&current->sighand->siglock);
#else
	spin_lock_irq(&aosCurrent_Siglock(current));
#endif
	siginitsetinv(&current->blocked, sigmask(SIGKILL) | sigmask(SIGSTOP));
#ifdef RH_NPTL_KLUDGE
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
#else
	aos_recalc_sigpending(current);
	spin_unlock_irq(&aosCurrent_Siglock(current));
#endif
	
	while (start_tcpapi) 
	{
		down_write(&tcp_conn_list_sem);
		list_for_each_entry_safe(conndata, conndata_next, &tcp_conn_list, list)
		{
			// check whether it has done connection
			if (!conndata->sock )
			{
				conndata_get(conndata);
				list_move_tail(&conndata->list, &tcp_do_conn_list);
				continue;
			}
			if (aosSock_State(conndata->sock->sk) != TCP_ESTABLISHED &&
				aosSock_State(conndata->sock->sk) != TCP_CLOSE_WAIT)
			{
				//aos_trace("conndata->sock %p closed", conndata->sock);
				conndata_get(conndata);
				conndata->return_code = eAosRc_SocketClosed;
				list_move_tail(&conndata->list, &tcp_err_list);
				continue;
			}
			// check whether there are received packets
			if (!skb_queue_empty(&(aosSock_ReceiveQueue(conndata->sock->sk))))
			{
				conndata_get(conndata);
				list_move_tail(&conndata->list, &tcp_do_rcv_list);
				continue;
			}
			else
			{
				conndata_get(conndata); // definitly it should be added to another list, so increase refcnt
				// socket is closed
				if (aosSock_State(conndata->sock->sk) == TCP_CLOSE_WAIT )
				{
					aos_trace("remote close connection, conndata 0x%p, conndata->sock 0x%p", conndata, conndata->sock);
					conndata->return_code = eAosRc_SocketClosed;
					list_move_tail(&conndata->list, &tcp_err_list);
					continue;
				}
				// recv timeout occur
				if ((jiffies - conndata->lastupdated) > conndata->timeout)
				{
					aos_debug_log(eAosMD_Platform, "rcv_thread: conndata 0x%p timeout, timeout is %d second", conndata, conndata->timeout/HZ);
					conndata->return_code = eAosRc_Timeout;
					list_move_tail(&conndata->list, &tcp_err_list);
					continue; 
				}
				add_wait_queue(aosSock_Sleep(conndata->sock->sk), &conndata->wait);
				list_move_tail(&conndata->list, &tcp_wait_list);
			}
		}
		up_write(&tcp_conn_list_sem);
		
		handle_connect();
		handle_rcv();
		handle_error();
		
		__set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
		__set_current_state(TASK_RUNNING);
		
		handle_wait();
	}
	/* ?????cleanup tcp_conn_list TBD */
	return 0;
}

void aos_tcpapi_getstats(void)
{
	printk("aos tcp api statistics:\n");
	printk("\tConnection creation requests %lu\n", tcpapi_stats.create_requests);
	printk("\tSuccessful connections %lu\n", tcpapi_stats.conn_success);
	printk("\tFailed connections %lu\n", tcpapi_stats.conn_failed);
	printk("\tBytes Received %lu\n", tcpapi_stats.bytes_rcvd);
	printk("\tBytes Sent %lu\n", tcpapi_stats.bytes_sent);
	printk("\tCurrent Connections %lu\n", tcpapi_stats.current_conns);
}
int aos_tcpapi_start(void)
{
	if (start_tcpapi)
	{
		aos_min_log(eAosMD_Platform, "aos tcp api: tcp api already inited");
		return eAosRc_Success;
	}
	start_tcpapi = 1;
	if (kernel_thread(rcv_thread, NULL, CLONE_VM | CLONE_FS | CLONE_FILES) < 0)
	{
		start_tcpapi = 0;
		printk("aos tcp api: kernel thread create failed\n");
		return eAosRc_Failed;
	}
	aos_min_log(eAosMD_Platform, "aos tcp api: tcp api started");
	memset(&tcpapi_stats, 0, sizeof(struct aos_tcpapi_statistics));
	return eAosRc_Success;
}

int aos_tcpapi_stop(void)
{
	start_tcpapi = 0;
	aos_min_log(eAosMD_Platform, "aos tcp api: tcp api stopping");
	aos_tcpapi_getstats();
	return eAosRc_Success;
}
