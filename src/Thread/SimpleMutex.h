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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Thread_SimpleMutex_h 
#define Omn_Thread_SimpleMutex_h

#include "Porting/Mutex.h"


class AosSimpleMutex
{
private:
    OmnMutexType	mLock;

	// The following should not be used.
    AosSimpleMutex & operator = (const AosSimpleMutex &);
    AosSimpleMutex(const AosSimpleMutex &);

public:
	AosSimpleMutex();
    ~AosSimpleMutex();

    bool 			lock();
    bool			unlock();
};

#endif

