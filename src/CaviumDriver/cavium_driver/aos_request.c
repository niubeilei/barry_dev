////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aos_request.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "linux/spinlock.h"
#include "linux_sysdep.h"
#include "cavium_kernel_common.h"
#include "cavium.h"
#include "request_manager.h"
#include "aosUtil/List.h"
#include "aosUtil/Tracer.h"

struct aos_list_head	sgaos_request_list;
static spinlock_t sglock_request;

extern int query_pending_entry(cavium_device *n1_dev, Uint32 *req_id);

int aos_init_request(void)
{
	AOS_INIT_LIST_HEAD( &sgaos_request_list );
	spin_lock_init( &sglock_request );
	return 0;
}

int do_request(cavium_device * n1_dev, n1_request_buffer *req, Uint32 *req_id)
{
	aos_trace("do aos cavium request");
	if ( query_pending_entry(n1_dev, req_id) )
	{
		return do_cavium_request( n1_dev, req, req_id ); 
	}
//aos_min_log( eAosMD_Platform, "reach the max cavium card requests 2000" );
	req->n1_dev = n1_dev;

	spin_lock( &sglock_request );
	
	aos_list_add_tail( &req->aos_list, &sgaos_request_list );
		
	spin_unlock( &sglock_request );   
	return 0;
}

void aos_handle_request(void)
{
	n1_request_buffer *req_this, *req_next;
		
	spin_lock( &sglock_request );
	
	aos_list_for_each_entry_safe( req_this, req_next, &sgaos_request_list, aos_list ) 
	{
		if ( query_pending_entry( req_this->n1_dev, &req_this->req_id ) )
		{
			aos_list_del( &req_this->aos_list );
			do_cavium_request( req_this->n1_dev, req_this, &req_this->req_id );
		}
		else
		{
			break;
		}
		
	}
		
	spin_unlock( &sglock_request );   
}

