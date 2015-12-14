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
// 	This is an interface used for 
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Proggie_ThreadRunner_ThreadRunnerObj_h
#define Omn_Proggie_ThreadRunner_ThreadRunnerObj_h

#include "Thread/Ptrs.h"
#include "Util/RCObject.h"


class AosThreadRunnerObj : public virtual OmnRCObject
{

public:
	virtual bool	doThreadFunc(const OmnThreadPtr &thread) = 0;
	virtual void	startHealthCheck() {}
	virtual bool	isThreadHealthy() {return true;}
};
#endif

