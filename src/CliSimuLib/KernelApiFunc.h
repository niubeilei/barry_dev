////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelApiFunc.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CliSimu_KernelApiFunc_h
#define Aos_CliSimu_KernelApiFunc_h

#include "KernelSimu/compiler.h"


extern int AosKernelApi_setSockopt(int level, 
					int optname,
		    		char __user *optval, 
					int optlen);
#endif

