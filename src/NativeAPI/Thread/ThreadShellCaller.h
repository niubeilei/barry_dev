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
// Created By Ken Lee, 2013/04/12	
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Thread_ThreadShellCaller_h
#define Omn_Thread_ThreadShellCaller_h

#include "Util/String.h"


class OmnThreadShellCaller : virtual public OmnRCObject
{
private:
	OmnString		mThreadShellCallerFname;
	int				mThreadShellCallerLine;

public:
	OmnThreadShellCaller(const char *fname, const int line)
	:
	mThreadShellCallerFname(fname),
	mThreadShellCallerLine(line)
	{
	}

	virtual bool	returnShell(const OmnThreadShellPtr &theShell) = 0;
};
#endif

