////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ThreadedObj.h
// Description:
//	When a class starts a thread, the class should derive from this class.
//  and implement the member functions defined in this class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#ifndef Omn_Thread_ThreadedObj_h
#define Omn_Thread_ThreadedObj_h

#include "aosUtil/Types.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdStatus.h"
#include "Util/SPtr.h"
#include "Util/RCObject.h"
#include "Util1/Time.h"

class OmnThread;


class OmnThreadedObj : public virtual OmnRCObject
{
protected:
	bool			mThreadStatus;
	int64_t			mHeartbeatStartSec;

public:
	OmnThreadedObj()
	:
	mThreadStatus(true),
	mHeartbeatStartSec(OmnGetSecond())
	{
	}

	virtual ~OmnThreadedObj() {}

	// This is the main body of the thread function
	// If the logic ID is not recognized, it returns false. Otherwise,
	// it should start the thread function, which should not return
	// until the 'state' changes to a non-active state.
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread) = 0;
	virtual bool	signal(const int threadLogicId) = 0;
    virtual void    heartbeat(const int tid); 
    virtual bool    checkThread111(OmnString &err, const int thrdLogicId) {return mThreadStatus;};

    virtual bool    postProc();

};
#endif

