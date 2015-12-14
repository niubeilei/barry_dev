////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosHooks.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_2_6_7_aosHooks_h
#define aos_core_2_6_7_aosHooks_h

struct sk_buff;

// 
// AosHook_001
// Insertion Point: tcp_v4_syn_recv_sock() in ipv4/tcp_ipv4.h
// Comments:
//	A socket for a new connection is created after the completion of
//  three-way handshaking. Before returning, this hook is inserted
//  to check whether the listening socket is for an aosAppProxy. 
//  If yes, we will call its 'createSockBridge(...)' to create 
//  a sockBridge. 
//
#define AosHook_001 												\
if (sk->aos_data.type == eAosSockType_AppProxy)						\
{																	\
	if (!sk->aos_data.data)											\
	{																\
		aosAlarm(eAosAlarm);										\
	}																\
	else															\
	{																\
		int ret;													\
		struct aosSockBridge *bridge;								\
		ret = ((struct aosAppProxy *)sk->aos_data.data)				\
			->createSockBridge(&bridge, (struct aosAppProxy *)		\
				sk->aos_data.data, newsk);							\
		if (ret)													\
		{															\
			aosAlarmInt(eAosAlarm, ret);							\
		}															\
		newsk->aos_data.type = eAosSockType_Bridged;				\
		newsk->aos_data.data = bridge;								\
		aosSockBridge_hold(bridge);									\
	}																\
}

// 
// AosHook_002 No definitions here
// 

// 
// AosHook_003
//
#define AosHook_003(x) (x)->aos_data.type == eAosSockType_AppProxy

// #define AosHook_004 		


// 
// AosHook_005
//
#define AosHook_005			\
	struct aosSockData		aos_data;


// 
// AosHook_006
// Kernel created a socket, sent a SYN, and waiting for the SYN-ACK. If it receives
// the SYN-ACK, it changes the sock's state to TCP_ESTABLISHED (in function 
// tcp_rcv_synsent_state_process(...) in ipv4/tcp_input.c. This hook checks
// whether the sock is an aosSockType_Bridged. If yes, it will retrieve the bridge
// and call its 'connected(...)' member function.  
// 
// File: net/ipv4/tcp_input.c
// Function: tcp_rcv_synsent_state_process(...)
// 
#define AosHook_006													\
if (sk->aos_data.type == eAosSockType_Bridged)						\
{																	\
    if (!sk->aos_data.data)											\
    {																\
        aosAlarm(eAosAlarm);										\
    }																\
    else															\
    {																\
		if (!sk->aos_data.data)										\
		{															\
			aosAlarm(eAosAlarm);									\
		}															\
		else														\
		{															\
        	((struct aosSockBridge *)sk->aos_data.data)->			\
			sockConnected((struct aosSockBridge*)sk->aos_data.data);	\
		}															\
    }																\
}


// 
// AosHook_007
// This is the portion of the code to implement the KernelAPI
// 
// File: net/core/sock.c
// Function: setsockopt(...)
// 
// #define AOS_HOOK_007 												



// 
// AosHook_008
// 
// Description:
//	Initialize sock.aos_data.
//
// Function: sock_init_data(...)
// File:     net/core/sock.c
// 
#define AOS_HOOK_008 sk->aos_data.type = 0; sk->aos_data.data = 0;


// 
// AosHook_009
// 
// Description:
//	This is the Network Input PCP. 
//
// Function: netif_receive_skb(...)
// File		 net/core/dev.c
// 
#define AOS_HOOK_009 												\
    if (aosNetworkInputPcpStatus &&									\
        aosNetInputPcp_proc(skb) == eAosRc_PktConsumed)				\
    {																\
        goto out;													\
    }


#endif
