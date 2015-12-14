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
//
// Modification History:
// 2015/03/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoCallAsync_h
#define Aos_JimoCall_JimoCallAsync_h

#include "JimoCall/JimoCall.h"
#include "Rundata/Ptrs.h"
#include "Util1/TimerObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"

class OmnMutex;

class AosJimoCallAsync : public AosJimoCall,
						public OmnThreadedObj,
						public virtual OmnTimerObj
{
	OmnDefineRCObject;	//this may be a problem

private:
	i64					mCubeID;
	AosJimoCallerPtr	mCaller;
	OmnMutex *			mLock;
	OmnMutex *			mTimerLock;
	bool				mCallSucceeded;
	int					mCallTimeoutThreshold;
	u32					mRetryTimesThreshold;
	u32					mTimeoutCount;
	AosRundataPtr		mRdata;
	OmnThreadPtr        mThread;
	int					mTimerID;
	AosBuffPtr			mBuffInitialSend;	//FIXME:the bson buff's first two bytes are modified somewhere unknown which is
											//unexpected, use this to hold the initial buffer.

public:
	AosJimoCallAsync(
		AosRundata			*rdata,
		const OmnString		&package,
		const i64			cube_id,
		AosClusterObj		*cluster,
		AosJimoCallerPtr	caller = NULL,
		const int			iCallTimeoutThreshold = 10,
		const u32			ulRetryTimesThreshold = 3);

	AosJimoCallAsync(
		AosRundata				*rdata,
		const OmnString			&package,
		const int				func,
		const i64				cube_id,
		const AosJimoCallerPtr	&caller,
		AosClusterObj			*cluster,
		const int				iCallTimeoutThreshold = 10,
		const u32				ulRetryTimesThreshold = 3);

	virtual bool makeCall(AosRundata *rdata);
	virtual bool callResponded(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual void checkTimer(AosRundata *rdata);
	virtual bool waitForFinish();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

	//OmnTimerObj interface
	virtual void timeout(
			const int		timerId,
			const OmnString	&timerName,
			void			*parm);

private:
};
#endif

