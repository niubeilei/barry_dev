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
// Modification History:
// 4/16/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmClient_AmThread_h
#define Aos_AmClient_AmThread_h

#include "AmClient/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosAmThread : public OmnThrdShellProc
{
	OmnDefineRCObject;

private: 
	AosAmApiProcPtr		mProc;

public:
	AosAmThread(const AosAmApiProcPtr &proc);
	~AosAmThread();

	virtual bool	threadShellProc(const OmnThreadShellPtr &shell);
		 
private:
};
#endif

