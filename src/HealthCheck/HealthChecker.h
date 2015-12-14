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
#ifndef Omn_HealthChecker_HealthChecker_h
#define Omn_HealthChecker_HealthChecker_h

#include "HealthCheck/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include <vector>


class AosHealthChecker : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftFreq = 1,
		eMaxAges = 255,
		eMaxEntries = 1000000
	};

	OmnMutexPtr				mLock;
	OmnThreadPtr			mThread;
	AosHealthCheckObjPtr	mCaller;
	int	*					mCounters;
	u8 *					mAgeIdx;
	int						mNumAges;
	int						mNumEntries;
	int						mCrtAgeIdx;
	int						mFreq;
	vector<AosHealthCheckReqPtr> *	mRequests;

public:
	AosHealthChecker(
		const AosHealthCheckObjPtr &caller, 
		const int freq,
		const int numCounters, 
		const int maxEntries);
	~AosHealthChecker();

	bool	addEntry(const u64 &trans_id);
	bool	addEntry(const AosHealthCheckReqPtr &req);
	bool	removeEntry(const u64 &trans_id);
	bool	removeEntry(const AosHealthCheckReqPtr &req);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

private:
	bool	checkEntries();
	bool	systemFails();
};
#endif

