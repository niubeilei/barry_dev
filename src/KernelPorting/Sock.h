////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Sock.h
// Description:
//	
//	'nonagle'
//  There is a major difference between 2.4.30 and 2.6.7+. In 2.4.30, 
//  'nonagle' is defined in 'struct tcp_opt'. In 2.4.30, this is defined
//  as a member of the union 'tp_pinfo'. In 2.6.7+, it is defined in 
//  'struct tcp_sock'.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelPorting_Sock_h
#define Omn_KernelPorting_Sock_h

#ifdef AOS_KERNEL_2_6_15_5
#define aosSock_Reuse(x) (x)->sk_reuse
#define aosSock_State(x) (x)->sk_state
#define aosSock_CallbackLock(x) (x)->sk_callback_lock
#define aosSock_Socket(x) (x)->sk_socket
#define aosSock_Sleep(x) (x)->sk_sleep
#define aosSock_Nonagle(x) tcp_sk(x)->nonagle
#define aosSock_DataReady(x) (x)->sk_data_ready
#define aosSock_WriteQueue(x) (x)->sk_write_queue
#define aosSock_ForwardAlloc(x) (x)->sk_forward_alloc
#define aosSock_ReceiveQueue(x) (x)->sk_receive_queue
#define aosSock_TcpOpt(x) ((struct tcp_sock *)(x))->tcp
#elif defined(AOS_KERNEL_2_6_7)
#define aosSock_Reuse(x) (x)->sk_reuse
#define aosSock_State(x) (x)->sk_state
#define aosSock_CallbackLock(x) (x)->sk_callback_lock
#define aosSock_Socket(x) (x)->sk_socket
#define aosSock_Sleep(x) (x)->sk_sleep
#define aosSock_Nonagle(x) tcp_sk(x)->nonagle
#define aosSock_DataReady(x) (x)->sk_data_ready
#define aosSock_WriteQueue(x) (x)->sk_write_queue
#define aosSock_ForwardAlloc(x) (x)->sk_forward_alloc
#define aosSock_ReceiveQueue(x) (x)->sk_receive_queue
#define aosSock_TcpOpt(x) ((struct tcp_sock *)(x))->tcp
#elif defined(AOS_KERNEL_2_6_11)
#define aosSock_Reuse(x) (x)->sk_reuse
#define aosSock_State(x) (x)->sk_state
#define aosSock_CallbackLock(x) (x)->sk_callback_lock
#define aosSock_Socket(x) (x)->sk_socket
#define aosSock_Sleep(x) (x)->sk_sleep
#define aosSock_Nonagle(x) tcp_sk(x)->nonagle
#define aosSock_DataReady(x) (x)->sk_data_ready
#define aosSock_WriteQueue(x) (x)->sk_write_queue
#define aosSock_ForwardAlloc(x) (x)->sk_forward_alloc
#define aosSock_ReceiveQueue(x) (x)->sk_receive_queue
#define aosSock_TcpOpt(x) ((struct tcp_sock *)(x))->tcp
#elif defined(AOS_KERNEL_2_4_30)
#define aosSock_Reuse(x) (x)->reuse
#define aosSock_State(x) (x)->state
#define aosSock_CallbackLock(x) (x)->callback_lock
#define aosSock_Socket(x) (x)->socket
#define aosSock_Sleep(x) (x)->sleep
#define aosSock_Nonagle(x) (x)->tp_pinfo.af_tcp.nonagle
#define aosSock_DataReady(x) (x)->data_ready
#define aosSock_WriteQueue(x) (x)->write_queue
#define aosSock_ForwardAlloc(x) (x)->forward_alloc
#define aosSock_ReceiveQueue(x) (x)->receive_queue
#define aosSock_TcpOpt(x) ((x)->tp_pinfo.af_tcp)
#else
#define aosSock_Reuse(x) Please Define AOS_KERNEL_x_y_z
#define aosSock_State(x) Please Define AOS_KERNEL_x_y_z
#define aosSock_CallbackLock(x) Please Define AOS_KERNEL_x_y_z
#define aosSock_Socket(x) Please Define AOS_KERNEL_x_y_z 
#define aosSock_Sleep(x) Please Define AOS_KERNEL_x_y_z
#define aosSock_Nonagle(x) Please Define AOS_KERNEL_x_y_z
#define aosSock_DataReady(x) Please Define AOS_KERNEL_x_y_z
#define aosSock_WriteQueue(x)
#define aosSock_ForwardAlloc(x)
#define aosSock_ReceiveQueue(x)
#define aosSock_TcpOpt(x)
#endif

#endif

