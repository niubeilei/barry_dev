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
// 2013/03/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SysConsole/AllMessages.h"

#include "Thread/Mutex.h"


static OmnMutex sgLock;
static bool sgInited = false;

bool AosSysConsoleRegisterMsgs()
{
	aos_assert_r(!sgInited, false);

	sgLock.lock();
	AosGetCubeMgr()->registerMsgProc(OmnNew AosMsgProcServerOperation());
	sgInited = true;
	sgLock.unlock();
	return true;
}
#endif
