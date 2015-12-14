////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tcp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Tcp_h
#define Aos_AosUtil_Tcp_h

#ifdef __KERNEL__

#include "KernelUtil/aostcpapi.h"
#ifndef AosTcp_write
//#define AosTcp_write(x, y, z) aos_tcp_sendbuffer((struct socket *)(x), (y), (z))

extern int tcp_vs_sendbuffer(
	struct socket *sock, 
	const char *buffer,
    const size_t length, 
	unsigned long flags);
#define AosTcp_write(x, y, z) tcp_vs_sendbuffer((struct socket *)(x), (y), (z), 0)
#endif

#else // ifdef __KERNEL__

#include <unistd.h>

#ifndef AosTcp_write
#define AosTcp_write(x, y, z) write((x), (y), (z))
#endif

#ifndef AosTcp_read
#define AosTcp_read(x, y, z) read((x), (y), (z))
#endif

#ifndef AosTcp_close
#define AosTcp_close(x) close(x)
#endif


#endif // ifdef __KERNEL__

#endif

