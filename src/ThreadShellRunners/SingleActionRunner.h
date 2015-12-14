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
// 07/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ThreadShellRunners_SingleActionRunner_h
#define Aos_ThreadShellRunners_SingleActionRunner_h

#include "Thread/ThrdShellProc.h"
/*
class AosSingleActionRunner : virtual public OmnThrdShellProc
{
	OmnDefineRCObject;

private:
	const AosDataBlobObjPtr			mBlob;
	const AosBuffArrayPtr			mBuffArray;
	const AosActionObjPtr			mAction;
	const AosRundataPtr				mRundata;
	const AosValueRslt				mUserData;
	int64_t							mMaxErrors;
	const AosActionRunnerListener	mCaller;
	bool							mStatus;

	AosSingleActionRunner(
		const AosDataBlobObjPtr &blob,
		const AosActionObjPtr &action,
		const AosValueRslt &user_data, 
		const int64_t max_errors,
		const AosActionRunnerListener &caller,
		const AosRundataPtr &rdata)
	:
	mBlob(blob),
	mAction(action),
	mRundata(rdata),
	mUserData(user_data),
	mCaller(caller),
	mStatus(false)
	{
	}

	virtual bool        run();
	virtual bool		procFinished();

public:
	static bool runAction(
						const AosDataBlobObjPtr &blob,
						const AosActionObjPtr &action,
						const AosValueRslt &user_data, 
						const int64_t max_errors,
						const AosActionRunnerListener &caller,
						const AosRundataPtr &rdata);
private:
	bool runAction();
};
*/
#endif

