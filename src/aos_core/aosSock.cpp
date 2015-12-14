////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSock.cpp
// Description:
//	AosSock adds special functions/capabilities to struct sock in that
//  when a sock receives a packet, if it is a signal packet, such as 
//  SYN, FIN, ACK, etc., it is processed normally by the built-in 
//  functions. If it is a data packet, however, the data packet is 
//  passed to AosSock. AosSock will then determine how to handle the
//  data packets, such as:
//		1. TCP Proxying: data packets are proxied to another connection
//		2. Content Filtering
//		3. SSL
//		4. IPSec
//		5. Content Manipulation
//		
//	Each is implemented through different types of AosSocks, such as
//	SSL AosSock, IPSec AosSock, etc. 
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosSock.h"

#include <KernelSimu/skbuff.h>
#include <KernelSimu/ip.h>
#include <KernelSimu/udp.h>
// #include <KernelSimu/route.h>
#include <KernelSimu/dev.h>
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/sock.h>


// 
// This function is a modification of inet_release(struct sock *sk)
// This function should be called for those socks that were created
// in the kernel only. The sock should not have anything to do with 
// multicasting. The linger flag is ignored by this function.
// 
int aosSock_release(struct sock *sk)
{
//	sk->sk_prot->close(sk, 0);
	return 0;
}

