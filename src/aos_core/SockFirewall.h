////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SockFirewall.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_SockFirewall_h
#define aos_core_SockFirewall_h

#include <KernelSimu/types.h>
#include "aos_core/aosSockBridge.h"

struct aosSock;

struct aosSockFirewall
{

};


extern int aosSockFirewall_put(struct aosSockFirewall *self);

#endif

