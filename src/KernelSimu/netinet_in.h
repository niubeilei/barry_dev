////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: netinet_in.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_netinet_h
#define Omn_aos_KernelSimu_netinet_h

#ifdef AOS_KERNEL_SIMULATE

// inline unsigned int htonl(unsigned int hostlong) {return hostlong;}

#ifdef OMN_PLATFORM_UNIX
#include <netinet/in.h>
#endif


#else
#include <netinet/in.h>
#endif

#endif

