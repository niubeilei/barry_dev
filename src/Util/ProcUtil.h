////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ProcUtil.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_ProcUtil_h
#define Snt_Util_ProcUtil_h

#include "Porting/Process.h"
#include "Util/String.h"

#include <sys/types.h>
#include <unistd.h>


class OmnProcUtil 
{
public:
	static bool	killProc(const OmnString &name);
	static int 	getProcId(const OmnString &name);
	static bool isProcRunning(const OmnString &name);

	static OmnProcessIdType getProcid()
	{
		// This function gets the process id of this program
		return getpid();
	}

	static OmnProcessIdType getParentProcid()
	{
		return getppid();
	}
};

#endif

