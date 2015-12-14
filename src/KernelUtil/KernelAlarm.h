////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelAlarm.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelUtil_KernelAlarm_h
#define Omn_KernelUtil_KernelAlarm_h

// #include <stdio.h>

#ifndef OmnKernelAlarm
#define OmnKernelAlarm(errmsg)	\
	printf("<***** Kernel Alarm *****: %s:%d> %s\n", __FILE__, __LINE__, (errmsg));
#endif

#ifndef OmnKernelSanityCheck
#define OmnKernelSanityCheck(cond)	\
	if ((cond) == 0)				\
	{								\
		printf("<***** Kernel Alarm *****: %s:%d> Sanity Check Failed!\n", __FILE__, __LINE__); \
		return;						\
	}								
#endif


#ifndef OmnKernelSanityCheckInt
#define OmnKernelSanityCheckInt(cond, rslt)	\
	if ((cond) == 0)						\
	{										\
		printf("<***** Kernel Alarm *****: %s:%d> Sanity Check Failed: %d!\n", \
			__FILE__, __LINE__, rslt);		\
		return rslt;						\
	}								
#endif

#endif

