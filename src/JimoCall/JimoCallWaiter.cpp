////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2014/12/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/JimoCallWaiter.h"

#include "JimoCall/JimoCall.h"
#include "SEInterfaces/JimoCaller.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Sem.h"

//static __thread OmnMutex *tsLock = 0;
//static __thread OmnCondVar *tsCondVar = 0;
static __thread OmnSem * tsSem = 0;

/*
void
AosJimoCallWaiter::wait(
		AosJimoCall &jimo_call,
		const int timer_sec)
{
	if (!tsLock)
	{
		tsLock = OmnNew OmnMutex();
		tsCondVar = OmnNew OmnCondVar();
	}
		
	tsLock->lock();
	jimo_call.setWaitLock(tsLock, tsCondVar);

	bool is_timeout;
	int start_sec = OmnTime::getSecTick();
	int delta = OmnTime::getSecTick() - start_sec;
	int crt_sec = 0;
	while (!jimo_call.isCallFinished() && delta < timer_sec)
	{
		tsCondVar->timedWait(*tsLock, is_timeout, 1, 0);
		crt_sec = OmnTime::getSecTick();
		delta = crt_sec - start_sec;
		OmnScreen << "Tick: " << delta << endl;
	}

	tsLock->unlock();
	if (jimo_call.isCallFinished())
	{
		if (jimo_call.isCallSuccess())
		{
			OmnScreen << "Call Success" << endl;
		}
		else
		{
			OmnScreen << "Call Failed" << endl;
		}
	}
	else
	{
		OmnScreen << "Timeout" << endl;
	}
	return;
}
*/


void
AosJimoCallWaiter::setWait(AosJimoCall &jimo_call)
{
	if (!tsSem)
	{
		tsSem = OmnNew OmnSem(0);
	}
		
	jimo_call.setSem(tsSem);
}


void
AosJimoCallWaiter::wait(
		AosJimoCall &jimo_call,
		const int timer_sec)
{
//	if (!tsSem)
//	{
//		tsSem = OmnNew OmnSem(0);
//	}

	// Chen Ding, 2015/08/11
	// OmnSem *sem = jimo_call.getSem();
	// if (!sem)
	// {
	// 	sem = tsSem;
	// 	jimo_call.setSem(sem);
	// }
//	OmnSem *sem = tsSem;
	OmnSem *sem = OmnNew OmnSem(0);
	jimo_call.setSem(sem);

	bool is_timeout;
	int start_sec = OmnTime::getSecTick();
	int delta = OmnTime::getSecTick() - start_sec;
	int crt_sec = 0;
	while (!jimo_call.isCallFinished() && delta < timer_sec)
	{
		sem->timedWait(1000000, is_timeout);
		crt_sec = OmnTime::getSecTick();
		delta = crt_sec - start_sec;
	}

	if (jimo_call.isCallFinished())
	{
		if (jimo_call.isCallSuccess())
		{
			OmnScreen << "Call Succeeded on method:" << jimo_call.getOmnStr(NULL, AosFN::eMethod, "")
					<< " docid:" << jimo_call.getU64(NULL, AosFN::eDocid, 0)
					<< " snap_id:" << jimo_call.getU64(NULL, AosFN::eSnapID, 0) << endl;
		}
		else
		{
			OmnScreen << "Call Failed on method:" << jimo_call.getOmnStr(NULL, AosFN::eMethod, "")
					<< " docid:" << jimo_call.getU64(NULL, AosFN::eDocid, 0)
					<< " snap_id:" << jimo_call.getU64(NULL, AosFN::eSnapID, 0) << endl;
			jimo_call.dump();	//by white, 2015-08-14 17:30
		}
	}
	else
	{
		OmnScreen << "Timeout" << endl;
	}
	return;
}

