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
// 	09/06/2010	by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_HealthChecker_HealthCheckObj_h
#define Omn_HealthChecker_HealthCheckObj_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosHealthCheckObj : virtual public OmnRCObject
{
public:
	virtual bool healthCheckFailed(const AosHealthCheckReqPtr &request) = 0;
	virtual bool healthCheckFailed(const u64 &transid) = 0;
};
#endif

