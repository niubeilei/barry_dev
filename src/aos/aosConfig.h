////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosConfig.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosConfig_h
#define aos_core_aosConfig_h

#include <KernelSimu/compiler.h>

extern int aosConfig_config(char __user *optval, int optlen);
extern int aosConfig_saveConfigCore(char __user *optval, int optlen);
extern int aosConfig_loadConfigCore(char __user *optval, int optlen);

#endif
