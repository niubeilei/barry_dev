////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSockBridge.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosSockBridge.h"

#include <KernelSimu/skbuff.h>
#include <KernelSimu/ip.h>
#include <KernelSimu/udp.h>
// #include <KernelSimu/route.h>
#include <KernelSimu/dev.h>
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/string.h>


int aosSockBridge_init(void)
{
	return 0;
}


int aosSockBridge_constructor(struct aosSockBridge *self)
{
	// 
	// This function should be called first before the derived object does the
	// initialization. 
	//
	memset(self, 0, sizeof(struct aosSockBridge));
	return 0;
}


int aosSockBridge_resetStat(struct aosSockBridge *self)
{
    self->mLeftBytesRcvd = 0;
    self->mLeftBytesSent = 0;
    self->mLeftPacketsRcvd = 0;
    self->mLeftPacketsSent = 0;
    self->mLeftPacketsRecvDropped = 0;
    self->mLeftPacketsSendDropped = 0;

    self->mRightBytesRcvd = 0;
    self->mRightBytesSent = 0;
    self->mRightPacketsRcvd = 0;
    self->mRightPacketsSent = 0;
    self->mRightPacketsRecvDropped = 0;
    self->mRightPacketsSendDropped = 0;

	return 0;
}

	
// 
// It assumes the right has not been created yet. This function
// creates the right side.
//
/*
int aosSockBridge_createPeer(struct aosSockBridge *self,
							 u32 localAddr, 
							 u16 localPort,
							 u32 remoteAddr,
							 u16 remotePort)
{
	if (self->mState != eAosLeftCreated || mRight)
	{
		aosAlarmInt(eAosRc_aosSockBridge_createPeer1, self->mState);
	}


}


int aosSockBridge_healthCheck(void)
{
	return 0;
}
*/


#if 0
// 
// This is the function to send data for userland process. An skb was received
// from 'fromSk' and we want to forward the skb to 'toSk'. If the skb is forwarded
// or dropped, 0 is returned. Otherwise, 1 is returned. The caller should either 
// queue or drop the skb if 1 is returned.
//
//
int aosSendToPeer(struct sk_buff *skb, 
				  struct sock *fromSk, 
				  struct sock *toSk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct sk_buff *skb;
	int iovlen, flags;
	int mss_now;
	int err, copied;
	long timeo;

	lock_sock(sk);
	TCP_CHECK_TIMER(sk);

	flags = msg->msg_flags;

	// 
	// 'timeo' is the sending timer. If it is non-blocking, or MSG_DONTWAIT, 
	// then it is set to 0. Otherwise, it is set to sk->sk_sendtimeo.
	//
	timeo = sock_sndtimeo(sk, flags & MSG_DONTWAIT);

	//
	// Wait for a connection to finish.
	// 
	// If it is not in the established or wait state, the connection is not
	// ready yet.
	//
	if ((1 << sk->sk_state) & ~(TCPF_ESTABLISHED | TCPF_CLOSE_WAIT))
	{
		// 
		// Wait for the socket to be connected. 
		//
		if ((err = sk_stream_wait_connect(sk, &timeo)) != 0)
		{
			goto out_err;
		}
	}

	//
	// This should be in poll
	//
	clear_bit(SOCK_ASYNC_NOSPACE, &sk->sk_socket->flags);

	mss_now = tcp_current_mss(sk, !(flags&MSG_OOB));

	//
	// Ok commence sending.
	//
	iovlen = msg->msg_iovlen;
	iov = msg->msg_iov;
	copied = 0;

	err = -EPIPE;
	if (sk->sk_err || (sk->sk_shutdown & SEND_SHUTDOWN))
		goto do_error;

	// 
	// Ready to copy the data
	//
	while (--iovlen >= 0) 
	{
		int seglen = iov->iov_len;
		unsigned char __user *from = iov->iov_base;

		iov++;

		while (seglen > 0) 
		{
			int copy;

			skb = sk->sk_write_queue.prev;

			if (!sk->sk_send_head ||
			    (copy = mss_now - skb->len) <= 0) 
			{

new_segment:
				//
				// Allocate new segment. If the interface is SG,
				// allocate skb fitting to single page.
				// 
				// sk_stream_memory_free(sk) checks wether the sk has memory for
				// sending. This is done by checking:
				//		sk->sk_wmem_queued < sk->sk_sndbuf
				// where sk->sk_wmem_queued is the amount of memory allocated for
				// writing and sk_sndbuf is the maximum size. 
				//
				if (!sk_stream_memory_free(sk))
				{
					goto wait_for_sndbuf;
				}

				// 
				// Allocate the skb.
				//
				skb = sk_stream_alloc_pskb(sk, select_size(sk, tp),
							   0, sk->sk_allocation);
				if (!skb)
					goto wait_for_memory;

				//
				// Check whether we can use HW checksum.
				//
				if (sk->sk_route_caps &
				    (NETIF_F_IP_CSUM | NETIF_F_NO_CSUM |
				     NETIF_F_HW_CSUM))
					skb->ip_summed = CHECKSUM_HW;

				skb_entail(sk, tp, skb);
				copy = mss_now;
			}

			//
			// Try to append data to the end of skb.
			//
			if (copy > seglen)
				copy = seglen;

			//
			// Where to copy to?
			//
			if (skb_tailroom(skb) > 0) 
			{
				//
				// We have some space in skb head. Superb!
				//
				if (copy > skb_tailroom(skb))
					copy = skb_tailroom(skb);
				if ((err = skb_add_data(skb, from, copy)) != 0)
					goto do_fault;
			} 
			else 
			{
				int merge = 0;
				int i = skb_shinfo(skb)->nr_frags;
				struct page *page = TCP_PAGE(sk);
				int off = TCP_OFF(sk);

				if (skb_can_coalesce(skb, i, page, off) &&
				    off != PAGE_SIZE) {
					/* We can extend the last page
					 * fragment. */
					merge = 1;
				} else if (i == MAX_SKB_FRAGS ||
					   (!i &&
					   !(sk->sk_route_caps & NETIF_F_SG))) {
					/* Need to add new fragment and cannot
					 * do this because interface is non-SG,
					 * or because all the page slots are
					 * busy. */
					tcp_mark_push(tp, skb);
					goto new_segment;
				} else if (page) {
					/* If page is cached, align
					 * offset to L1 cache boundary
					 */
					off = (off + L1_CACHE_BYTES - 1) &
					      ~(L1_CACHE_BYTES - 1);
					if (off == PAGE_SIZE) {
						put_page(page);
						TCP_PAGE(sk) = page = NULL;
					}
				}

				if (!page) {
					/* Allocate new cache page. */
					if (!(page = sk_stream_alloc_page(sk)))
						goto wait_for_memory;
					off = 0;
				}

				if (copy > PAGE_SIZE - off)
					copy = PAGE_SIZE - off;

				/* Time to copy data. We are close to
				 * the end! */
				err = skb_copy_to_page(sk, from, skb, page,
						       off, copy);
				if (err) {
					/* If this page was new, give it to the
					 * socket so it does not get leaked.
					 */
					if (!TCP_PAGE(sk)) {
						TCP_PAGE(sk) = page;
						TCP_OFF(sk) = 0;
					}
					goto do_error;
				}

				/* Update the skb. */
				if (merge) {
					skb_shinfo(skb)->frags[i - 1].size +=
									copy;
				} else {
					skb_fill_page_desc(skb, i, page, off, copy);
					if (TCP_PAGE(sk)) {
						get_page(page);
					} else if (off + copy < PAGE_SIZE) {
						get_page(page);
						TCP_PAGE(sk) = page;
					}
				}

				TCP_OFF(sk) = off + copy;
			}

			if (!copied)
				TCP_SKB_CB(skb)->flags &= ~TCPCB_FLAG_PSH;

			tp->write_seq += copy;
			TCP_SKB_CB(skb)->end_seq += copy;
			skb_shinfo(skb)->tso_segs = 0;

			from += copy;
			copied += copy;
			if ((seglen -= copy) == 0 && iovlen == 0)
				goto out;

			if (skb->len != mss_now || (flags & MSG_OOB))
				continue;

			if (forced_push(tp)) {
				tcp_mark_push(tp, skb);
				__tcp_push_pending_frames(sk, tp, mss_now, TCP_NAGLE_PUSH);
			} else if (skb == sk->sk_send_head)
				tcp_push_one(sk, mss_now);
			continue;

wait_for_sndbuf:
			set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
wait_for_memory:
			if (copied)
				tcp_push(sk, tp, flags & ~MSG_MORE, mss_now, TCP_NAGLE_PUSH);

			if ((err = sk_stream_wait_memory(sk, &timeo)) != 0)
				goto do_error;

			mss_now = tcp_current_mss(sk, !(flags&MSG_OOB));
		}
	}

out:
	if (copied)
		tcp_push(sk, tp, flags, mss_now, tp->nonagle);
	TCP_CHECK_TIMER(sk);
	release_sock(sk);
	return copied;

do_fault:
	if (!skb->len) {
		if (sk->sk_send_head == skb)
			sk->sk_send_head = NULL;
		__skb_unlink(skb, skb->list);
		sk_stream_free_skb(sk, skb);
	}

do_error:
	if (copied)
		goto out;
out_err:
	err = sk_stream_error(sk, flags, err);
	TCP_CHECK_TIMER(sk);
	release_sock(sk);
	return err;
}
#endif

