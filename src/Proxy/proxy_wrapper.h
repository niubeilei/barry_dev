////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_wrapper.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef PROXY_WRAPPER_H
#define PROXY_WRAPPER_H

#include "proxy_service.h"

#include <linux/kernel.h>
#include <linux/net.h>

#define app_of_wrapper(wo) ((wo)->po->svc->app)
int find_sw_by_name( char * name );

extern AosSockWrapperType_t	* gSockWrapperTypes[MAX_WRAPPER];

int AosProxy_registerWrapperType( AosSockWrapperType_t * wt);
AosSockWrapperType_t *AosProxy_unregisterWrapperType( AosSockWrapperType_t * wt );

/*
int AosProxy_registerSockWrapperCli(void);
int AosProxy_unregisterSockWrapperCli(void);
*/


#endif

