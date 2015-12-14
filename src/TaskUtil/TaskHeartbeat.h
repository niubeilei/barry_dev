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
// 07/27/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_TaskUtil_TaskHeartbeat_h
#define AOS_TaskUtil_TaskHeartbeat_h

#include "Rundata/Ptrs.h"

class AosTaskHeartbeat 
{
	OmnDefineRCObject;

	AosTaskHeartbeat()
	:
	mSvrDeath(false),
	mCallBack(true),
	mCountCheckNum(0),
	mCheckStime(0)
	{
	}

	~AosTaskHeartbeat() {}

public:
	bool 			mSvrDeath;
	bool			mCallBack;
	int				mCountCheckNum;
	int				mCheckStime;

public:
	bool	heartbeatFailed()
	{
		if (mSvrDeath) return true;
		if (mCountCheckNum > 5) return true; 
		return false;
	}

	bool	isCheck()
	{
		u64 crttime = OmnGetSecond(); 
		if (mCallBack) 
		{
			mCountCheckNum++;
			mCheckStime = crttime;
			mCallBack = false;
			return true;
		}

		if (crttime - mCheckStime > 5 * 60)
		{
			mCountCheckNum++;
			mCheckStime = crttime;
			mCallBack = false;
			return true;
		}
		return false;
	}


	void reset()
	{
		mSvrDeath = false;
		mCallBack = true;
		mCountCheckNum = 0;
		mCheckStime = 0;
	}

	void callBack()
	{
		mCallBack = true;
		mCountCheckNum = 0;
	}

	void svrDeath(){mSvrDeath = true;}

};

#endif
#endif

