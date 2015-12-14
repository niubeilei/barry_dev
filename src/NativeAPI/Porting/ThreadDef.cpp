////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ThreadDef.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Porting/ThreadDef.h"


#ifdef OMN_PLATFORM_UNIX
#include <pthread.h>

#include "Debug/Debug.h"

// int OmnCreateThread(OmnThreadReturnType (*threadFunc)(OmnThreadParmType ),
// 				   void *parm,
// 				   pthread_t &thrdId, 
// 				   const bool high)
OmnThreadIdType OmnCreateThread(
		OmnThreadReturnType (*threadFunc)(OmnThreadParmType ),
		void *parm,
		const bool high)
{
	pthread_t thrdId = 0;
	if (!high)
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr,400000);
		pthread_create(&thrdId, &attr, threadFunc, parm);
	    pthread_detach(thrdId);
		return thrdId;
	}
	else
	{
		OmnTrace << "Create high priority thread" << endl;
		pthread_attr_t attr;
		struct sched_param parms;

		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr,400000);
		pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

		// 
		// Chen Ding, 08/03/2003, 2003-0103
		// Change it to SCHED_RR
		//
		// pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
		// int maxPri = sched_get_priority_max(SCHED_FIFO);
		// int minPri = sched_get_priority_min(SCHED_FIFO);
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		int maxPri = sched_get_priority_max(SCHED_RR);
		int minPri = sched_get_priority_min(SCHED_RR);
		int midPri = (maxPri + minPri) / 2;
		parms.sched_priority = midPri;
		pthread_attr_setschedparam(&attr, &parms);


		pthread_create(&thrdId, &attr, threadFunc, parm);  
	    pthread_detach(thrdId);
		return thrdId;
	}

	return thrdId;
}

void OmnExitThread(void *retval)
{
	pthread_exit(retval);
}

int OmnCancelThread(OmnThreadIdType &t)
{
	return pthread_cancel(t);
}
#elif OMN_PLATFORM_MICROSOFT

#endif
