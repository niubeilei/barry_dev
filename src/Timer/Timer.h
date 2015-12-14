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
// 2011/06/01	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Timer_Timer_h
#define AOS_Timer_Timer_h

#include "Util/Buff.h"
#include "Rundata/Rundata.h"
#include "Timer/TimerTypes.h"

#define AOS_TIMER_FLAG				'a'
#define AOS_TIMER_CANCELED_FLAG		'b'

enum AosTimerOffset
{
	eTimerSize = 150,
	eTimerFlagOffset = 0,
	eTimerExpandOffset = 1,
	eTimerCancelOffset = 5,
	eTimerTypeOffset = 6,
	eTimerSecondOffset = 7,
	eTimerStartTimeOffset = 11,
	eTimerEndTimeOffset = 15,
	eTimerIdOffset = 19,
	eTimerSdocIdOffset = 27,
	eTimerUseridOffset = 35,
	eTimerSiteidOffset = 43,
	//eTimerUDataOffset = 50
	eTimerUDataOffset = 47
};


class AosTimer : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	bool				mCanceled;
	AosTimerType		mTimerType;
	u32 				mSecond;
	u32 				mStartTime;
	u32 				mEndTime;
	u64					mTimerid;
	u64 				mSdocid;

	u64					mUserid;
	u32					mSiteid;
	
	OmnString 			mUData;

public:
	AosTimer();
	AosTimer(char * t);
	AosTimer(const u64 &sdocid,
			 const u32 &startTime,
			 const u32 &second,
			 const AosTimerType type,
			 const AosRundataPtr &rdata,
			 const OmnString &udata);
	~AosTimer();

	bool			setBuff(AosBuff &buff);

	bool			setTimerId(const u32 &offset, u64 &timerid);

	bool			isValid() const {return !mCanceled;}
	AosTimerType	getType() const {return mTimerType;}
	u32				getSecond() const {return mSecond;}
	u32				getStartTime() const {return mStartTime;}
	u32				getEndTime() const {return mEndTime;}
	u64				getTimerId() const {return mTimerid;}
	
	bool			procMemoryTimer(AosRundataPtr &rdata);
	bool			procXmlTimer(AosRundataPtr &rdata);
};

#endif
