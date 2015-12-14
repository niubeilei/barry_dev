////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpSockBridge.h
// Description:
//    The states are:
//		eAosLeftEstablishing
//		eAosLeftEstablished
//		eAosRightEstablishing
//		eAosBothEstablished
//		eAosTearingDown
//		eAosClosed  
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_TcpSockBridge_h
#define aos_core_TcpSockBridge_h

#include <KernelSimu/types.h>
#include "aos_core/aosSockBridge.h"

struct aosSock;
struct aosSockFw;
struct aosAppProxy;

struct aosTcpSockBridge
{
	aosSockBridgeMemberDecl;
};

extern int aosTcpSockBridge_create(struct aosSockBridge **theCreated, 
				struct aosAppProxy *proxy); 
extern int aosTcpSockBridge_connect(struct aosSockBridge *bridge);
extern struct aosTcpSockBridge *aosTcpSockBridge_constructor(void);
extern int aosTcpSockBridge_holdAppProxy(struct aosTcpSockBridge *self, 
			struct aosAppProxy *proxy);
extern int aosTcpSockBridge_holdAppServer(struct aosTcpSockBridge *self, 
			struct aosAppServer *server);
extern int aosTcpSockBridge_destructor(struct aosTcpSockBridge *self);
extern int aosTcpSockBridge_connected(struct aosSockBridge *self);
extern int aosTcpSockBridge_sockClosed(struct aosSockBridge *self, struct sock *sk);
extern void aosTcpSockBridge_release(struct aosTcpSockBridge *self);
extern int  aosTcpSockBridge_setLeft(struct aosSockBridge *self, struct sock *sk);

static inline void aosTcpSockBridge_hold(struct aosTcpSockBridge *self)
{
	atomic_inc(&self->mRefcnt);
}


static inline void aosTcpSockBridge_put(struct aosTcpSockBridge *self)
{
	if (atomic_dec_and_test(&self->mRefcnt))
	{
		aosTcpSockBridge_release(self);
	}
}


#endif

