////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpSockBridge.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/TcpSockBridge.h"

#include <KernelSimu/in.h>
#include <KernelSimu/jiffies.h>
#include <KernelSimu/string.h>
#include <KernelSimu/net.h>
#include <KernelSimu/sock.h>
#include <KernelSimu/aosKernelDebug.h>

#include "AppProxy/AppProxy.h"
#include "aos/aosKernelAlarm.h"
#include "aos_core/aosSock.h"
#include "aos_core/SockFirewall.h"
#include "aos_core/aosSocket.h"
#include "KernelUtil/MgdObj.h"
#include "KernelUtil/KernelMem.h"
#include "Servers/AppServer.h"

#ifdef __KERNEL__
#include "linux/module.h"
#endif

// static struct aosSlab *sgSlab = 0;
// static u32 sgMaxSlabEntry = 5000;

int aosTcpSockBridge_init(void)
{
//	sgSlab = aosSlab_constructor("aosTcpSockBridge",
//		sizeof(struct aosTcpSockBridge), sgMaxSlabEntry);
	return 0;
}


int aosTcpSockBridge_create(struct aosSockBridge **theCreated, 
			struct aosAppProxy *proxy) 
{
	// 
	// This function creates an AosTcpSockBridge. 
	//
	int ret;
	struct aosTcpSockBridge *bridge = 0; 
	struct aosAppServer *server = 0;

	*theCreated = 0;

	// Get the application server to which this bridge is to connect
	if ((ret = proxy->getNextServer(proxy, &server)))
	{
		return ret;
	}

	bridge = aosTcpSockBridge_constructor();
	if (!bridge)
	{
		return aosAlarm(eAosAlarmMemErr);
	}

	// 
	// Connect the sock to the remote backend server. 
	//
	aosTcpSockBridge_holdAppServer(bridge, server);
	aosTcpSockBridge_holdAppProxy(bridge, proxy);
	bridge->mLocalAddr = server->mLocalAddr;
	bridge->mLocalPort = 0;

//	if ((ret = aosTcpSockBridge_connect(bridge)))
//	{
//		// 
//		// Failed to connect to the remote server.
//		//
//		aosAppServer_put(server);
//		aosTcpSockBridge_destructor(bridge);
//		return ret;
//	}

	bridge->mLeft = 0;

	aosAppServer_put(server);
	*theCreated = (struct aosSockBridge *)bridge;
	return 0;
}


int aosTcpSockBridge_setLeft(struct aosSockBridge *self, struct sock *sk)
{
	// Hold a refcnt for the sk
	aosKernelAssert(self && sk, eAosAlarmProgErr);

	sock_hold(sk);
	self->mLeft = sk;
	sk->aos_data.type = eAosSockType_Bridged;
	sk->aos_data.data = (void *)self;
	return 0;
}


struct aosTcpSockBridge *aosTcpSockBridge_constructor()
{
	struct aosTcpSockBridge *bridge;

	bridge = (struct aosTcpSockBridge *)
		aosMgdObj_alloc(eAosObjType_TcpSockBridge, sizeof(struct aosTcpSockBridge));
	if (!bridge)
	{
		aosAlarm(eAosAlarmMemErr);
		return 0;
	}

	aosSockBridge_constructor((struct aosSockBridge *)bridge);
	bridge->mType = IPPROTO_TCP;	
	bridge->connect = aosTcpSockBridge_connect;
	bridge->sockConnected = aosTcpSockBridge_connected;
	bridge->sockDisconnected = aosTcpSockBridge_sockClosed;
	atomic_set(&bridge->mRefcnt, 0);
	return bridge;
}


// 
// This function closes a TCP Sock Bridge. It does so by closing both
// socks. After this function, both socks are set to null. There will
// be no way to trace the two socks back. We assume that scok's
// refcnt will take care of the resource freeing. 
//
int aosTcpSockBridge_close(struct aosTcpSockBridge *self)
{
	if (self->mLeft)
	{
		aosSock_release(self->mLeft);
		sock_put(self->mLeft);
		self->mLeft = 0;
	}

	if (self->mRight)
	{
		aosSock_release(self->mRight);
		sock_put(self->mRight);
		self->mRight = 0;
	}

	self->mState = eAosSockBridgeState_Closed;
	self->mTimestamp = jiffies;
	return 0;
}


// 
// This function will close both socks and free other resources.
// 
int aosTcpSockBridge_destructor(struct aosTcpSockBridge *self)
{
	aosTcpSockBridge_close(self);

	if (self->mAppProxy) 		aosAppProxy_put(self->mAppProxy);
	if (self->mSockFirewall)  	aosSockFirewall_put(self->mSockFirewall);

	memset(self, 0, sizeof(struct aosTcpSockBridge));
	return 0;
}


// 
// A TCP socket bridge holds an application proxy. If an AppProxy is held by
// this bridge, this bridge must increment the reference count by one. Similarly
// if it releases an AppProxy, it should decrement the refcnt by one. 
// This function manages that.
//
int aosTcpSockBridge_holdAppProxy(struct aosTcpSockBridge *self, 
			struct aosAppProxy *proxy)
{
	if (!proxy)
	{
		return aosAlarm(eAosAlarm);
	}

	if (self->mAppProxy)
	{
		if (self->mAppProxy == proxy)
		{
			return 0;
		}
		aosAppProxy_put(self->mAppProxy);
	}
	aosAppProxy_hold(proxy);
	self->mAppProxy = proxy;
	return 0;
}


// 
// A TCP socket bridge holds an application server. If an AppServer is held by
// this bridge, this bridge must increment the reference count by one. Similarly
// if it releases an AppServer, it should decrement the refcnt by one. 
// This function manages that.
//
int aosTcpSockBridge_holdAppServer(struct aosTcpSockBridge *self, 
			struct aosAppServer *server)
{
	if (!server)
	{
		return aosAlarm(eAosAlarm);
	}

	if (self->mServer)
	{
		if (self->mServer == server)
		{
			return 0;
		}
		aosAppServer_put(self->mServer);
	}
	aosAppServer_hold(server);
	self->mServer = server;
	return 0;
}

	
int aosTcpSockBridge_connect(struct aosSockBridge *self)
{
	// 
	// The bridge state must be that the left side is in the Established
	// state and the right side is null.
	// 
	// This function creates the right sock and connects it to the remote site 
	// by sending a SYN to it and
	// wait for its SYN-ACK. If the SYN-ACK is received, the TCP stack will 
	// handle it as if it is a normal TCP socket. After ACK is sent, however, 
	// aosTcpSockBridge_connected() will be called. 
	// 
	struct socket *theSocket;
	int ret;

	aosKernelAssert(self->mServer, eAosAlarmProgErr);
	aosKernelAssert(!self->mRight, eAosAlarmProgErr);

	ret = aosSocket_createAndConnect(&theSocket, self->mLocalAddr, 
			self->mLocalPort,
			self->mServer->mRemoteAddr, self->mServer->mRemotePort);
	if (!ret)
	{
		// Right socket created successfully. A SYN should have been sent out.
		self->mRight = theSocket->sk;

		self->mState = eAosSockBridgeState_Connecting;

		// Hold the sock and set the sock AOS data
		sock_hold(theSocket->sk);
		theSocket->sk->aos_data.type = eAosSockType_Bridged;
		theSocket->sk->aos_data.data = self;
		return 0;
	}

	// 
	// Failed to create socket
	//
	return eAosRc_FailedToCreateSocket;
}


int aosTcpSockBridge_connected(struct aosSockBridge *self)
{
	// 
	// This function is called back when mRight sock goes to the Established
	// state. After this function, the bridge can start bridging data. 
	// 
	// There doesn't seem anything we need to do. Simply return.
	//
	self->mState = eAosSockBridgeState_Connected;
	return 0;
}


int aosTcpSockBridge_forward(struct aosTcpSockBridge *self, 
							 struct sock *sock, 
							 struct sk_buff *skb)
{
	// 
	// Call this function when a data packet (skb) is received from the sock
	// 'sock' for the bridge 'self'. This function forwards the data to the
	// other end. 
	// 
	
	struct sock *fsock = (sock == self->mLeft)?self->mRight:self->mLeft;

	if (!self->mLeft || !self->mRight || 
		 (sock != self->mLeft && sock != self->mRight))
	{
		return aosAlarm(eAosAlarm_aosTcpSockBridge_forward);
	}

	// 
	// Now 'fsock' is the sock to which the data should be forwarded. 
	// 
	// (TBD)
	// 
	fsock = fsock;	// This line is to prevent the warning message. 
					// Delete this line when implementing this function.
	return 0;
}


// 
// This function is called when one of the sock is disconnected. 
// 
int aosTcpSockBridge_sockClosed(struct aosSockBridge *self, struct sock *sk)
{
	aosKernelAssert(self, eAosAlarmNullPointer);
	aosKernelAssert(sk, eAosAlarmNullPointer);
	
	if (sk == self->mLeft)
	{
		// The left side is closed. Need to close the right side.
		aosSock_release(self->mRight);	
		sock_put(self->mRight);
		self->mRight = 0;

		sock_put(self->mLeft);
		self->mLeft= 0;
	}
	else if (sk == self->mRight)
	{
		// The right side is closed. Need to close the left side.
		aosSock_release(self->mLeft);
		sock_put(self->mLeft);
		self->mLeft = 0;

		sock_put(self->mRight);
		self->mRight = 0;
	}

	self->mState = eAosSockBridgeState_Closed;
	self->mTimestamp = jiffies;
	return 0;
}
		
	
// 
// This function returns the bridge to the slab.
//
void aosTcpSockBridge_release(struct aosTcpSockBridge *self)
{
	aosTcpSockBridge_destructor(self);
	aosMgdObj_release((struct aosMgdObj *)self);
}

#ifdef __KERNEL__
EXPORT_SYMBOL( aosTcpSockBridge_create );
EXPORT_SYMBOL( aosTcpSockBridge_release );
#endif
