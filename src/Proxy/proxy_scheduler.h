////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_scheduler.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef PROXY_SCHEDULER_H
#define PROXY_SCHEDULER_H

#include "proxy.h"

extern struct list_head proxy_schedulers;

int proxy_bind_scheduler(struct ProxyService *svc, struct proxy_scheduler *scheduler);
int proxy_unbind_scheduler(struct ProxyService *svc);

struct proxy_scheduler *proxy_sched_getbyname(const char *sched_name);
int register_proxy_scheduler(struct proxy_scheduler *scheduler);
int unregister_proxy_scheduler(struct proxy_scheduler *scheduler);



#endif


