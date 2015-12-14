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

#include "Timer/Timer.h"

#include "SEInterfaces/SmartDocObj.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"


AosTimer::AosTimer()
{
}


AosTimer::AosTimer(char * t)
{
	AosBuff buff(t, eTimerSize, eTimerSize, false AosMemoryCheckerArgs);
	
	buff.setCrtIdx(eTimerFlagOffset);
	char flag = buff.getChar(AOS_TIMER_FLAG);
	if(flag == AOS_TIMER_FLAG)
	{
		buff.setCrtIdx(eTimerCancelOffset);
		flag = buff.getChar(AOS_TIMER_FLAG);
		mCanceled = (flag == AOS_TIMER_CANCELED_FLAG) ? true : false;

		buff.setCrtIdx(eTimerTypeOffset);
		flag = buff.getChar(AOSTIMERTYPE_XML);
		mTimerType = AosTimerTypes::toEnum(flag);

		buff.setCrtIdx(eTimerSecondOffset);
		mSecond = buff.getU32(0);

		buff.setCrtIdx(eTimerStartTimeOffset);
		mStartTime = buff.getU32(0);

		buff.setCrtIdx(eTimerEndTimeOffset);
		mEndTime = buff.getU32(0);
	
		buff.setCrtIdx(eTimerIdOffset);
		mTimerid = buff.getU64(0);
	
		buff.setCrtIdx(eTimerSdocIdOffset);
		mSdocid = buff.getU64(0);

		buff.setCrtIdx(eTimerUseridOffset);
		mUserid = buff.getU64(0);

		buff.setCrtIdx(eTimerSiteidOffset);
		mSiteid = buff.getU32(0);

		buff.setCrtIdx(eTimerUDataOffset);
		buff.getOmnStr(mUData);
	}
}


AosTimer::AosTimer(
		const u64 &sdocid,
		const u32 &startTime,
		const u32 &second,
		const AosTimerType type,
		const AosRundataPtr &rdata,
		const OmnString &udata)
:
mCanceled(false)
{
	mTimerType = type;
	mSecond = second;
	mStartTime = startTime;
	mEndTime = startTime + second;
	mSdocid = sdocid;
	mUserid = rdata->getUserid();
	mSiteid = rdata->getSiteid();
	mUData = udata;
}


AosTimer::~AosTimer()
{
}


bool
AosTimer::setBuff(AosBuff &buff)
{
	if(mCanceled) return false;

	bool rslt = buff.setChar(AOS_TIMER_FLAG);	//check flag
	aos_assert_r(rslt, false);
	
	rslt = buff.setU32(0);					//expand offset
	aos_assert_r(rslt, false);

	rslt = buff.setChar(AOS_TIMER_FLAG);	//cancel flag
	aos_assert_r(rslt, false);

	char type = AosTimerTypes::toChar(mTimerType);
	rslt = buff.setChar(type);
	aos_assert_r(rslt, false);

	rslt = buff.setU32(mSecond);
	aos_assert_r(rslt, false);

	rslt = buff.setU32(mStartTime);
	aos_assert_r(rslt, false);

	rslt = buff.setU32(mEndTime);
	aos_assert_r(rslt, false);

	rslt = buff.setU64(mTimerid);
	aos_assert_r(rslt, false);

	rslt = buff.setU64(mSdocid);
	aos_assert_r(rslt, false);

	rslt = buff.setU64(mUserid);
	aos_assert_r(rslt, false);

	rslt = buff.setU32(mSiteid);
	aos_assert_r(rslt, false);

	rslt = buff.setOmnStr(mUData);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosTimer::setTimerId(const u32 &offset, u64 &timerid)
{
	timerid = offset;
	timerid = timerid << 32;
	timerid += mEndTime;
	mTimerid = timerid;
	return true;
}


bool
AosTimer::procMemoryTimer(AosRundataPtr &rdata)
{
	if(!isValid() || !rdata) return true;

	AosTimerObjPtr caller = rdata->getTimerCaller();
	aos_assert_rr(caller, rdata, false);
	caller->timeout(mTimerid, mUData, rdata);
	return true;
}


bool
AosTimer::procXmlTimer(AosRundataPtr &rdata)
{
	if (mCanceled) return true;

	OmnString timerid;
	timerid << mTimerid;
	rdata->setArg1(AOSARG_TIMER_ID, timerid);
	rdata->setArg1(AOSARG_USERDATA, mUData);
	rdata->setUserid(mUserid);
	rdata->setSiteid(mSiteid);

	//u32 now = OmnGetSecond();
//OmnScreen << "proc timer,timerid" << timerid << ",second:" << mSecond << ",mStartTime:" << mStartTime << ",now:" << now << endl;
	//OmnNotImplementedYet;
	//return false;
	return AosSmartDocObj::procSmartdocStatic(mSdocid, rdata);
}

