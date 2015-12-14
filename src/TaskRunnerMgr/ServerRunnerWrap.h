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
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskRunnerWrap_ServerRunnerWrap_h
#define AOS_TaskRunnerWrap_ServerRunnerWrap_h

#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "TaskMgr/Ptrs.h"
#include "Util/Buff.h"

class AosServerRunnerWrap :  public OmnRCObject  
{
	OmnDefineRCObject;

private:
	pid_t					mPid;
	int 					mReadFd;
	int 					mWriteFd;

public:
	AosServerRunnerWrap(pid_t &pid, int &fd0 , int &fd1);
	~AosServerRunnerWrap()
	{
	}

	AosBuffPtr readTask();
	int getReadFd();
	pid_t getPid();
	bool reset();
	bool sendMsg(const OmnString &content);
	bool stopServer(const int& value);

};
#endif

