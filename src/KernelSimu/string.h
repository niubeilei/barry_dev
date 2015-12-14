////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: string.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_string_h
#define Omn_aos_KernelSimu_string_h

#ifdef __KERNEL__
#include <linux/string.h>
#include <linux/ctype.h>

#else
// Userland Portion
#include <string.h>
#include <ctype.h>

#endif

#endif

