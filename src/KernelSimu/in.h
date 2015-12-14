////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: in.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_in_h
#define Omn_aos_KernelSimu_in_h

// #include "Porting/Socket.h"

#ifdef __KERNEL__
#include <linux/in.h>
#else
// Userland portion
#ifdef OMN_PLATFORM_UNIX
#include <netinet/in.h>
#endif

#ifdef OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#include <windef.h>
#endif

#endif


#endif

