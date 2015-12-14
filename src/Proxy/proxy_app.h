////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_app.h
// Description:
// This header file is only used by kernel   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////



#ifndef PROXY_APP_IF_H
#define PROXY_APP_IF_H

#include "proxy.h"

extern struct list_head	gAppProxyList;

int AosProxy_registerAppProxy(AppProxy_t *app);
AppProxy_t * AosProxy_unregisterAppProxy(char *name);
AppProxy_t * AosProxy_hasAppType(char *type);

#endif

