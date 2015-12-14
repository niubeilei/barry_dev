////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelDef.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelUtil_KernelDef_h
#define aos_KernelUtil_KernelDef_h

#ifdef AOS_USERLAND
#include <stdio.h>
#define aosPrintk printf
#else
#define aosPrintk printk
#endif

#endif

