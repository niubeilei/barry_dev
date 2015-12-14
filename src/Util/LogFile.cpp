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
// 08/31/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/LogFile.h"

#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/String.h"


AosLogFile gAosLogFile("seserver_log", false);

AosLogFile::AosLogFile(const char *fname, const bool flag)
:
mLock(OmnNew OmnMutex()),
mFlag(flag)
{
	if (flag) mFile = ::fopen64(fname, "w+b");
}

