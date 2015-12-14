////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_func.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef PROXY_FUNC_H
#define PROXY_FUNC_H

#include "proxy.h"

#include "aosUtil/Memory.h"


int AosProxy_sockWritable( struct sock * sk );
int AosProxy_send(struct sock * sk, char * buffer, int len);
int AosProxy_sendFin( struct sock * sk );
int AosProxy_connect( struct sock ** sk, __u32 ip, __u16 port );
	
int AosProxy_relayData(struct sock * sk);


typedef struct AosBuffer
{
	char	* buffer;
	int		offset;
	int		len;

	struct list_head	list;
}
AosBuffer_t;

#define AosBuffer_empty(x) list_empty(x)

static inline int AosBuffer_add( struct list_head * h, char * buffer, int len, int offset )
{
	AosBuffer_t * ab;

	ab=aos_malloc_atomic( sizeof(*ab) );
	if(!ab)
	{
		BUG();
	}
	
	ab->buffer = buffer;
	ab->len = len;
	ab->offset = offset;
	list_add_tail( &ab->list, h );

	return 0;
}

static inline int AosBuffer_remove( AosBuffer_t * ab )
{
	list_del( &ab->list );
	kfree( ab->buffer );
	kfree( ab );
	return 0;
}

static inline int AosBuffer_destroy( struct list_head * buffer_list )
{
	AosBuffer_t * ab;
	
	while( ! list_empty(buffer_list) )
	{
		struct list_head *l,*n;
		
		list_for_each_safe( l, n, buffer_list)
		{
			ab = list_entry( l, AosBuffer_t, list );

			AosBuffer_remove( ab );
		}
	}

	return 0;
}


static inline int
AosBuffer_send( struct sock * sk, struct list_head * buffer_list )
{
	int s = 0;
	int len_sent;
	AosBuffer_t * ab;
	
	while( ! list_empty(buffer_list) )
	{
		struct list_head *l,*n;
		
		
		list_for_each_safe( l, n, buffer_list)
		{
			ab = list_entry( l, AosBuffer_t, list );

			len_sent = AosProxy_send( sk, &ab->buffer[ab->offset], ab->len );

			if( len_sent == ab->len )
			{
				AosBuffer_remove( ab );
				s += len_sent;
			}
			else if ( len_sent < 0 )
			{
				goto out;
			}
			else
			{
				ab->len -= len_sent;
				ab->offset += len_sent;
				s += len_sent;
				goto out;
			}

		}
	}

out:
	return s;
}


#endif

