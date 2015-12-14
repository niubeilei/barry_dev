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
// 09/23/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_TransProcThrd_h
#define AOS_TransUtil_TransProcThrd_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/RCObjImp.h"
#include "Thread/ThreadedObj.h"
#include "TransBasic/Trans.h"
#include <queue>


class AosTransProcThrd: public OmnThreadedObj 
{
	OmnDefineRCObject;

	enum
	{
		eMaxThrds = 32,
		eDftThrds = 1
	};


	struct StCache
	{
		u32 		thrd_index;
		AosTransPtr trans;
	};


private:
	OmnString 				mName;
	OmnMutexPtr				mLock[eMaxThrds];
	OmnCondVarPtr			mCondVar[eMaxThrds];
	OmnThreadPtr    		mThread[eMaxThrds];
	queue<AosTransPtr> 		mQueue[eMaxThrds]; 
	u32						mNumThrds;

public:
	AosTransProcThrd(const OmnString &name, const u32 num_thrd = eDftThrds);
	~AosTransProcThrd();

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	addRequest(const u32 idx, const AosTransPtr &trans);

	u32		getNumThrds(){return mNumThrds;}

	bool	isBusy();
};

#endif
