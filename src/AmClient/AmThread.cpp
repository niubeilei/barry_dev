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
#include "AmClient/AmThread.h"

#include "AmClient/AmApiProc.h"



AosAmThread::AosAmThread(const AosAmApiProcPtr &proc)
:
mProc(proc)
{
}


AosAmThread::~AosAmThread()
{
}


bool	
AosAmThread::threadShellProc(const OmnThreadShellPtr &shell)
{
	// 
	// This thread sends a message to the server and waits for
	// the response.
	//
	return mProc->proc(shell);
}

