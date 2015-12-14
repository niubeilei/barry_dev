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
#ifndef Aos_ThreadShellRunners_CacherActionsRunner_h
#define Aos_ThreadShellRunners_CacherActionsRunner_h

#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "ThreadShellRunners/Ptrs.h"
#include "ThreadShellRunners/SplitMethod.h"
#include "Util/ValueRslt.h"

#include<vector>
using namespace std;

class AosCacherActionsRunner : virtual public OmnThrdShellProc
{
	OmnDefineRCObject;       

private:
	OmnMutexPtr								mLock;
	OmnCondVarPtr							mCondVar;
	int64_t									mPageSize;
	int										mNumSeqments;
	const AosDataCacherObjPtr				mDataCacher;
	vector<AosActionObjPtr>					mActionsBeforeSort;
	vector<AosActionObjPtr>					mActionsAfterSort;
	AosActionRunnerListenerPtr				mCaller;
	AosValueRslt							mValueRslt;
	AosSplitMethod::SplitMethod				mSplitMethodBeforeSort;
	AosSplitMethod::SplitMethod				mSplitMethodAfterSort;
	AosDataRecordObjPtr						mDataRecord;
	OmnString								mCalledClassName;
	AosRundataPtr							mRundata;

public:
	AosCacherActionsRunner(
			const int &num_seq,
			const int64_t &page_size,
			const AosDataCacherObjPtr &cacher,
			const vector<AosActionObjPtr> &actions_before_sort, 
			const vector<AosActionObjPtr> &actions_after_sort, 
			const AosActionRunnerListenerPtr &caller,
			const AosValueRslt &userData,
			const AosSplitMethod::SplitMethod split_method_before_sort,
			const AosSplitMethod::SplitMethod split_method_after_sort,
			const AosDataRecordObjPtr &record,
			const OmnString &calledClassName,
			const AosRundataPtr &rdata);
	~AosCacherActionsRunner();
	
	// OmnThrdShellProc interface
	virtual bool run();
	virtual bool procFinished();
	virtual bool actionFinished(
			  		const bool status,              
					const AosValueRslt &user_data,
					const AosRundataPtr &rdata);
	virtual bool serializeFrom(
					const AosBuffPtr &buff,           
					const AosRundataPtr &rdata);
	virtual bool serializeTo(
					const AosBuffPtr &buff, 
					const AosRundataPtr &rdata);
	virtual bool runActions(
			const int64_t &startPos,
			const int64_t &endPos,
			const vector<AosActionObjPtr> &actions,
			const AosRundataPtr &rdata);
private:

	bool run(
			const AosSplitMethod::SplitMethod split_method,
			const vector<AosActionObjPtr> &actions, 
			const AosRundataPtr &rdata);
	bool runByCpuCores(
			const vector<AosActionObjPtr> &actions,
			const AosRundataPtr &rdata);
	bool runByPaging(
			const vector<AosActionObjPtr> &actions,
			const AosRundataPtr &rdata);
	bool runOnCacher(
			const int num_segments, 
			const vector<AosActionObjPtr> &actions, 
			const AosRundataPtr &rdata);

private:
	bool runInBackgroundPriv(
				const vector<AosActionObjPtr> actions_before_sort, 
				const vector<AosActionObjPtr> actions_after_sort, 
				const AosActionRunnerListenerPtr &caller);
	bool runInForegroundPriv(
				const vector<AosActionObjPtr> actions_before_sort, 
				const vector<AosActionObjPtr> actions_after_sort, 
				const AosActionRunnerListenerPtr &caller);
};
#endif

