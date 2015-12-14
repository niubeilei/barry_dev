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
#ifndef Omn_HealthChecker_HealthCheckReq_h
#define Omn_HealthChecker_HealthCheckReq_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosHealthCheckReq : virtual public OmnRCObject
{
private:
	u64		mEntryIdx;

public:
	virtual u64		getTransId() const = 0;

	void setEntryIdx(const u64 &id) {mEntryIdx = id;}
	u64	 getEntryIdx() const {return mEntryIdx;}
};
#endif

