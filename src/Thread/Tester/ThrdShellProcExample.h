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
#ifndef Aos_Thread_ThrdShellProcExample_h
#define Aos_Thread_ThrdShellProcExample_h

#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosThrdShellProcExample : public OmnThrdShellProc
{
	OmnDefineRCObject;

private: 

public:
	AosThrdShellProcExample();
	~AosThrdShellProcExample();

	virtual bool threadShellProc(const OmnThreadShellPtr &shell);

private:
};
#endif

