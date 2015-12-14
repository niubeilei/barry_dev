////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs.c
// Description:
//
//
// Modification History:
//
////////////////////////////////////////////////////////////////////////////


#define __KERNEL_SYSCALLS__	/*  for waitpid */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/vmalloc.h>
#include <linux/net.h>
#include <linux/sched.h>
#include <linux/skbuff.h>
#include <linux/smp_lock.h>
#include <linux/unistd.h>
#include <linux/wait.h>
#include <linux/syscalls.h> //for sys_wait4()

#include <asm/unistd.h>

#include <net/ip.h>
#include <net/sock.h>
#include <net/tcp.h>
#include <asm/current.h>
#include <linux/sched.h>

#include "aos/aosKernelAlarm.h"
#include "Ktcpvs/tcp_vs.h"
#include "KernelPorting/Sock.h"
#include "KernelPorting/Daemonize.h"
#include "KernelPorting/Current.h"
#include "KernelUtil/aostcpapi.h"
#include "Ktcpvs/interface.h"
#include "Ktcpvs/aosTcpVsCntl.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "CertVerify/aosAccessManage.h"
#include "ssl/Ssl.h" // AosSsl_Start()
#include "ssl/SslStatemachine.h"
//#include "ssl/aosSslProc.h"
#include "ssl/SslCommon.h"
#include "ssl/SslMisc.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"

#include "Ktcpvs/aosProxyConn.h"
#include "Porting/TimeOfDay.h"

// kevin 08/05/2006
#include <linux/proc_fs.h>
#include "Ktcpvs/aosLogFunc.h"
//extern struct list_head tcp_vs_svc_list;

#define waitpid(a,b,c) sys_wait4(a,b,c,0)
MODULE_LICENSE("GPL");
MODULE_AUTHOR("liqin");

//static int errno;

static atomic_t tcp_vs_daemon_count = ATOMIC_INIT(0);
atomic_t aos_tcpvs_curconns = ATOMIC_INIT(0);
int aos_tcpvs_maxconns = MAX_CONCURRENT_CONNECTIONS;

int  aos_back_sslcb(int rc, struct tcp_vs_conn *conn, char *rslt_data, int rslt_data_len);
int  aos_front_sslcb(int rc, struct tcp_vs_conn *conn, char *rslt_data, int rslt_data_len);

/*
 *	Relay data from one socket to the other
 *
 *	Make sure that data is available at "from" before calling it.
 */

static int
skb_send_datagram_socket(const struct sk_buff *skb, struct socket *to)
{
	int written, i;
	struct sk_buff *list;
	int res;

	if (!skb_is_nonlinear(skb))
		return tcp_vs_sendbuffer(to, skb->data, skb->len, 0);

	res = tcp_vs_sendbuffer(to, skb->data, skb_headlen(skb), MSG_MORE);
	if (res < 0)
		return res;
	written = res;

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		char *vaddr;
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
		struct page *page = frag->page;

		vaddr = kmap(page);
		res = tcp_vs_sendbuffer(to, vaddr + frag->page_offset,
					frag->size, 0);
		if (res < 0)
			goto out;
		written += res;
		kunmap(page);
	}

	for (list = skb_shinfo(skb)->frag_list; list; list = list->next) {
		res = skb_send_datagram_socket(list, to);
		if (res < 0)
			goto out;
		written += res;
	}
      out:
	return written;
}

/*

static inline void
skb_entail(struct sock *sk, struct tcp_opt *tp, struct sk_buff *skb)
{
	skb->csum = 0;
	TCP_SKB_CB(skb)->seq = tp->write_seq;
	TCP_SKB_CB(skb)->end_seq = tp->write_seq;
	TCP_SKB_CB(skb)->flags = TCPCB_FLAG_ACK;
	TCP_SKB_CB(skb)->sacked = 0;

	// __skb_queue_tail(&sk->write_queue, skb);
	__skb_queue_tail(&aosSock_WriteQueue(sk), skb);

	// if (sk->forward_alloc < skb->truesize) {
	if (aosSock_ForwardAlloc(sk) < skb->truesize) {
#define TCP_PAGES(amt) (((amt)+TCP_MEM_QUANTUM-1)/TCP_MEM_QUANTUM)
		int pages = TCP_PAGES(skb->truesize);
		// sk->forward_alloc += pages * TCP_MEM_QUANTUM;
		aosSock_ForwardAlloc(sk) += pages * TCP_MEM_QUANTUM;
	}
	tcp_charge_skb(sk, skb);

	if (tp->send_head == NULL)
		tp->send_head = skb;
}
*/

/*
 A positive return-value indicates the number of bytes sent, a negative
 value indicates an error-condition.
 */

static int tcp_vs_relay_socket(struct tcp_vs_conn * conn, char is_client_to_server)
{
	char *data = NULL;
 	unsigned int data_len;
	struct socket	*from,	*to;
	int	ret;

	from=conn->dsock;
	to=conn->csock;
	if(is_client_to_server)
	{
		from=conn->csock;
		to=conn->dsock;
	}
	data = aos_malloc(eAosSSL_RecordMaxLen);
    	aos_assert1(data);
    	data_len = eAosSSL_RecordMaxLen;

//	aos_trace("relay rcv before, pid %d, usec %u", current->pid, AosGetUsec());
    	data_len = tcp_vs_recvbuffer(from, data, data_len, 0);
//	aos_trace("relay rcv after, pid %d, usec %u, rcvlen %d", current->pid, AosGetUsec(), data_len);

	if (data_len < 0)
	{
		aos_free(data);
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "tcp_vs_relay_socket:data receive failed, data_len %d", data_len);
		return -1;
	}

	if(is_client_to_server && conn->svc->scheduler->client_to_server)
	{
		ret = (*conn->svc->scheduler->client_to_server)(data,&data_len,conn,0);
		if(ret<0)
		{
			aos_free(data);
			return -1;
		}
	}
	else if(!is_client_to_server && conn->svc->scheduler->server_to_client)
	{
		(*conn->svc->scheduler->server_to_client)(data,&data_len,conn,0);
	}

	if(data_len>0)
	{
		data_len = tcp_vs_sendbuffer(to, data, data_len, 0);
	}

	aos_free(data);

	return data_len;

}



/*

//
// It reads data from 'from' and relay it to 'to'. It returns negative if error.
// Returns the number of bytes written if successful.
//
int
tcp_vs_relay_socket1(struct socket *from, struct socket *to)
{
	struct sk_buff *skb;
	int len;
	struct sock *sk;
	struct tcp_opt *tp;

	lock_sock(from->sk);
	skb = skb_dequeue(&aosSock_ReceiveQueue(from->sk));
	if (!skb)
	{
		release_sock(from->sk);
		return -1;
	}

	len = skb->len;
	if (len == 0)
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
		release_sock(from->sk);
		return 0;
	}
	release_sock(from->sk);

	if (!sysctl_ktcpvs_zerocopy_send)
	{
		int res;
		int written;

		res = skb_send_datagram_socket(skb, to);
		if (res < 0)
		{
			return res;
		}
		written = res;

		//
		// Chen Ding, 07/17/2005
		// It appears that the only possibility for 'skb_send_datagram_socket(skb, to)'
		// not returning 'res' that makes 'written == len' is that the skb is not
		// linear.
		//
		if (written != len)
		{
			if (skb_is_nonlinear(skb) &&
			    skb_linearize(skb, GFP_ATOMIC) != 0)
			{
				TCP_VS_ERR_RL("relay socket data error "
					      "(len=%d, written=%d).\n",
					      len, written);
				kfree_skb(skb);
				return written;
			}

		    sendagain:
			res = tcp_vs_sendbuffer(to, skb->data + written,
						skb->len - written, 0);
			if (res < 0) {
				kfree_skb(skb);
				return written;
			}

			written += res;
			if (written != len)
				goto sendagain;
		}
		kfree_skb(skb);
		return written;
	}

	// we cannot release the skb here, but we do need to call
	//   its destructor so that the sock_rfree can update the
	//   source sk->rmem_alloc correctly.
	if (skb->destructor) {
		skb->destructor(skb);
	}

	// drop old route
	dst_release(skb->dst);
	skb->dst = NULL;
#ifdef CONFIG_NETFILTER_DEBUG
	skb->nf_debug = 0;
#endif				// CONFIG_NETFILTER_DEBUG

	sk = to->sk;
	tp = &aosSock_TcpOpt(sk);

	lock_sock(sk);

	skb_set_owner_w(skb, sk);
	skb_entail(sk, tp, skb);

	skb->ip_summed = CHECKSUM_HW;
	tp->write_seq += len;
	TCP_SKB_CB(skb)->end_seq += len;

	tcp_push_pending_frames(sk, tp);
	release_sock(sk);

	return len;
}

*/

int aos_send_data(char* data, int data_len, struct socket *dsock)
{
	return tcp_vs_sendbuffer(dsock, data, data_len, 0);
/*	struct sk_buff  *skb;
	struct sock *sk;
	struct tcp_opt *tp;

	aos_debug_log(eAosMD_TcpProxy, "aos_send_data");
	skb = alloc_skb(data_len, GFP_ATOMIC);
	if (!skb)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, ("SendDataToDest: alloc skb failed"));
		return -1;
	}
	aos_debug_log(eAosMD_TcpProxy, "aos_send_data");
	memcpy(skb->data, data, data_len);
	skb->len = data_len;

	skb->dst = NULL;
#ifdef CONFIG_NETFILTER_DEBUG
	skb->nf_debug = 0;
#endif				// CONFIG_NETFILTER_DEBUG

	sk = dsock->sk;
	tp = &aosSock_TcpOpt(sk);

	lock_sock(sk);

	// set the owner of skb to the dest sk
	skb_set_owner_w(skb, sk);
	skb_entail(sk, tp, skb);

	skb->ip_summed = CHECKSUM_HW;
	tp->write_seq += data_len;
	TCP_SKB_CB(skb)->end_seq += data_len;

	tcp_push_pending_frames(sk, tp);
	release_sock(sk);

	aos_debug_log(eAosMD_TcpProxy, "aos_send_data");
	return eAosRc_Success;	*/
}

int aos_app_call_back(struct aos_app_proc_req *appreq)
{
	struct tcp_vs_conn *conn;
	int ret = 0;
	aos_debug_log(eAosMD_TcpProxy, "app_call_back entry, appreq->conn=0x%x", appreq->pTcpVsConn);
	if (!appreq)
	{
		aos_debug_log(eAosMD_TcpProxy, "app_proc_call_back: appreq is null");
		return -1;
	}
	if ((conn = appreq->pTcpVsConn) == NULL)
	{
		aos_debug_log(eAosMD_TcpProxy, "app_prc_call_back: conn is null in appreq");
		return -1;
	}

	switch(appreq-> rslt)
	{
	case eAosRc_ForwardToPeer:
		aos_debug_log(eAosMD_TcpProxy, "app_proc_call_back: Forward To End");
		if (conn->backend_ssl_flags)
		{
			aos_debug_log(eAosMD_TcpProxy, "Forward To End, backend ssl enabled");
			/*ret = AosSslStm_procRequest(
				appreq->submit_buff,
				appreq->submit_buff_len,
				conn->backend_ssl_flags|eAosSSLFlag_Send|eAosSSLFlag_Plain,
				conn,
				NULL);*/
			if (conn->svc->scheduler->app_req_release)
				conn->svc->scheduler->app_req_release(appreq);
			return ret;
		}
		else{
			aos_debug_log(eAosMD_TcpProxy,
							"Forward to End: submit_buff=0x%x, submit_len=%d, conn->dsock=0x%x",
							appreq->submit_buff, appreq->submit_buff_len, conn->dsock);
			aos_send_data(appreq->submit_buff, appreq->submit_buff_len, conn->dsock);
		}
		break;
	case eAosRc_ReturnToPeer:
		aos_debug_log(eAosMD_TcpProxy,
			"app_proc_call_back:Back To Sender submit_buff=%p, submit_len=%d, conn->csock=%p",
			appreq->submit_buff, appreq->submit_buff_len, conn->csock);
		if (conn->front_ssl_flags)
        {
            aos_debug_log(eAosMD_TcpProxy, "Back To Sender, front ssl enabled");
            /*ret = AosSslStm_procRequest(
                appreq->submit_buff,
                appreq->submit_buff_len,
                conn->front_ssl_flags|eAosSSLFlag_Send|eAosSSLFlag_Plain,
                conn,
                NULL);*/
        }
        else{
            aos_send_data(appreq->submit_buff, appreq->submit_buff_len, conn->csock);
        }
		break;
	case eAosRc_Drop:
		aos_debug_log(eAosMD_TcpProxy, "app_proc_call_back:Drop");
		break;
	case eAosRc_DoNothing:
		aos_debug_log(eAosMD_TcpProxy, "app_proc_call_back:DoNothing");
		break;
	default:
		aos_debug_log(eAosMD_TcpProxy, "app_proc_call_back:Unknown rslt %d", appreq->rslt);
		break;
	}

	if (conn->svc->scheduler->app_req_release)
	{
		conn->svc->scheduler->app_req_release(appreq);
	}
	return eAosRc_Success;
}

int aos_front_plain_rcved(struct tcp_vs_conn *conn, char *data, int data_len)
{
	struct tcp_vs_service *svc;
//	struct aos_user pmi_user;
	int ret =  eAosRc_Success;

	aos_assert1(conn);
	svc = conn->svc;

	if (svc->scheduler->client_to_server)
	{
		ret = svc->scheduler->client_to_server(data, &data_len, conn, aos_app_call_back);
		if(ret<0)
		{
			aos_trace("client_to_server error");
			return -1;
		}
		else
		{
			aos_trace("client_to_server ok");
		}
	}
	if (data_len < 0)
		return -1;
	else if(data_len==0)
		return 0;

	aos_debug_log(eAosMD_TcpProxy,"directly send data to backend %d, pid %d, usec %u",
			data_len, current->pid, AosGetUsec());
	ret = aos_send_data(data, data_len, conn->dsock);

	return ret;
}

int aos_client_to_server(struct tcp_vs_conn *conn, struct tcp_vs_service *svc)
{
	int rcvlen = 0;
	int rc = 0;

//	aos_debug_log(eAosMD_TcpProxy, "aos client to server");

	if ( conn->dsock && !svc->conf.pmi &&
		 !conn->front_ssl_flags && !conn->backend_ssl_flags && !svc->conf.dynamic_dst )
	{
		return tcp_vs_relay_socket(conn, 1);
	}

	// flow control mechanism
	if (!conn->dsock || (conn->front_ssl_flags && !conn->front_context) ||
		(conn->front_context && conn->front_context->state != eAosSSLState_Established))
	{
		goto CTSRCV;
	}
	else if (!conn->backend_ssl_flags ||
			 (conn->backend_context && conn->backend_context->state == eAosSSLState_Established))
	{
		goto CTSRCV;
	}
	else
	{
		aos_debug_log(eAosMD_TcpProxy,"ctos, backend ssl is not established yet");
        __set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(conn->client_delay);
		conn->client_delay <<= 1;
        __set_current_state(TASK_RUNNING);
        return eAosRc_Success+1;
    }

CTSRCV:
    aos_assert1(conn->client_buff);
	if (!conn->delay_to_server)
	{
		aos_trace("cts rcv before, pid %d, usec %u", current->pid, AosGetUsec());
		rcvlen = tcp_vs_recvbuffer(conn->csock, conn->client_buff, conn->cbuff_len, 0);
		//rcvlen = tcp_vs_recvbuffer(conn->csock, conn->client_buff,
		//	conn->backend_ssl_flags?conn->cbuff_len-29:conn->cbuff_len, 0);
		aos_trace("cts rcv after, pid %d, usec %u, rcvlen %d", current->pid, AosGetUsec(), rcvlen);
	}
	else
	{
		//aos_trace_hex("cts delay", conn->delay_to_server, conn->delay_to_server_len);
		memcpy(conn->client_buff, conn->delay_to_server, conn->delay_to_server_len);
		rcvlen = conn->delay_to_server_len;
		aos_free(conn->delay_to_server);
		conn->delay_to_server = NULL;
		conn->delay_to_server_len = 0;
	}
	if (!rcvlen)
		return eAosRc_Success;
	//aos_debug_log(eAosMD_TcpProxy, "client to server %d", rcvlen);

	// if fronted ssl, do ssl handshake and application data decryption
	if  (conn->front_ssl_flags)
	{
		aos_trace("cts forntssl before, pid %d, usec %u", current->pid, AosGetUsec());
/*		rc = AosSslStm_procRequest(conn->client_buff, rcvlen,
				conn->front_ssl_flags|eAosSSLFlag_Receive, conn, aos_front_sslcb);
*/
		aos_trace("cts frontssl after, pid %d, usec %u", current->pid, AosGetUsec());
		return rc;
	}

	// plain text received processing
	rc = aos_front_plain_rcved(conn, conn->client_buff, rcvlen);

	return rc;
}

int  aos_back_sslcb(int rc, struct tcp_vs_conn *conn, char *rslt_data, int rslt_data_len)
{
	int  ret = eAosRc_Success;

	// should be the return of decryption
	//aos_debug_log(eAosMD_TcpProxy, "backend ssl call back");

	switch (rc)
	{
	case eAosContentType_AppData:
		if (!rslt_data || rslt_data_len <= 0)
		{
			return aos_alarm(eAosMD_TcpProxy, eAosAlarmProgErr,
				"invalid rslt_data 0x%x, rslt_data_len %d", (__u32)rslt_data, rslt_data_len);
		}
		if (conn->front_ssl_flags)
		{
			// do encryption and send back to client
/*			ret = AosSslStm_procRequest(rslt_data, rslt_data_len,
					conn->front_ssl_flags|eAosSSLFlag_Send|eAosSSLFlag_Plain, conn, NULL);
*/
		}
		else
		{
			// send plain data back to client
			//aos_trace("before aos_send_data %u", AosGetUsec());
			ret = aos_send_data(rslt_data, rslt_data_len, conn->csock);
			//aos_trace("aos_back_sslcb, jiffies %u, %u", jiffies, AosGetUsec());
		}
		break;
	default:
		aos_warn(eAosMD_TcpProxy, eAosAlarm_ProgErr, "server to client, front ssl proc return %d", rc);
		return -1;
	}
	return ret;

}

int aos_server_to_client(struct tcp_vs_conn *conn)
{
	//struct sk_buff *skb;
    int data_len = 0;
	int rc = eAosRc_Success;

	/*aos_debug_log(eAosMD_TcpProxy,
		"aos_server_to_client: from %d.%d.%d.%d:%d to %s %d.%d.%d.%d:%d",
		NIPQUAD(conn->dest->addr),
		ntohs(conn->dest->port),
		svc->ident.name,
		NIPQUAD(svc->conf.addr),
		ntohs(svc->conf.port));
	*/
	if (!conn->backend_ssl_flags && !conn->front_ssl_flags)
	{
		return tcp_vs_relay_socket(conn, 0);
	}

	// receive flow control, check whehter the client side can be write then receive from server
	/*if ( !skb_queue_empty(&(aosSock_WriteQueue(conn->csock->sk))) )
    {
        aos_trace("1client write queue is not empty");
        return 0;
    }*/

	// get received skb data
    aos_assert1(conn->server_buff);

	//aos_trace("server to client before receive %u", AosGetUsec());

	// the following is flow control mechanism
	if ((conn->backend_ssl_flags && !conn->backend_context) ||
		(conn->backend_context && conn->backend_context->state != eAosSSLState_Established))
	{
		aos_trace("stoc rcv before, pid %d, usec %u", current->pid, AosGetUsec());
        	data_len = tcp_vs_recvbuffer(conn->dsock, conn->server_buff, conn->sbuff_len, 0);
        	aos_trace("stoc rcv after, pid %d, usec %u, len=%d", current->pid, AosGetUsec(), data_len);
	}
	else if(!conn->front_ssl_flags ||
			 (conn->front_context && conn->front_context->state == eAosSSLState_Established))
	{
		aos_trace("stoc rcv before, pid %d, usec %u", current->pid, AosGetUsec());
        	data_len = tcp_vs_recvbuffer(conn->dsock, conn->server_buff, conn->sbuff_len, 0);
        	aos_trace("stoc rcv after, pid %d, usec %u, len=%d", current->pid, AosGetUsec(), data_len);
	}
	else // flow control
	{
		aos_trace("stoc flow control, front ssl is not established yet");
		__set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(conn->server_delay);
		conn->server_delay <<= 1;
		__set_current_state(TASK_RUNNING);
		return eAosRc_Success+1;
	}

	if (!data_len)
		return eAosRc_Success;

	//aos_debug_log(eAosMD_TcpProxy,
	//	"server to client data_len %d, jiffies %u, usec %u", data_len, jiffies, AosGetUsec());
	// if backend ssl, before send back to client, we should do decryption first
	if (conn->backend_ssl_flags)
	{
	//aos_trace("server to client before procRequest %u", AosGetUsec());
/*		rc = AosSslStm_procRequest(conn->server_buff, data_len,
				conn->backend_ssl_flags|eAosSSLFlag_Receive, conn, aos_back_sslcb);
*/
	//aos_trace("server to client end procRequest%u", AosGetUsec());
		return rc;
	}

	// if fronted ssl, before send back to client, we should do encryption
/*	if (conn->front_ssl_flags)
	{
		//aos_trace("data_len %d", data_len);
		rc = AosSslStm_procRequest(data, data_len,
				conn->front_ssl_flags|eAosSSLFlag_Send|eAosSSLFlag_Plain, conn, NULL);
		aos_free(data);
		return rc;
	}

	// directly send back data to front
	rc = aos_send_data(data, data_len, conn->csock);
*/
	aos_back_sslcb(eAosContentType_AppData, conn, conn->server_buff, data_len);
	return rc;
}

static void tcp_vs_constuct_auth_fail_infor( struct tcp_vs_conn *conn, LPVCHAR out, char *in )
{
	int inLen = strlen(in);
	
	VCHAR_MALLOC( (*out), conn->error_info_prefix.len+inLen+3 ); //here set out->len = 0;
	if( out->buf == NULL ){
		//add failure information to here
		return;
	}
	memcpy( out->buf, conn->error_info_prefix.buf, conn->error_info_prefix.len );
	memcpy( out->buf+conn->error_info_prefix.len, in, inLen );
	out->len = conn->error_info_prefix.len+inLen;
	out->buf[out->len] = '\0';
	/*
	aos_file_printk("/usr/local/keyou/Log/syslog.err",\
		"=======\n\t%s[%d]\n\t%s[%d]\n%s",\
		conn->error_info_prefix.buf, conn->error_info_prefix.len,\
		in, inLen, out->buf );
	*/
	//AOS_DEBUG_PRINTK("%s\n", out->buf );
}

/*
 *   Handle TCP connection between client and the tcpvs, and the one
 *   between the tcpvs and the selected server. Terminate until that
 *   the two connections are done.
 */

void session_time_control(unsigned long data)
{
        char buf[128];
        VCHAR timeMsg;
        
        struct tcp_vs_conn *conn=(struct tcp_vs_conn *)(data);

	 if( conn==NULL || conn->session_time_out_flags == -1 )return;

        snprintf( buf, 128, "\r\n******     HAC : Your valid time is %ld minutes !     ******\r\n", conn->session_time/60);
        tcp_vs_constuct_auth_fail_infor( conn, &timeMsg, buf );
        tcp_vs_sendbuffer(conn->csock, timeMsg.buf, timeMsg.len, 0);
        AOS_DEBUG_PRINTK( timeMsg.buf );
	 VCHAR_FREE( timeMsg );
        aos_free( conn->session_timer );
        conn->session_time_out_flags=1;
        conn->session_timer = NULL;
}

int session_time_control_init(struct tcp_vs_conn *conn)
{
	//if don't need time control
	if( conn->session_time <= 0 )return 0;
	
	conn->session_timer = (struct timer_list *)aos_malloc( sizeof(struct timer_list) );
	if( conn->session_timer == NULL )
	{
		AOS_DEBUG_PRINTK("Malloc memory failure !");
		return -1;
	}
	else
	{
		init_timer( conn->session_timer );
		conn->session_timer->expires = jiffies+HZ*conn->session_time;
		conn->session_timer->data = (u32)conn;
		conn->session_timer->function = session_time_control;
		add_timer( conn->session_timer );
	}
	return 0;
}

void session_time_control_destroy(struct tcp_vs_conn *conn)
{
	if(conn->session_timer == NULL)return;
	
	del_timer( conn->session_timer );
       aos_free( conn->session_timer );
       conn->session_timer = NULL;
}

//
//Pownall.Zhang, 12/22/2006
//To find a connect based on session id.
//
struct tcp_vs_conn *
tcp_vs_conn_lookup_byid(const __u64 session_id)
{
	struct list_head *c,*e;
	struct tcp_vs_service *svc;
	struct tcp_vs_conn *conn;

	list_for_each( e, &tcp_vs_svc_list ){
		svc = list_entry(e, struct tcp_vs_service, list);
		list_for_each( c, &svc->connections ) {
			conn = list_entry(c, struct tcp_vs_conn, n_list);
			if ( conn->session_id == session_id )
			return conn;
		}
	}

	return NULL;
}


int
tcp_vs_conn_handle(struct tcp_vs_conn *conn, struct tcp_vs_service *svc)
{
	struct socket *csock;
	DECLARE_WAITQUEUE(wait1, current);
	DECLARE_WAITQUEUE(wait2, current);
	unsigned long lastupdated;
	int ret;

	//aos_min_log(eAosMD_TcpProxy, "tcp_vs_conn_handle");

	csock = conn->csock;
	/*
	if (aosSock_State(csock->sk) != TCP_ESTABLISHED)
	{
		if (aosSock_State(csock->sk) == TCP_CLOSE_WAIT)
		{
			aos_min_log(eAosMD_TcpProxy, "tcp_vs_conn_handle: csock closed");
			return 0;
		}
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr,
			"Error connection not established (state %d)",
			aosSock_State(csock->sk));
		return -1;
	}
	*/

	//
	//   call its scheduler to process the connect request, the scheduler may:
	//   1, Select a destination server and return 0, or;
	//   2, Deal with the requestion alone and return 1, or;
	//   3, Return -1  when could not find a right server
	//   4, return -2 when other errors such as socket broken occur.
	//
	if (!svc->conf.dynamic_dst)
	{
		if (svc->scheduler->schedule(conn, svc, NULL, 0) != 0)
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, ("static schedule failed"));
			return -1;
		}
		/*
		if (conn->dsock == NULL)
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, ("conn->dscok is null"));
			return -1;
		}
		*/


		// start ssl client state machine
/*		if (conn->backend_ssl_flags && AosSsl_Start(conn, eAosSSLFlag_Client) < 0)
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed");
			return -1;
 		}*/
	}

	//
	//  NOTE: we should add a mechanism to provide higher degree of
	//        fault-tolerance here in the future, if the destination
	//        server is dead, we need replay the request to a
	//       surviving one, and continue to provide the service to
	//        the established connection.
	//        Transaction and Logging?
	//        We need to explore.
	//

	lastupdated = jiffies;
	//
	// It has a control over the connection. If the connection has been
	// idled for too long (sysctl_ktcpvs_read_timeout seconds), it will
	// terminate the connection.
	//
	while (!svc->stop && jiffies-lastupdated<sysctl_ktcpvs_read_timeout*HZ && !conn->session_time_out_flags )
	{
		//
		// Check the destination socket is good
		// if the connection is closed, go out of this loop
		//
		if (conn->dsock &&
		    aosSock_State(conn->dsock->sk) != TCP_ESTABLISHED &&
		    aosSock_State(conn->dsock->sk) != TCP_CLOSE_WAIT)
		{
			//aos_trace("dsock closed or not established");
			aos_min_log(eAosMD_TcpProxy, "tcp_vs_conn_handle: dsock closed or not established");
			break;
		}

		//
		// Check the source socket is good
		//
		if (aosSock_State(csock->sk) != TCP_ESTABLISHED
		    && aosSock_State(csock->sk) != TCP_CLOSE_WAIT)
		{
			//aos_trace("csock closed or not established");
//			aos_min_log(eAosMD_TcpProxy, "tcp_vs_conn_handle: csock closed or not established");
			break;
		}

		//
		// Do we have data from server?
		//
		if ( conn->dsock &&
			 !skb_queue_empty(&(aosSock_ReceiveQueue(conn->dsock->sk))) )// &&
			 //(jiffies - lastupdated) >= 2 )
		{
			//
			// There are data from the server. If there is a content processor
			// ask the processor to process the event.
			//
			if ((ret = aos_server_to_client(conn) < 0))
			{
				aos_trace("the return value of sever_to_client < 0");
				break;
			}
			if (ret == 0)
				lastupdated = jiffies;
		}

		//
		// Do we have data from client?
		//
		if ( (!skb_queue_empty(&(aosSock_ReceiveQueue(csock->sk))))// && (jiffies - lastupdated) >= 2)
			 || conn->delay_to_server)
		{
			// ssl processing or app accessing control processing
			if ((ret = aos_client_to_server(conn, svc)) < 0)
			{
				aos_min_log(eAosMD_TcpProxy, "the return value of client_to_server < 0");
				break;
			}
			if ( ret == 0 )
				lastupdated = jiffies;
		}

		if (conn->dsock)
		{
			if (skb_queue_empty(&(aosSock_ReceiveQueue(conn->dsock->sk))) &&
		    	skb_queue_empty(&(aosSock_ReceiveQueue(csock->sk))))
			{
				//
				// Check whether the sockets are closing
				//
				if ( !conn->delay_to_server &&
						(aosSock_State(conn->dsock->sk) == TCP_CLOSE_WAIT
						 ||
				    		aosSock_State(csock->sk) == TCP_CLOSE_WAIT)
						)
				{
					//aos_trace("dsock or csock  closed");
					//aos_min_log(eAosMD_TcpProxy, "tcp_vs_conn_handle: dsock or csock closed ");
					break;
				}

				//
				//  Put the current task on the sleep wait queue
				//  of both the sockets, wake up the task if one
				//  socket has some data ready.
				//
				add_wait_queue(aosSock_Sleep(csock->sk), &wait1);
				add_wait_queue(aosSock_Sleep(conn->dsock->sk), &wait2);
				__set_current_state(TASK_INTERRUPTIBLE);
				schedule_timeout(HZ);
				__set_current_state(TASK_RUNNING);
				remove_wait_queue(aosSock_Sleep(csock->sk), &wait1);
				remove_wait_queue(aosSock_Sleep(conn->dsock->sk), &wait2);

			}
		}
		else
		{ // conn->dsock is 0
			if(skb_queue_empty(&(aosSock_ReceiveQueue(csock->sk))))
			{
				if (aosSock_State(csock->sk) == TCP_CLOSE_WAIT)
				{
					aos_min_log(eAosMD_TcpProxy, "tcp_vs_conn_handle: csock closed ");
					break;
				}
				add_wait_queue(aosSock_Sleep(csock->sk), &wait1);
				__set_current_state(TASK_INTERRUPTIBLE);
				schedule_timeout(HZ);
				__set_current_state(TASK_RUNNING);
				remove_wait_queue(aosSock_Sleep(csock->sk), &wait1);
			}
		}
	}//while ((jiffies - lastupdated) < sysctl_ktcpvs_read_timeout * HZ)

	return 0;
}


enum {
	SERVER_DEAD = 0,
	SERVER_STARTING,
	SERVER_READY,
	SERVER_BUSY
};

#ifndef MAX_SPAWN_RATE
#define MAX_SPAWN_RATE	32
#endif

struct tcp_vs_child_table {
	struct tcp_vs_child children[KTCPVS_CHILD_HARD_LIMIT];
	int max_daemons_limit;
	int idle_spawn_rate;
	unsigned long last_modified;	/* last time of add/killing child */
};

static int tcp_vs_child(void *__child);

static inline void
make_child(struct tcp_vs_child_table *tbl,
	   int slot, struct tcp_vs_service *svc)
{
	//aos_min_log(eAosMD_TcpProxy, "To make child");

	if (slot + 1 > tbl->max_daemons_limit)
		tbl->max_daemons_limit = slot + 1;
	tbl->last_modified = jiffies;
	tbl->children[slot].svc = svc;
	if (kernel_thread(tcp_vs_child, &tbl->children[slot],
			  CLONE_VM | CLONE_FS | CLONE_FILES) < 0)
		TCP_VS_ERR("spawn child failed\n");
}

static inline void
kill_child(struct tcp_vs_child_table *tbl, int slot)
{
	kill_proc(tbl->children[slot].pid, SIGKILL, 1);
	tbl->last_modified = jiffies;
}

static inline void
update_child_status(struct tcp_vs_child *chd, __u16 status)
{
	chd->status = status;
}


static inline void
child_pool_maintenance(struct tcp_vs_child_table *tbl,
		       		   struct tcp_vs_service *svc)
{
	int i;
	int free_slots[MAX_SPAWN_RATE];
	int free_length = 0;
	int to_kill = -1;
	int idle_count = 0;
	int last_non_dead = -1;

	for (i = 0; i < svc->conf.maxClients; i++)
	{
		int status;

		if (i >= tbl->max_daemons_limit
		    && free_length == tbl->idle_spawn_rate)
			break;

		status = tbl->children[i].status;
		switch (status)
		{
		case SERVER_DEAD:
			 if (free_length < tbl->idle_spawn_rate)
			 {
				free_slots[free_length] = i;
				free_length++;
			 }
			 break;

		case SERVER_STARTING:
			 idle_count++;
			 last_non_dead = i;
			 break;

		case SERVER_READY:
			 idle_count++;
			 to_kill = i;
			 last_non_dead = i;
			 break;

		case SERVER_BUSY:
			 last_non_dead = i;
			 break;
		}
	}

	tbl->max_daemons_limit = last_non_dead + 1;

	if (idle_count > svc->conf.maxSpareServers)
	{
		//
		// kill one child each time
		//
		aos_trace("try to kill one child, idle_count %d, to_kill %d", idle_count, to_kill);
		kill_child(tbl, to_kill);
		tbl->idle_spawn_rate = 1;
	}
	else if (idle_count < svc->conf.minSpareServers)
	{
		if (free_length)
		{
			if (tbl->idle_spawn_rate > 8 && net_ratelimit())
				aos_prod_log(eAosMD_TcpProxy,
					"Server %s seems busy, you may "
				     "need to increase StartServers, "
				     "or Min/MaxSpareServers free_length %d",
				     svc->ident.name, free_length);

			//
			// spawn a batch of children
			//
			for (i = 0; i < free_length; i++)
			{
				make_child(tbl, free_slots[i], svc);
			}

			if (tbl->idle_spawn_rate < MAX_SPAWN_RATE)
			{
				tbl->idle_spawn_rate *= 2;
			}
		}
		else if (net_ratelimit())
		{
			aos_prod_log(eAosMD_TcpProxy,
			     "Server %s reached MaxClients setting, "
			     "consider raising the MaxClients "
			     "setting\n", svc->ident.name);
		}
	}
	else
	{
		//
		// if the number of spare servers remains in the interval
		// (minSpareServers, maxSpareServers] and the time of
		// last modified is larger than ten minutes, we try to
		// kill one spare child in order to release some resource.
		//
		if (idle_count > svc->conf.minSpareServers
		    && jiffies - tbl->last_modified > 600 * HZ)
		{
			aos_trace("kill on child idle_cout %d, min %d", idle_count, svc->conf.minSpareServers);
			kill_child(tbl, to_kill);
		}

		tbl->idle_spawn_rate = 1;
	}
}


//
// This is the thread function for a child.
//
static int
tcp_vs_child(void *__child)
{
	struct tcp_vs_conn *conn;
	struct socket *sock;
	int ret = 0;
	struct tcp_vs_child *chd = (struct tcp_vs_child *) __child;
	struct tcp_vs_service *svc = chd->svc;

	/* DECLARE_WAIT_QUEUE_HEAD(queue); */
	DECLARE_WAITQUEUE(wait, current);

	atomic_inc(&svc->childcount);
aos_trace("%s childcount %d", svc->ident.name, atomic_read(&svc->childcount));
	chd->pid = current->pid;
	update_child_status(chd, SERVER_STARTING);

	snprintf(current->comm, sizeof(current->comm),
		 "ktcpvs %s c", svc->ident.name);
	lock_kernel();
	aosDaemonize(svc->ident.name);

	/* Block all signals except SIGKILL and SIGSTOP */
#ifdef RH_NPTL_KLUDGE
	spin_lock_irq(&current->sighand->siglock);
#else
	// spin_lock_irq(&current->sigmask_lock);
	spin_lock_irq(&aosCurrent_Siglock(current));
#endif
	siginitsetinv(&current->blocked,
		      sigmask(SIGKILL) | sigmask(SIGSTOP));
#ifdef RH_NPTL_KLUDGE
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
#else
	// recalc_sigpending(current);
	// spin_unlock_irq(&current->sigmask_lock);
	aos_recalc_sigpending(current);
	spin_unlock_irq(&aosCurrent_Siglock(current));
#endif

	sock = svc->mainsock;
	if (sock == NULL)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "%s's socket is NULL", svc->ident.name);
		ret = -1;
		goto out;
	}

	//
	// This is the thread loop.
	//
	while (svc->stop == 0 && sysctl_ktcpvs_unload == 0)
	{
		if (signal_pending(current))
		{
			//aos_min_log(eAosMD_TcpProxy, "child (pid=%d): signal received\n", current->pid);
			break;
		}

		update_child_status(chd, SERVER_READY);
		// refer to inet_csk_accept()
		//if (aosSock_TcpOpt(sock->sk).accept_queue == NULL)
		if(reqsk_queue_empty(&(inet_csk(sock->sk)->icsk_accept_queue)))
		{
			/* interruptible_sleep_on_timeout(&queue, HZ); */
			add_wait_queue(aosSock_Sleep(sock->sk), &wait);
			__set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(HZ);
			__set_current_state(TASK_RUNNING);
			remove_wait_queue(aosSock_Sleep(sock->sk), &wait);
			continue;
		}

		if (atomic_read(&aos_tcpvs_curconns) >= aos_tcpvs_maxconns)
		{
			aos_min_log(eAosMD_TcpProxy, "Reach max Conns %d, current conns is %d",
						aos_tcpvs_maxconns, atomic_read(&aos_tcpvs_curconns));
			__set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(100);
			__set_current_state(TASK_RUNNING);
			continue;
		}

		/* create tcp_vs_conn object */
		conn = (struct tcp_vs_conn*)AosAppProxy_connCreate(sock);
		if (!conn)
			break;

		conn->svc = svc;
		//init kevin
		list_add(&conn->n_list, &svc->connections);

		/* Do the actual accept */
		ret = sock->ops->accept(sock, conn->csock, O_NONBLOCK);
		//aos_trace("accept child thread %x pid %d", current, current->pid);
		if (ret < 0) {
			aos_alarm(eAosMD_TcpProxy, eAosAlarm, "accept failed child thread %x pid %d", current, current->pid);
			//tcp_vs_conn_release(conn);
			list_del(&conn->n_list);
			AosAppProxy_connPut(conn);
			continue;
		}

		update_child_status(chd, SERVER_BUSY);
		atomic_inc(&svc->conns);
		atomic_inc(&aos_tcpvs_curconns);

		conn->front_ssl_flags = svc->conf.front_ssl_flags;
		conn->backend_ssl_flags = svc->conf.back_ssl_flags;
		// start ssl server state machine
		if (conn->front_ssl_flags)
		{
#ifdef CONFIG_AOS_SSL
			conn->front_cert = AosCertMgr_getSystemCert();
			if (conn->front_cert)
			{
				AosCert_hold(conn->front_cert);
			}
			else
			{
				aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no system cert");
				list_del(&conn->n_list);
				AosAppProxy_connPut(conn);
				atomic_dec(&svc->conns);
				atomic_dec(&aos_tcpvs_curconns);
				break;
			}
			conn->front_prikey = AosCertMgr_getSystemPrivKey();
			//if (!conn->front_prikey && !aos_ssl_hardware_flag)
			if (!conn->front_prikey && (gAosSslAcceleratorType != eAosSslAcceleratorType_517PM) )
			{
				aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no system pri key");
				list_del(&conn->n_list);
				AosAppProxy_connPut(conn);
				atomic_dec(&svc->conns);
				atomic_dec(&aos_tcpvs_curconns);
				break;
			}
			conn->front_authorities = AosCertChain_getByName("system");
			if (!conn->front_authorities)
			{
				aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no authorities certs");
				list_del(&conn->n_list);
				AosAppProxy_connPut(conn);
				atomic_dec(&svc->conns);
				atomic_dec(&aos_tcpvs_curconns);
				break;
			}
/*			if(AosSsl_Start(conn, eAosSSLFlag_Server) < 0)
			{
				aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed");
				AosAppProxy_connPut(conn);
				atomic_dec(&svc->conns);
				atomic_dec(&aos_tcpvs_curconns);
				break;
			}*/
	#endif
		}

		/* Do the work */
		ret = tcp_vs_conn_handle(conn, svc);

		if (ret < 0) {
			aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error handling connecting conn->refcnt %d", atomic_read(&conn->refcnt));
			list_del(&conn->n_list);
			AosAppProxy_connPut(conn);
			atomic_dec(&svc->conns);
			atomic_dec(&aos_tcpvs_curconns);
			break;
		};

		//list_del(&conn->n_list, &svc->connections);
		list_del(&conn->n_list);
		AosAppProxy_connPut(conn);
		atomic_dec(&svc->conns);
		atomic_dec(&aos_tcpvs_curconns);
	}

out:
	update_child_status(chd, SERVER_DEAD);
	atomic_dec(&svc->childcount);
	//aos_trace("tcp vs child exit");
	return 0;
}


static int
tcp_vs_daemon(void *__svc)
{
	int waitpid_result;
	int i;
	struct tcp_vs_service *svc = (struct tcp_vs_service *) __svc;
	struct tcp_vs_child_table *child_table = NULL;

	DECLARE_WAIT_QUEUE_HEAD(WQ);

	atomic_inc(&tcp_vs_daemon_count);

	snprintf(current->comm, sizeof(current->comm),
		 "ktcpvs %s d", svc->ident.name);
	lock_kernel();
	aosDaemonize("tcp_vs_daemon");

	aos_trace("To start a service: %s\n", svc->ident.name);

	/* Block all signals except SIGKILL and SIGSTOP */
#ifdef RH_NPTL_KLUDGE
	spin_lock_irq(&current->sighand->siglock);
#else
	// spin_lock_irq(&current->sigmask_lock);
	spin_lock_irq(&aosCurrent_Siglock(current));
#endif
	siginitsetinv(&current->blocked,
		      sigmask(SIGKILL) | sigmask(SIGSTOP));
#ifdef RH_NPTL_KLUDGE
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
#else
	// recalc_sigpending(current);
	// spin_unlock_irq(&current->sigmask_lock);
	aos_recalc_sigpending(current);
	spin_unlock_irq(&aosCurrent_Siglock(current));
#endif

	if (!svc->scheduler) {
		TCP_VS_ERR("%s's scheduler is not bound\n",
			   svc->ident.name);
		goto out;
	}

	child_table = vmalloc(sizeof(*child_table));
	if (!child_table)
		goto out;

	/* Then start listening and spawn the daemons */

	aos_trace( "To start listening on %d.%d.%d.%d:%d", NIPQUAD(svc->conf.addr), ntohs(svc->conf.port));
	if (StartListening(svc) < 0)
		goto out;

	atomic_set(&svc->running, 1);
	atomic_set(&svc->childcount, 0);
	svc->stop = 0;

	memset(child_table, 0, sizeof(*child_table));
	child_table->idle_spawn_rate = 1;

	aos_trace( "To start servers: %d", svc->conf.startservers);

	for (i = 0; i < svc->conf.startservers; i++)
	{
		make_child(child_table, i, svc);
	}

	/* Then wait for deactivation */
	while (svc->stop == 0 && !signal_pending(current)
	       && sysctl_ktcpvs_unload == 0) {
		interruptible_sleep_on_timeout(&WQ, HZ);

		/* dynamically keep enough thread to handle load */
		child_pool_maintenance(child_table, svc);

		/* reap the zombie daemons */
		waitpid_result = waitpid(-1, NULL, __WCLONE | WNOHANG);
	}

	/* Wait for tcp_vs_child to stop, one second per iteration */
	while (atomic_read(&svc->childcount) > 0)
		interruptible_sleep_on_timeout(&WQ, HZ);

	/* reap the zombie daemons */
	waitpid_result = 1;
	while (waitpid_result > 0)
		waitpid_result = waitpid(-1, NULL, __WCLONE | WNOHANG);

	/* stop listening */
	StopListening(svc);

      out:
	if (child_table)
		vfree(child_table);
	svc->start = 0;
	atomic_set(&svc->running, 0);
	atomic_dec(&tcp_vs_daemon_count);

	return 0;
}


static int
master_daemon(void *unused)
{
	int waitpid_result;
	struct list_head *l;
	struct tcp_vs_service *svc;
	DECLARE_WAIT_QUEUE_HEAD(WQ);
//	MOD_INC_USE_COUNT;

	aos_min_log(eAosMD_TcpProxy, "master_daemon thread start");
	sprintf(current->comm, "ktcpvs master");
	lock_kernel();
	aosDaemonize("ktcpvs-master");

	/* Block all signals except SIGKILL and SIGSTOP */
#ifdef RH_NPTL_KLUDGE
	spin_lock_irq(&current->sighand->siglock);
#else
	spin_lock_irq(&aosCurrent_Siglock(current));
#endif
	siginitsetinv(&current->blocked,
		      sigmask(SIGKILL) | sigmask(SIGSTOP));
#ifdef RH_NPTL_KLUDGE
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
#else
	// recalc_sigpending(current);
	// spin_unlock_irq(&current->sigmask_lock);
	aos_recalc_sigpending(current);
	spin_unlock_irq(&aosCurrent_Siglock(current));
#endif

	/* main loop */
	while (sysctl_ktcpvs_unload == 0) {
		read_lock(&__tcp_vs_svc_lock);
		list_for_each(l, &tcp_vs_svc_list) {
			svc = list_entry(l, struct tcp_vs_service, list);
			if (!atomic_read(&svc->running) && svc->start)
			{
				aos_trace("To start a service: %s\n", svc->ident.name);
				kernel_thread(tcp_vs_daemon, svc, 0);
			}
		}
		read_unlock(&__tcp_vs_svc_lock);

		/* run the slowtimer collection */
		tcp_vs_slowtimer_collect();

		if (signal_pending(current))
			break;

		current->state = TASK_INTERRUPTIBLE;
		interruptible_sleep_on_timeout(&WQ, HZ);

		/* reap the daemons */
		waitpid_result = waitpid(-1, NULL, __WCLONE | WNOHANG);
	}

	/* Wait for tcp_vs daemons to stop, one second per iteration */
	while (atomic_read(&tcp_vs_daemon_count) > 0)
		interruptible_sleep_on_timeout(&WQ, HZ);

	/* reap the zombie daemons */
	waitpid_result = 1;
	while (waitpid_result > 0)
		waitpid_result = waitpid(-1, NULL, __WCLONE | WNOHANG);

	/* flush all the virtual servers */
	tcp_vs_flush();

	aos_min_log(eAosMD_TcpProxy, "The master daemon stopped");

	//MOD_DEC_USE_COUNT;

	return 0;
}


// kevin, 08/05/2006
static int aosAppProxy_procRead(char *buf, char **start, off_t offset, int count1, int *eof, void *data)
{
	struct tcp_vs_service *svc;
	struct list_head *l, *ll;
	struct list_head *e;
	tcp_vs_dest_t *dest;
	int count = 0;
	int end = 0;

	if(offset>0)
		return 0;

	sprintf(&buf[end], "Concurrent Conns: %d    [MAX: %d]\n", atomic_read(&aos_tcpvs_curconns), aos_tcpvs_maxconns);
	end=strlen(buf);
	//sprintf(&buf[end], "------------------------------- The App Proxy List ----------------------------\n");
	//end=strlen(buf);
	// for keyou HAC
	//sprintf(&buf[end], "Name\t\tStatus\tType\tVS_IP\t\tVS_Port\t\tF_SSL\tConns\tRS\n");
	//end=strlen(buf);
	//sprintf(&buf[end], "-------------------------------------------------------------------------------\n");
	//end=strlen(buf);

	list_for_each(l, &tcp_vs_svc_list)
	{
		svc = list_entry(l, struct tcp_vs_service, list);

		sprintf(&buf[end], "%s ", svc->ident.name);
		end=strlen(buf);

		if (svc->start && !svc->stop)
		{
			sprintf(&buf[end], "Started ");
		}
		else
		{
			sprintf(&buf[end], "Stopped ");
		}
		end=strlen(buf);

    	sprintf(&buf[end], "%s ", svc->conf.sched_name);
		end=strlen(buf);

    	sprintf(&buf[end], "%u.%u.%u.%u:%d ", NIPQUAD(svc->conf.addr), ntohs(svc->conf.port));
		end=strlen(buf);

		//sprintf(&buf[end], "%d ", svc->conf.timeout);
		//end=strlen(buf);

    	//sprintf(&buf[end], "Start Server: %d ", svc->conf.startservers);
		//end=strlen(buf);

    	//sprintf(&buf[end], "Max Servers: %d ", svc->conf.maxSpareServers);
		//end=strlen(buf);

    	//sprintf(&buf[end], "Min Servers: %d ", svc->conf.minSpareServers);
		//end=strlen(buf);

    	//sprintf(&buf[end], "Max Clients: %d ", svc->conf.maxClients);
		//end=strlen(buf);

    	//sprintf(&buf[end], "Keep Alive: %-d\t", svc->conf.keepAlive);
		//end=strlen(buf);

    	//sprintf(&buf[end], "Max Keepalive Req: %-d\t", svc->conf.maxKeepAliveRequests);
		//end=strlen(buf);

    	//sprintf(&buf[end], "Keepalive Timer: %-d\t", svc->conf.keepAliveTimeout);
		//end=strlen(buf);

    	sprintf(&buf[end], "%s ",
		svc->conf.front_ssl_flags?"on":"off");
		end=strlen(buf);

    	//sprintf(&buf[end], "%s\t",
		//svc->conf.back_ssl_flags?"on":"off");

		sprintf(&buf[end], "%d ", atomic_read(&svc->conns));
		end=strlen(buf);

		//sprintf(&buf[end], "%-d\t", svc->num_dests);
		//end=strlen(buf);

		//sprintf(&buf[end], "Running: %-d\t", atomic_read(&svc->running));
		//end=strlen(buf);

		//sprintf(&buf[end], "PMI: %s\t", svc->conf.pmi?"on":"off");
		//end=strlen(buf);

		//sprintf(&buf[end], "PMI Sysname: %s\t", svc->conf.pmi_sysname);
		//end=strlen(buf);

		//sprintf(&buf[end], "Forward table: %s\t", svc->conf.dynamic_dst?"on":"off");
		//end=strlen(buf);

    	ll = &svc->destinations;
    	for (e = ll->next; e != ll; e = e->next)
		{
        	dest = list_entry(e, tcp_vs_dest_t, n_list);
    		sprintf(&buf[end], "%u.%u.%u.%u:%d\n", NIPQUAD(dest->addr), ntohs(dest->port));
			end=strlen(buf);
    	}

		count++;
	}

	return end;
}


// kevin, 08/05/2006
static int aosAppProxyUser_procRead(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	struct tcp_vs_service *svc;
	struct tcp_vs_conn * conn;
	struct list_head *l;
	struct list_head *lc;
	int end = 0;

	if(offset>0)
		return 0;

	//sprintf(&buf[end], "------------------------- The App Proxy User List ------------------------\n", atomic_read(&aos_tcpvs_curconns));
	//end=strlen(buf);
	// for keyou HAC
	//sprintf(&buf[end], "Name\t\tProxy-Name\tLogin-Time\n");
	//end=strlen(buf);
	//sprintf(&buf[end], "--------------------------------------------------------------------------\n");
	//end=strlen(buf);

	list_for_each(l, &tcp_vs_svc_list)
	{
		svc = list_entry(l, struct tcp_vs_service, list);

		list_for_each(lc, &svc->connections)
		{
			conn = list_entry(lc, struct tcp_vs_conn, n_list);
    		sprintf(&buf[end], "%s ", conn->proxy_username);
			end=strlen(buf);

    		sprintf(&buf[end], "%s ", svc->ident.name);
			end=strlen(buf);

    		sprintf(&buf[end], "%s\n", aos_LogChgTime2Str(conn->proxy_logintime));
			end=strlen(buf);
			if (end > (1024 * 4 - 100))   // the proc file limit is 1024*4
			{
				return end;
			}
		}
	}

	return end;
}


static int aosAppProxy_registerProc(void)
{
	create_proc_read_entry("app_proxy",0444,proc_net,aosAppProxy_procRead,0);
	return 0;
}


static int aosAppProxy_unregisterProc(void)
{

	remove_proc_entry("app_proxy",proc_net);
	return 0;
}


static int aosAppProxyUser_registerProc(void)
{
	create_proc_read_entry("app_proxy_user",0444,proc_net,aosAppProxyUser_procRead,0);
	return 0;
}


static int aosAppProxyUser_unregisterProc(void)
{
	remove_proc_entry("app_proxy",proc_net);
	return 0;
}


// end kevin

//add function my_atoi by WangLei
static long int my_atoi(char *str)
{
	long int sum=0;
	while(*str && *str>='0'&&*str<='9')
		sum=sum*10+(*(str++)-'0');
        sum*=60;
	return sum;
}

//end my_atoi

static int tcp_vs_authentication_authorize( struct socket * sock, struct tcp_vs_conn *conn)
{
	char buf[PROXY_USERNAME_LEN*3], ipstr[24];
	int len=PROXY_USERNAME_LEN*3-1,ret;
	struct  sockaddr_in client_addr;

	DECLARE_WAITQUEUE( wait, current );

	conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,&ret,1);
        aos_LogGetIPStr( &client_addr, ipstr );
        sprintf(buf,"2 %s %s %s %s\r\n",conn->proxy_username, conn->svc->ident.name,\
                ipstr, aos_LogGetSessionID(conn->session_id) );
	tcp_vs_sendbuffer( sock, buf ,strlen(buf),0);
	aos_trace("authenticat send command %s",buf);
	while(
			skb_queue_empty(&(sock->sk->sk_receive_queue))
			&&
//			(
			sock->sk->sk_state==TCP_ESTABLISHED
				)
//			sock->sk->sk_state==TCP_CLOSE_WAIT) &&
	{
//		aos_trace("authenticate went to sleep...");
		add_wait_queue(sock->sk->sk_sleep, &wait);
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ);
		current->state = TASK_RUNNING;
		remove_wait_queue(sock->sk->sk_sleep, &wait);
	}


	aos_trace("authenticat recving...");
	len = tcp_vs_recvbuffer( sock, buf, len, 0 );
	buf[len] = '\0';

	if(len<2)
	{
		aos_trace("authenticte return len=%d",len);
		goto fail;
	}
	aos_trace("authenticte return buf=%s",buf);
	if(strncmp("b",buf,1)==0||strncmp("c",buf,1)==0)
	{
		conn->session_time=my_atoi(buf+2);
		return 1;
	}
fail:
	tcp_vs_constuct_auth_fail_infor( conn, &conn->auth_fail_infor, buf );
	return 0;

}

static int tcp_vs_authentication_password( struct socket * sock, struct tcp_vs_conn *conn)
{
	char buf[PROXY_USERNAME_LEN*3], ipstr[24];
	int len=PROXY_USERNAME_LEN*3-1,ret;
	struct  sockaddr_in client_addr;

	DECLARE_WAITQUEUE( wait, current );

	conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,&ret,1);
        aos_LogGetIPStr( &client_addr, ipstr );
	sprintf(buf,"1 %s %s %s %s\r\n",conn->proxy_username, conn->proxy_password,\
		ipstr, aos_LogGetSessionID(conn->session_id) );
	tcp_vs_sendbuffer( sock, buf ,strlen(buf),0);
	aos_trace("authenticat send command %s",buf);
	while(
			skb_queue_empty(&(sock->sk->sk_receive_queue))
			&&
//			(
			sock->sk->sk_state==TCP_ESTABLISHED
				)
//			sock->sk->sk_state==TCP_CLOSE_WAIT) &&
	{
//		aos_trace("authenticate went to sleep...");
		add_wait_queue(sock->sk->sk_sleep, &wait);
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ);
		current->state = TASK_RUNNING;
		remove_wait_queue(sock->sk->sk_sleep, &wait);
	}


	aos_trace("authenticat recving...");
	len = tcp_vs_recvbuffer( sock, buf, len, 0 );
	buf[len] = '\0';

	if(len<2)
	{
		aos_trace("authenticte return len=%d",len);
		goto fail;
	}
	aos_trace("authenticte return buf=%s",buf);
	if(strncmp( "a", buf, 1 )==0)
		return 1;
fail:
	tcp_vs_constuct_auth_fail_infor( conn, &conn->auth_fail_infor, buf );
	return 0;

}


int tcp_vs_authentication( struct tcp_vs_conn *conn)
{

	struct socket * sock;
	tcp_vs_dest_t  dest;

	//char buf[PROXY_USERNAME_LEN*3];
	//int len=PROXY_USERNAME_LEN*3-1;
	//DECLARE_WAITQUEUE( wait, current );



	aos_trace("authenticat...");
	dest.addr= htonl(INADDR_LOOPBACK);
	dest.port = htons(1812);
	sock = tcp_vs_connect2dest( &dest );

	if(!sock)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Can not connect to the AM");
		goto fail;
	}

	/*
	sprintf(buf,"1 %s %s\r\n",conn->proxy_username, conn->proxy_password);
	tcp_vs_sendbuffer( sock, buf ,strlen(buf),0);
	aos_trace("authenticat send command %s",buf);
	while( skb_queue_empty(&(sock->sk->sk_receive_queue)) && sock->sk->sk_state==TCP_ESTABLISHED )
	{
		add_wait_queue(sock->sk->sk_sleep, &wait);
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ);
		current->state = TASK_RUNNING;
	}

	len = tcp_vs_recvbuffer( sock, buf, len, 0 );

	if(len<3)
		goto close_fail;

	buf[3]=0;
	if(strcmp("YES",buf)==0)
		goto close_success;
	else
		goto close_fail;

	*/
	if( !tcp_vs_authentication_password( sock, conn ) )
	{
		goto close_fail;
	}
	aos_trace("authentication password success");

	if( !tcp_vs_authentication_authorize( sock, conn ) )
		goto close_fail;

//close_success:
	aos_trace("authentication authorize success");
	sock_release( sock );
	return 1;
close_fail:
	sock_release( sock );
fail:
	aos_min_log(eAosMD_TcpProxy, "authentication failed");
	return 0;
}

extern int sysctl_tcp_tw_reuse;

static  __init int ktcpvs_init(void)
{
	static int lsKtcpvsInited = 0;

	aos_min_log(eAosMD_TcpProxy, "tcp proxy init");

	tcp_death_row.sysctl_tw_recycle=1;
	sysctl_tcp_tw_reuse=1;

	if (lsKtcpvsInited)
	{
		printk("AOSTCPVS already inited\n");
		return 0;
	}

	lsKtcpvsInited = 1;

	tcp_vs_control_start();

	tcp_vs_slowtimer_init();

	//tcp_vs_srvconn_init();

	(void) kernel_thread(master_daemon, NULL, 0);

	aos_min_log(eAosMD_TcpProxy, "aostcpvs loaded");

	aosAppProxy_registerCli();
	aosAppProxy_registerProc();
	aosAppProxyUser_registerProc();

	return 0;
}


static __exit void ktcpvs_cleanup(void)
{
	//tcp_vs_srvconn_cleanup();


	tcp_vs_slowtimer_cleanup();

	tcp_vs_control_stop();

	sysctl_ktcpvs_unload = 1;

	aosAppProxy_unregisterCli();
	aosAppProxy_unregisterProc();
	aosAppProxyUser_unregisterProc();

	aos_min_log(eAosMD_TcpProxy, "aostcpvs unloaded");
}



module_init(ktcpvs_init);
module_exit(ktcpvs_cleanup);


