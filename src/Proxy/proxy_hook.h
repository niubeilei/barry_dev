////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_hook.h
// Description:
// This header file is only used by kernel   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////



#ifndef PROXY_HOOK_H
#define PROXY_HOOK_H

#include <linux/tcp.h>


typedef struct ProxyHook
{
	int (*proxyAllowFrontConnect)(struct sock *);
	int (*proxyFrontConnectionCreated)(struct sock *);
	int (*proxyBackConnectionCreated)(struct sock *);
	int (*proxyDataReceived)(struct sock *);
	int (*proxyDataAckReceived)(struct sock *);
	int (*proxyFinReceived)(struct sock *);
	int (*proxyResetReceived)(struct sock *);
	int (*proxyConnectionTimeout)(struct sock *);
}
ProxyHook_t;

extern ProxyHook_t 			gProxyHook;
extern kmem_cache_t			*gProxyObjectCachep;
extern struct list_head		gAppProxyList;
#endif

