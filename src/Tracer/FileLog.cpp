////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/29/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tracer/FileLog.h"

#include "Thread/Mutex.h"
#include "Util/File.h"


AosFileTracer 	AosFileTracer::smLog1;
AosFileTracer 	AosFileTracer::smLog2;
bool 			AosFileTracer::smInited = false;
static OmnMutex sgLock;

AosFileTracer::AosFileTracer()
{
}


bool
AosFileTracer::openFile(const OmnString &name)
{
	mFile = OmnNew OmnFile(name, OmnFile::eCreate AosMemoryCheckerArgs);
	if (!mFile->isGood())
	{
		OmnAlarm << "Failed open the log file: " << name << enderr;
		mFile = 0;
		return false;
	}
	return true;
}


bool 
AosFileTracer::init()
{
	sgLock.lock();
	if (smInited)
	{
		sgLock.unlock();
		return true;
	}
	smLog1.openFile("./file_tracer_1");
	smLog1.openFile("./file_tracer_2");
	smInited = true;
	sgLock.unlock();
	return true;
}

