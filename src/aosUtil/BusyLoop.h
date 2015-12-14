////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BusyLoop.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_BusyLoop_h
#define Aos_AosUtil_BusyLoop_h

#ifndef __KERNEL__
# define HZ		1000		/* Internal kernel timer frequency */
#endif
extern int AosSystem_BusyLoopinit(void);

#endif
