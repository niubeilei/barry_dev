////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GenThread.h
// Description:
//   This is a Singleton class.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestDrivers_TrafficGen_GenThread_h
#define Omn_TestDrivers_TrafficGen_GenThread_h

#include "Debug/Debug.h"
#include "Porting/LongTypes.h"
#include "Porting/TimeOfDay.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"
#include "Util1/Ptrs.h"


class OmnGenThread : public virtual OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnThreadPtr    mSecTickThread;

public:
	OmnGenThread();
	~OmnGenThread();

	bool				start();
	bool				stop();

	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, 
							const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 

};

#endif
