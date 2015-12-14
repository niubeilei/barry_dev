////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_service.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef PROXY_SERVICE_H
#define PROXY_SERVICE_H

#include "proxy.h"

inline int AosProxy_poRelease( struct ProxyObject * po );
inline int AosProxy_poRelease1( struct ProxyObject * po );

int AosProxy_serviceStop(char * name, char *errmsg);
int AosProxy_serviceStart(char * name, char *errmsg);
ProxyService_t *AosProxy_getServiceByName(char *name);
int AosProxy_serviceAdd(char * appname, char * type, int addr, int port, 
		AosSockWrapperType_t * fwt, 
		AosSockWrapperType_t * bwt, 
		struct proxy_scheduler * scheduler,
		char *errmsg);
int AosProxy_rsAdd(char *name, u32 addr, u16 port, int weight, char *errmsg);
int AosProxy_rsRemove(char *name, u32 addr, u16 port, char *errmsg);
int AosProxy_serviceRemove( ProxyService_t * svc );
int __AosProxy_serviceStop(ProxyService_t *svc);

extern struct list_head 	gProxyServiceList;
extern kmem_cache_t		*gProxyObjectCachep;


#endif

