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
// 2010-02-19:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Porting/Process.h"


#ifdef OMN_PLATFORM_UNIX

#include "alarm_c/alarm.h"
#include "Util/File.h"
#include "Util/String.h"

int AosGetPeakMem()
{
	// This function retrieves the current process peak
	// memory. For each process, there is a file:
	// 	/proc/pid/status
	// There is a line in the file:
	// 	VmPeak:	xxxx kB
	// This function retrieves the number 'xxxx'.
	OmnString fname = "/proc/";
	fname << OmnGetCrtPID() << "/status";
	OmnFile ff(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	OmnString ss;
	aos_assert_r(ff.readToString(ss), false);
	int idx = ss.findSubString("VmPeak:", 0);
	aos_assert_r(idx > 0, false);
	const char *data = ss.data();
	int mem = atoi(&data[idx+8]);
	return mem;
}



#elif OMN_PLATFORM_MICROSOFT

#endif
