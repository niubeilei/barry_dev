//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: qos_proc.h
// Description:
//   
//
// Modification History:
//		created 01/24/2007
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __QOS_PROC_H__
#define __QOS_PROC_H__

#ifdef __KERNEL__
#include <linux/proc_fs.h>
#endif

typedef struct proc_show_buf{
	char *buf;
	int offset;
	int size;
}proc_show_buf_t;

extern int ros_qos_register_proc(void);

extern void ros_qos_unregister_proc(void);

#endif // #ifdef __QOS_PROC_H
