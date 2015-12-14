////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_http_trans.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef _TCP_VS_HTTP_TRANS_H
#define _TCP_VS_HTTP_TRANS_H

/*
 * KTCPVS  -    Kernel TCP Virtual Server
 *
 * tcp_vs_http_trans.h: HTTP transport definition and function prototypes
 *
 * $Id: tcp_vs_http_trans.h,v 1.2 2015/01/06 08:57:50 andy Exp $
 *
 * Authors:	Hai Long <david_lung@yahoo.com>
 *		Wensong Zhang <wensong@linuxvirtualserver.org>
 *
 */


/* buffer to store http lines */
typedef struct http_buf_s {
	struct list_head	b_list;
	char			*buf;
	int			data_len;
} http_buf_t;

/*
 *	Control block to read data from socket
 */
typedef struct http_read_ctl_block_s {
	struct socket *sock;	/* socket that message read from */
	struct list_head buf_entry_list; /* buffer list */
	http_buf_t *cur_buf;	/* current buffer */
	char *info;		/* point to the current information */
	int offset;		/* offset of remaining bytes */
	int remaining;		/* remaining bytes not return */
	int buf_size;		/* buffer size */
	int flag;		/* read flag */
} http_read_ctl_block_t;


/* HTTP transport function prototypes */
extern int relay_http_message_body(struct socket *dsock,
				   http_read_ctl_block_t * ctl_blk,
				   http_mime_header_t * mime);

extern int http_read_init(http_read_ctl_block_t *ctl_blk, struct socket *sock);

extern void http_read_free(http_read_ctl_block_t *read_ctl);

extern int data_available(http_read_ctl_block_t * ctl_blk);

extern int http_read_line(http_read_ctl_block_t * ctl_blk, int grow);

#endif
