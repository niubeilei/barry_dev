////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: if.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_aos_KernelSimu_module_h
#define Omn_aos_KernelSimu_module_h

#ifndef __KERNEL__

#define EXPORT_SYMBOL(x)  
#define MODULE_LICENSE(x)
#define MODULE_AUTHOR(x)
#define __init
#define __exit

#define module_init(x) int __init_module_12345 = (x())
#define module_exit(x) 

#define THIS_MODULE 0
#else
#include <linux/module.h>
#endif

#endif

