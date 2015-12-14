////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSockBridge.h
// Description:
//	A SockBridge contains two socks: mLeft and mRight. In most cases, 
//  mLeft is the initiating sock and mRight is passive one. The initiating
//  sock should be created by receiving SYN. After mLeft is in the 
//  ESTABLISHED state, it will create mRight. After both are in the
//  ESTABLISHED state, data can be transferred between them.
//
//  mSockFw is a socket level firewall. In the current implementation,
//  it is just a dummy structure. 
// 
//  The states are:
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

#ifndef aos_core_aosSockBridge_h
#define aos_core_aosSockBridge_h

#include <KernelSimu/types.h>
#include <KernelSimu/atomic.h>
#include "KernelUtil/MgdObj.h"

struct sock;
struct aosSockFirewall;
struct aosAppProxy;
struct aosList;
struct aosMgdObj;

enum
{
	eAosSockBridgeState_Idle,
	eAosSockBridgeState_LeftCreated,
	eAosSockBridgeState_Connecting,
	eAosSockBridgeState_Connected,
	eAosSockBridgeState_Closing,
	eAosSockBridgeState_Closed,

	eAosSockType_Normal = 0,
	eAosSockType_AppProxy,
	eAosSockType_Bridged
};

#define aosSockBridgeMemberDecl 											\
	aosMgdObjDecl;															\
																			\
	struct sock 			*mLeft;											\
	struct sock				*mRight;										\
	struct aosSockFirewall	*mSockFirewall;									\
	struct aosAppProxy  	*mAppProxy;										\
	struct aosAppServer 	*mServer;										\
	u32						mLocalAddr;										\
	u16						mLocalPort;										\
	u8						mType;											\
	u8						mState;											\
	u64						mTimestamp;										\
	atomic_t				mRefcnt;										\
																			\
	int (*connect)(struct aosSockBridge *self);								\
	int (*sockConnected)(struct aosSockBridge *self);						\
	int (*sockDisconnected)(struct aosSockBridge *self, struct sock *sk);	\
																			\
	u64					mLeftBytesRcvd;										\
	u64					mLeftBytesSent;										\
	u64					mLeftPacketsRcvd;									\
	u64					mLeftPacketsSent;									\
	u64					mLeftPacketsRecvDropped;							\
	u64					mLeftPacketsSendDropped;							\
																			\
	u64					mRightBytesRcvd;									\
	u64					mRightBytesSent;									\
	u64					mRightPacketsRcvd;									\
	u64					mRightPacketsSent;									\
	u64					mRightPacketsRecvDropped;							\
	u64					mRightPacketsSendDropped

struct aosSockBridge
{
	aosSockBridgeMemberDecl;
};

extern int aosSockBridge_constructor(struct aosSockBridge *self);
extern int aosSockBridge_resetStat(struct aosSockBridge *self);

static inline void aosSockBridge_hold(struct aosSockBridge *self)
{
	atomic_inc(&self->mRefcnt);
}

#endif
