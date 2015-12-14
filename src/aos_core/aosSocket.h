////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSocket.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosSocket_h
#define aos_core_aosSocket_h

#include <KernelSimu/types.h>

struct socket;
extern int aosSocket_createAndConnectCli(char *data, unsigned int *length);
extern int aosSocket_createAndConnect(struct socket **res, 
					u32 localAddr, 
					u16 localPort,
					u32 remoteAddr, 
					u16 remotePort);
extern int aosSocket_createAndListen(struct socket **res, 
					u32 localAddr, 
					u16 localPort, 
					int backlog);
extern int aosSocket_free(struct socket *sk);
#endif

