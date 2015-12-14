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
// 03/25/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Proggie_ThreadRunnerWQueue_ThrdRunnerWQObj_h
#define Omn_Proggie_ThreadRunnerWQueue_ThrdRunnerWQObj_h

#include "Thread/Ptrs.h"
#include "Util/RCObject.h"


class AosThrdRunnerWQObj : public virtual OmnRCObject
{
public:
	virtual bool				procRequest(const OmnThreadPtr &thread, 
									const AosThrdRunnerReqPtr &req) = 0;
	virtual AosThrdRunnerReqPtr	createRequest(const OmnThreadPtr &thread) = 0;	
	virtual void				threadNotGood(const OmnThreadPtr &thread) {}
	virtual void				procThreadCreated(const OmnThreadPtr &thread) {}
	virtual void				createThreadCreated(const OmnThreadPtr &thread) {}
	virtual void				procThreadStopped(const OmnThreadPtr &thread) {}
	virtual void				createThreadStopped(const OmnThreadPtr &thread) {}
};
#endif

