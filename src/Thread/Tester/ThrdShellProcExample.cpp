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
#include "Thread/Tester/ThrdShellProcExample.h"

#include "Thread/ThreadShell.h"
#include "Thread/Tester/ThrdTransExample.h"
#include "Thread/Tester/Ptrs.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"


AosThrdShellProcExample::AosThrdShellProcExample()
{
}


AosThrdShellProcExample::~AosThrdShellProcExample()
{
}


bool 
AosThrdShellProcExample::threadShellProc(const OmnThreadShellPtr &shell)
{
	// 
	// This is an example of ThreadShellProc. 
	//
	OmnTrace << "start the threadShellProcExample(...) " << endl;
	mFinished = false;
	AosThrdTransExamplePtr trans = OmnNew AosThrdTransExample(10);

	if (shell->waitForMsg(trans, 2))
	{
		OmnTrace << "Received a message" << endl;
	}
	else
	{
		OmnTrace << "Failed to receive a message" << endl;
	}

	threadFinished();
	return true;
}

