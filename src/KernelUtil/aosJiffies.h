////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosJiffies.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelUtil_aosJiffies_h
#define aos_KernelUtil_aosJiffies_h

#ifdef AOS_USERLAND
#include "Porting/LongTypes.h"
extern OmnUint64    jiffies;

#else
// #include <linux/jiffies.h>
extern unsigned long volatile jiffies;

#endif

#endif

