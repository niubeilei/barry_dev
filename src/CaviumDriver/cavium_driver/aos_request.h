////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aos_request.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/List.h"

extern struct aos_list_head sgaos_request_list;

extern int aos_init_request(void);

extern int do_request(cavium_device * n1_dev, n1_request_buffer *req, Uint32 *req_id);
	
extern void aos_handle_request(void);

static inline int is_aos_req_queue_empty(void)
{
	return aos_list_empty( &sgaos_request_list );
}
