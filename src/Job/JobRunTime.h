////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/01/19 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Job_JobRunTime_h
#define AOS_Job_JobRunTime_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "API/AosApi.h"
#include "BSON/BSON.h"

class AosJobRunTime : virtual public OmnRCObject
{
	OmnDefineRCObject
private:
	OmnMutexPtr			mLock;
	AosBSONPtr			mVariableEnv;

public:
	AosJobRunTime();
	~AosJobRunTime();

	AosValueRslt 	get(
						const OmnString &key,
						const AosRundataPtr &rdata);
	bool 			set(
						const OmnString &key, 
						const AosValueRslt &value,
						const AosRundataPtr &rdata);
};
#endif
