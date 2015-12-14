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
// Function: tcp_v4_syn_recv_sock() 
// File: ipv4/tcp_ipv4.h
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
// Function: 'tcp_check_req(...)'
// File: ipv4/tcp_minisock.c
// Comments:
// 	If the stack receives a valid ACK for establishing a connection 
// 	to an Application Proxy, the stack should have created a sock 
// 	for the connection. Unlike the normal TCP processing, which 
// 	will put the newly create sock into the listening sock's 
// 	accepted queue, we will not add the newly created sock to 
// 	the accept queue if the listening sock is for an application 
// 	proxy. 
// 
// Definition:
//    if (sk->aos_data.type == eAosSockType_Normal)
//    {
//        tcp_acceptq_queue(sk, req, child);
//    }


// 
// AosHook_003
// Function:	get_cookie_sock(...)
// File:		ipv4/syncookies.c
// Comments:
//	This function is called when the stack receives an ACK 
// 	and the syncookie is on. It has passed all the checking 
// 	that this is a valid ACK. The stack is willing to create 
//  a new connection based on this ACK. This function will actually 
//  create the new sock for the connection. For the same reason
//  as that of AosHook_002, we will not add the new sock to the
//  listening sock's accept queue if it is AOS sockets.
// 
// Definition:
// 	      if (sk->aos_data.type == eAosSockType_Normal)
//        {
//            tcp_acceptq_queue(sk, req, child);
//        }


// 
// AosHook_004
//
// #define AosHook_004 		
// enum 						
// { 							
// 	AOS_SO_KAPI = 80, 		
// 	AOS_SO_INIT_AOS = 81 	
// };


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
#define AOS_HOOK_006 												\
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
