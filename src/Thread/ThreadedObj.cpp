////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ThreadedObj.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Thread/ThreadedObj.h"


extern OmnTime *	OmnTimeSelf;

void    	
OmnThreadedObj::heartbeat(const int logicId)
{
	mThreadStatus = false;
};

bool
OmnThreadedObj::postProc()
{
	return false;
}

