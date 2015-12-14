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
// 07/06/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActJoinIILMultiThrdRunner_h
#define Aos_SdocAction_ActJoinIILMultiThrdRunner_h
/*
#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataAssembler/Ptrs.h"
#include "DataRecord/DataRecord.h"
#include "IILAssembler/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/Ptrs.h"
#include "SEInterfaces/IILScannerListener.h"
#include "SEInterfaces/TaskObj.h"
#include "SEUtil/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include <vector>
using namespace std;

class AosActJoinIILMultiThrdRunner : public OmnThrdShellProc,
					  				 public AosIILScannerListener
{
	OmnDefineRCObject;

private:
	struct WorkingData
	{
		const char *key;
		int			len;
		int			idx;
		bool		is_smallest;
	};

	enum Status
	{
		eInvalid,

		eIdle,
		eWait,
		eActive,
		eFinished,
	};

	Status						mStatus;
	u32							mStartTime;
	AosActJoinIILMultiThrdPtr	mCaller;
	vector<AosIILScannerObjPtr>	mIILScanners;
	int							mNumScanners;
	vector<WorkingData>			mWorkingData;
	vector<WorkingData>			mNewData;
	AosDataAssemblerPtr			mIILAssembler;
	AosDataAssemblerPtr			mDocAssembler;
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	AosGroupbyProcPtr			mGroupbyProc;
	AosRundataPtr				mRundata;
	bool						mDataRetrieveSuccess;
	AosDataRecordObjPtr			mRecord;
	int							mPrefixLen;		// Chen Ding, 06/11/2012

public:
	AosActJoinIILMultiThrdRunner(
			const AosActJoinIILMultiThrdPtr &caller,
			const vector<AosIILScannerObjPtr> &scanners, 
			const AosRundataPtr &rdata);
	virtual bool run();
	virtual bool procFinished();

	virtual bool dataRetrieveFailed(
			const AosIILScannerPtr &scanner,
			const AosRundataPtr &rdata);

	// AosIILScannerListener Interface
	virtual bool dataRetrieved(const AosIILScannerObjPtr &scanner, const AosRundataPtr &rdata);
	virtual bool dataRetrieveFailed(const AosIILScannerObjPtr &scanner, const AosRundataPtr &rdata);
	virtual bool noMoreData(const AosIILScannerObjPtr &scanner, const AosRundataPtr &rdata);

private:
	bool addToWorkingData(
					vector<WorkingData> &working_data, 
					int &num_values,
					const char *data, 
					const int len,
					const u32 idx);
	bool mergeData(const int num_new_data);
	bool setSmallestFlag(const u32 nn);
	void printWorkingData(const int num_new_data);
};
*/
#endif

