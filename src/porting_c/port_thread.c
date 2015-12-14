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
// 02/24/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "porting_c/port_thread.h"


#ifdef OMN_PLATFORM_UNIX
#include <pthread.h>


int aos_create_thread(
		aos_raw_thread_func_t thread_func,
		void *parm,
		pthread_t *thrdId, 
		const int high)
{
	if (!high)
	{
		pthread_attr_t attr;

		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr,200000);
		int rslt = pthread_create(thrdId, &attr, thread_func, parm);
	    pthread_detach(*thrdId);
		return rslt;
	}
	else
	{
		pthread_attr_t attr;
		struct sched_param parms;

		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr,200000);
		pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		int maxPri = sched_get_priority_max(SCHED_RR);
		int minPri = sched_get_priority_min(SCHED_RR);
		int midPri = (maxPri + minPri) / 2;
		parms.sched_priority = midPri;
		pthread_attr_setschedparam(&attr, &parms);

		int rslt = pthread_create(thrdId, &attr, thread_func, parm);  
	    pthread_detach(*thrdId);
		return rslt;
	}

	return *thrdId;
}

void aos_exit_thread(void *retval)
{
	pthread_exit(retval);
}

#elif OMN_PLATFORM_MICROSOFT

#endif
