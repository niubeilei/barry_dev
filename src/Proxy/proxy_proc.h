////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_proc.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef PROXY_PROC_H
#define PROXY_PROC_H

#include "proxy.h"

#include <linux/proc_fs.h>

void AosProxy_unregisterProc(void);
int AosProxy_registerProc(void);

extern struct proc_dir_entry *proc_proxy;

#endif

