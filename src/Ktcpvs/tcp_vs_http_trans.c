////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_http_trans.c
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
 * tcp_vs_http_trans.c: KTCPVS HTTP transport engine
 *
 * Version:	$Id: tcp_vs_http_trans.c,v 1.2 2015/01/06 08:57:50 andy Exp $
 *
 * Authors:	Wensong Zhang <wensong@linuxvirtualserver.org>
 *		Hai Long <david_lung@yahoo.com>
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 */

#include <linux/net.h>
#include <linux/sched.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <asm/uaccess.h>
#include <linux/ctype.h>
#include <linux/tcp.h>

#include "Ktcpvs/tcp_vs.h"
#include "Ktcpvs/tcp_vs_http_parser.h"
#include "Ktcpvs/tcp_vs_http_trans.h"
#include "KernelPorting/Sock.h"


/****************************************************************************
*	Relay data between source socket and destination socket
*
*	Parameters:
*	  dsock		destination socket
*	  ctl_blk	read control block with source socket
*	  len		relay data length
*/
static int
relay_http_data(struct socket *dsock,
		http_read_ctl_block_t * ctl_blk, int len)
{
	int nbytes, reads, w = 0;
	int ret = -1;

	DECLARE_WAITQUEUE(wait, current);

	EnterFunction(5);

	assert(ctl_blk->remaining <=
	       (ctl_blk->buf_size - ctl_blk->offset));
	assert(len > 0);

	/* if there is enough data in read buffer */
	nbytes = len - ctl_blk->remaining;
	if (nbytes <= 0) {
		if (tcp_vs_xmit
		    (dsock, ctl_blk->cur_buf->buf + ctl_blk->offset, len,
		     MSG_MORE) < 0) {
			TCP_VS_ERR("Error in xmitting message body\n");
			goto exit;
		}
		ctl_blk->offset += len;
		ctl_blk->remaining -= len;
		goto done;
	}

	/* xmit the remaining bytes */
	if (ctl_blk->remaining > 0) {
		if (tcp_vs_xmit(dsock, ctl_blk->cur_buf->buf + ctl_blk->offset,
				ctl_blk->remaining, MSG_MORE) < 0) {
			TCP_VS_ERR("Error in xmitting remaining bytes\n");
			goto exit;
		}
	}

	do {
		reads =
		    tcp_vs_recvbuffer(ctl_blk->sock, ctl_blk->cur_buf->buf,
				      ctl_blk->buf_size, ctl_blk->flag);
		if (reads == 0) {
			TCP_VS_DBG(5, "Reads 0 bytes while relay\n");
			add_wait_queue(aosSock_Sleep(ctl_blk->sock->sk), &wait);
			__set_current_state(TASK_INTERRUPTIBLE);
			schedule();
			__set_current_state(TASK_RUNNING);
			remove_wait_queue(aosSock_Sleep(ctl_blk->sock->sk), &wait);
			continue;
		}

		if (reads < 0) {
			TCP_VS_ERR("Error in reading while relaying\n");
			goto exit;
		}

		w = MIN(nbytes, reads);

		if (tcp_vs_xmit(dsock, ctl_blk->cur_buf->buf, w, MSG_MORE)
		    < 0) {
			TCP_VS_ERR("Error in relaying bytes\n");
			goto exit;
		}

		nbytes -= w;
	} while (nbytes > 0);

	ctl_blk->offset = w;
	ctl_blk->remaining = reads - w;

	assert(ctl_blk->remaining >= 0);
	assert(ctl_blk->offset < ctl_blk->buf_size);

      done:
	ret = 0;
      exit:
	LeaveFunction(5);
	return ret;
}


/****************************************************************************
*
* http_read_init - init read buffer
*
*/
int
http_read_init(http_read_ctl_block_t * ctl_blk, struct socket *sock)
{
	http_buf_t *buf;
	char *page;

	buf = (http_buf_t *) kmalloc(sizeof(http_buf_t), GFP_KERNEL);
	// page = (char *) get_free_page(GFP_KERNEL);
	page = (char *) get_zeroed_page(GFP_KERNEL);
	if (!buf || !page) {
		TCP_VS_ERR("Out of memory.\n");
		return -1;
	}
	INIT_LIST_HEAD(&buf->b_list);
	buf->buf = page;
	buf->data_len = 0;

	INIT_LIST_HEAD(&ctl_blk->buf_entry_list);
	list_add_tail(&buf->b_list, &ctl_blk->buf_entry_list);
	ctl_blk->cur_buf = buf;
	ctl_blk->offset = 0;
	ctl_blk->remaining = 0;
	ctl_blk->info = NULL;
	ctl_blk->sock = sock;
	ctl_blk->buf_size = PAGE_SIZE;
	ctl_blk->flag = 0;

	return 0;
}


/****************************************************************************
*
* http_read_free - free the read buffer.
*
*/
void
http_read_free(http_read_ctl_block_t * read_ctl)
{
	struct list_head *l, *temp;
	http_buf_t *buf_entry;

	list_for_each_safe(l, temp, &read_ctl->buf_entry_list) {
		list_del(l);
		buf_entry = list_entry(l, http_buf_t, b_list);
		free_page((unsigned long) buf_entry->buf);
		kfree(buf_entry);
	}

	return;
}


/****************************************************************************
*
* http_read_line - read a line of http header from socket.
*
*   At first, get the line from the remaining bytes. then read bytes
*   without move. finally, move and get a line.
*
*   Return the len of the line (not including CRLF), or -1 if failed.
*
*   Note:
*	1. http_read_line does not terminate the line with '\0', it
*          still end with CRLF.
*	2. If the line length is too larger, it will failed.
*
*/
int
http_read_line(http_read_ctl_block_t * ctl_blk, int grow)
{
	char *buf, *pos;
	int buf_size, nbytes, i, offset, reads, move;
	int len = -1;

	DECLARE_WAITQUEUE(wait, current);

	EnterFunction(5);

	ctl_blk->info = NULL;

	offset = ctl_blk->offset;
	buf = ctl_blk->cur_buf->buf + offset;
	buf_size = ctl_blk->buf_size;

	/* try to get a line from the remaining bytes */
	for (i = 0; i < ctl_blk->remaining - 1; i++) {
		if ((buf[i] == CR) && (buf[i + 1] == LF)) {
			len = i;
			goto done;
		}
	}

	move = 0;

      get_a_line:
	nbytes = buf_size - ctl_blk->offset - ctl_blk->remaining;

	/* try to read a line from the socket */
	while ((nbytes > 0) && (len < 0)) {
		/* go out if the connection is closed */
		if (aosSock_State(ctl_blk->sock->sk) != TCP_ESTABLISHED
		    && aosSock_State(ctl_blk->sock->sk) != TCP_CLOSE_WAIT) {
			if (len > 0)
				goto done;
			else
				goto exit;
		}

		assert(ctl_blk->remaining <= (buf_size - ctl_blk->offset));

		pos =
		    ctl_blk->cur_buf->buf + ctl_blk->offset +
		    ctl_blk->remaining;
		reads =
		    tcp_vs_recvbuffer(ctl_blk->sock, pos,
				      buf_size - ctl_blk->offset -
				      ctl_blk->remaining, ctl_blk->flag);

		if (reads == 0) {
			TCP_VS_DBG(5,
				   "Read 0 bytes while reading a line\n");
			add_wait_queue(aosSock_Sleep(ctl_blk->sock->sk), &wait);
			__set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(HZ);
			__set_current_state(TASK_RUNNING);
			remove_wait_queue(aosSock_Sleep(ctl_blk->sock->sk), &wait);
			continue;
		}

		if (reads < 0) {
			TCP_VS_ERR("Error in reading a line\n");
			goto exit;
		}

		ctl_blk->remaining += reads;

		/* try to get a line from the remaing bytes */
		for (; i < ctl_blk->remaining - 1; i++) {
			if ((buf[i] == CR) && (buf[i + 1] == LF)) {
				len = i;
				goto done;
			}
		}
		nbytes -= reads;
	}

	/* memmove and read again */
	if ((len < 0) && (move == 0)) {
		char *page;
		if (grow) {
			http_buf_t *hdr;
			hdr =
			    (http_buf_t *) kmalloc(sizeof(http_buf_t),
						   GFP_KERNEL);
			// page = (char *) get_free_page(GFP_KERNEL);
			page = (char *) get_zeroed_page(GFP_KERNEL);
			if (!hdr || !page) {
				TCP_VS_ERR("Out of memory.\n");
				goto exit;
			}
			INIT_LIST_HEAD(&hdr->b_list);
			hdr->buf = page;
			hdr->data_len = 0;
			list_add_tail(&hdr->b_list,
				      &ctl_blk->buf_entry_list);
			ctl_blk->cur_buf = hdr;
		} else {
			page = ctl_blk->cur_buf->buf;
		}
		memmove(page, buf, ctl_blk->remaining);
		ctl_blk->offset = 0;
		buf = page;
		move = 1;
		goto get_a_line;
	} else {
		assert((len < 0) && (move == 1));
		TCP_VS_ERR("Buffer is too small while reading a line.\n");
		goto exit;
	}

      done:
	ctl_blk->info = buf;
	ctl_blk->offset += len + 2;
	ctl_blk->remaining -= len + 2;
	ctl_blk->cur_buf->data_len = ctl_blk->offset;

      exit:
	LeaveFunction(5);
	return len;
}


/****************************************************************************
* relay_multiparts: relay multipart/byteranges body
*
*  relay all data until "CRLF--THIS_STRING_SEPARATES--CRLF" is found.
*
*  Note: there may be a endless loop if the separate string is not found, tbd.
*
*/
static int
relay_multiparts(struct socket *dsock,
		 http_read_ctl_block_t * ctl_blk,
		 http_mime_header_t * mime)
{
	int len, sep_len, l, reads;
	int ret = -1;
	char *buf, *pos;
	char *sep = NULL;

	DECLARE_WAITQUEUE(wait, current);

	EnterFunction(5);

	sep_len = strlen(mime->sep) + 8;
	if ((sep = kmalloc(sep_len + 1, GFP_KERNEL)) == NULL) {
		goto exit;
	}

	snprintf(sep, sep_len + 1, "\r\n--%s--\r\n", mime->sep);

	/* deal with the remaining bytes */
	buf = ctl_blk->cur_buf->buf + ctl_blk->offset;
	len = ctl_blk->remaining;

	if ((len > 0) && (tcp_vs_xmit(dsock, buf, len, MSG_MORE) < 0)) {
		TCP_VS_ERR("Error in xmitting multiparts (remaining)\n");
		goto exit;
	}

	pos = search_sep(buf, len, sep);
	if (pos != NULL) {
		goto done;
	}

	l = MIN(len, sep_len);
	memmove(ctl_blk->cur_buf->buf, buf + len - l, l);

	/* search for CRLF--THIS_STRING_SEPARATES--CRLF */
	while (1) {
		reads =
		    tcp_vs_recvbuffer(ctl_blk->sock,
				      ctl_blk->cur_buf->buf + l,
				      ctl_blk->buf_size - l, 0);
		if (reads == 0) {
			TCP_VS_DBG(5, "Reads 0 bytes while relaying "
				   "multiparts\n");
			add_wait_queue(aosSock_Sleep(ctl_blk->sock->sk), &wait);
			__set_current_state(TASK_INTERRUPTIBLE);
			schedule();
			__set_current_state(TASK_RUNNING);
			remove_wait_queue(aosSock_Sleep(ctl_blk->sock->sk), &wait);
			continue;
		}

		if (reads < 0) {
			TCP_VS_ERR("Error in receiving multiparts\n");
			goto exit;
		}

		if (tcp_vs_xmit
		    (dsock, ctl_blk->cur_buf->buf + l, reads,
		     MSG_MORE) < 0) {
			TCP_VS_ERR("Error in xmitting multiparts\n");
			goto exit;
		}

		len = l + reads;
		pos = search_sep(ctl_blk->cur_buf->buf, len, sep);
		if (pos != NULL) {
			goto done;
		}

		l = MIN(len, sep_len);
		memmove(ctl_blk->cur_buf->buf,
			ctl_blk->cur_buf->buf + len - l, l);
	}

      done:
	ret = 0;
      exit:
	if (sep) {
		kfree(sep);
	}

	LeaveFunction(5);
	return ret;
}


/****************************************************************************
* transfer http message body.
*
* When a message-body is included with a message, the transfer-length
* of that body is determined by one of the following (in order of
* precedence):
*
* 1. Any response message which "MUST NOT" include a message-body
* (such as the 1xx, 204, and 304 responses and any response to a HEAD
* request) is always terminated by the first empty line after the
* header fields, regardless of the entity-header fields present in the
* message.
*
* 2. If a Transfer-Encoding header field (section 14.41) is present
* and has any value other than "identity", then the transfer-length is
* defined by use of the "chunked" transfer-coding (section 3.6),
* unless the message is terminated by closing the connection.
*
* 3. If a Content-Length header field (section 14.13) is present, its
* decimal value in OCTETs represents both the entity-length and the
* transfer-length. The Content-Length header field MUST NOT be sent if
* these two lengths are different (i.e., if a Transfer-Encoding header
* field is present). If a message is received with both a
* Transfer-Encoding header field and a Content-Length header field,
* the latter MUST be ignored.
*
* 4. If the message uses the media type "multipart/byteranges", and
* the transfer- length is not otherwise specified, then this
* self-delimiting media type defines the transfer-length. This media
* type MUST NOT be used unless the sender knows that the recipient can
* arse it; the presence in a request of a Range header with multiple
* byte-range specifiers from a 1.1 client implies that the client can
* parse multipart/byteranges responses.
*
*      A range header might be forwarded by a 1.0 proxy that does not
*      understand multipart/byteranges; in this case the server MUST
*      delimit the message using methods defined in items 1,3 or 5 of
*      this section.
*
* 5. By the server closing the connection. (Closing the connection
* cannot be used to indicate the end of a request body, since that
* would leave no possibility for the server to send back a response.)
*
* Parameters:
*	dsock		destination socket
*	ctl_blk		read control block
*	mime		HTTP MIME header
*/
int
relay_http_message_body(struct socket *dsock,
			http_read_ctl_block_t * ctl_blk,
			http_mime_header_t * mime)
{
	int ret = -1;

	EnterFunction(5);

	if (mime->transfer_encoding) {
		/*
		 * 19.4.6 Introduction of Transfer-Encoding
		 *
		 * HTTP/1.1 introduces the Transfer-Encoding header field
		 * (section 14.41). Proxies/gateways MUST remove any
		 * transfer-coding prior to forwarding a message via a
		 * MIME-compliant protocol.  A process for decoding the
		 * "chunked" transfer-coding (section 3.6) can be represented
		 * in pseudo-code as:
		 *
		 *   length := 0
		 *   read chunk-size, chunk-extension (if any) and CRLF
		 *   while (chunk-size > 0) {
		 *          read chunk-data and CRLF
		 *          append chunk-data to entity-body
		 *          length := length + chunk-size
		 *          read chunk-size and CRLF
		 *   }
		 *   read entity-header
		 *   while (entity-header not empty) {
		 *          append entity-header to existing header fields
		 *          read entity-header
		 *   }
		 *   Content-Length := length
		 *   Remove "chunked" from Transfer-Encoding
		 */
		int len, chunk_size;
		do {
			len = http_read_line(ctl_blk, 0);
			if (len < 0) {
				TCP_VS_ERR("Error in reading chunk "
					   "size from client\n");
				goto exit;
			}

			if (tcp_vs_xmit
			    (dsock, ctl_blk->info, len + 2, MSG_MORE) < 0) {
				TCP_VS_ERR("Error in xmitting chunk "
					   "size & extension\n");
				goto exit;
			}

			ctl_blk->info[len] = 0;
			chunk_size = get_chunk_size(ctl_blk->info);

			TCP_VS_DBG(5, "Chunked line: %s\n", ctl_blk->info);

			if (chunk_size > 0) {
				if (relay_http_data
				    (dsock, ctl_blk, chunk_size + 2) < 0) {
					TCP_VS_ERR("Error in xmitting "
						   "chunk data\n");
					goto exit;
				}
			}
		} while (chunk_size > 0);

		/* relay the trailer */
		do {
			len = http_read_line(ctl_blk, 0);
			if (len < 0) {
				TCP_VS_ERR("Error in reading trailer.\n");
				goto exit;
			}
			if (tcp_vs_xmit
			    (dsock, ctl_blk->info, len + 2, MSG_MORE) < 0) {
				TCP_VS_ERR("Error in xmitting trailer\n");
				goto exit;
			}
		} while (len != 0);
		ret = 0;
	} else if (mime->content_length) {
		ret =
		    relay_http_data(dsock, ctl_blk, mime->content_length);
	} else if (mime->sep) {
		ret = relay_multiparts(dsock, ctl_blk, mime);
	} else {
		ret = 0;	/* ? */
	}

      exit:
	LeaveFunction(5);
	return ret;
}


/****************************************************************************
*	Is there any data in socket?
*
* return:
*	-1,	Socket error
*	 0,	No data can read from socket
*	 1,	Data available
*/
int
data_available(http_read_ctl_block_t * ctl_blk)
{
	int ret;

	EnterFunction(12);

	if (ctl_blk->remaining == 0) {
		/* check if the connection is closed */
		if (aosSock_State(ctl_blk->sock->sk) != TCP_ESTABLISHED
		    && aosSock_State(ctl_blk->sock->sk) != TCP_CLOSE_WAIT) {
			ret = -1;
			goto out;
		}

		/* Do we have data ? */
		if (skb_queue_empty(&(aosSock_ReceiveQueue(ctl_blk->sock->sk))))
			ret = 0;
		else
			ret = 1;
		goto out;
	} else
		ret = 1;

      out:
	LeaveFunction(12);
	return ret;
}
