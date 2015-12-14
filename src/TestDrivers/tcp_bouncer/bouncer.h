////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 12/06/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_TestDrivers_tcp_bouncer_bouncer_h
#define aos_TestDrivers_tcp_bouncer_bouncer_h

#include "util/types.h"

extern int aos_tcp_bouncer_create(
		const u32 local_addr, 
		const u16 local_port, 
		const int num_ports);

#endif

