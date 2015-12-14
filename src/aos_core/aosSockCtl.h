////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSockCtl.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosSockCtl_h
#define aos_core_aosSockCtl_h

#include "KernelSimu/compiler.h"


/*
enum
{
	// AOS_SOCK_OPT_BASE = 3264,

	// AOS_SO_KAPI = AOS_SOCK_OPT_BASE,
	AOS_SO_KAPI = 3264,
	AOS_SO_INIT_AOS = AOS_SO_KAPI+1,

	AOS_SOCK_OPT_MAX = AOS_SO_INIT_AOS
};
*/

#ifndef AOS_SO_KAPI
#define AOS_SO_KAPI 80
#endif

#ifndef AOS_SO_INIT_AOS
#define AOS_SO_INIT_AOS 81
#endif


struct socket;
extern int aosSockCtl_register(void);
extern void aosSockCtl_unregister(void);
extern int aosSockSetopt(struct socket *sock, 
 				  int cmd,
       			  char __user *user, 
	 			  unsigned int optlen);

#endif

