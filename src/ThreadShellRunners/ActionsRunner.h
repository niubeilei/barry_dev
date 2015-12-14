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
#ifndef Aos_ThreadShellRunners_ActionsRunner_h
#define Aos_ThreadShellRunners_ActionsRunner_h

#include "SEInterfaces/Ptrs.h"
#include "ThreadShellRunners/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/ValueRslt.h"
#include<vector>
using namespace std;

class AosActionsRunner : virtual public OmnThrdShellProc 
{
	OmnDefineRCObject; 

public:
	enum ProcDataType
 	{
		eInvalid,

		eDataBlob,
		eDataCacher,
		eBuffArray,

		eMax
	};

private:
	ProcDataType				mDataType;
	int64_t						mStartPos;
	int64_t						mEndPos;
	AosDataBlobObjPtr			mBlob;
	AosDataCacherObjPtr			mCacher;
	AosBuffArrayPtr				mBuffArray;
	vector<AosActionObjPtr>		mActions;
	OmnThrdShellProcPtr			mCaller;
	const AosValueRslt			mUserData;
	AosDataRecordObjPtr			mDataRecord;
	bool						mStatus;
	AosRundataPtr				mRundata;

public:
	AosActionsRunner(
					const int64_t &startPos,
					const int64_t &endPos,
					const AosDataBlobObjPtr &blob,
					const AosDataCacherObjPtr &cacher,
					const AosBuffArrayPtr &buffArray,
					const ProcDataType &type,
					const vector<AosActionObjPtr> &actions,
					const OmnThrdShellProcPtr &caller,
					const AosValueRslt &userData,
					const AosDataRecordObjPtr &record,
					const OmnString &calledClassName,
					const AosRundataPtr &rdata);
	~AosActionsRunner();
	
	virtual bool run();
	virtual bool procFinished();
private:

	bool runActions(
			const AosDataBlobObjPtr &blob,
			const AosRundataPtr &rdata);
	bool runActions(
			const AosDataCacherObjPtr &blob,
			const AosRundataPtr &rdata);
	bool runActions(
			const AosBuffArrayPtr &blob,
			const AosRundataPtr &rdata);
	bool runActions(
			const AosDataRecordObjPtr &blob,
			const AosRundataPtr &rdata);
	bool checkData();
};
#endif

