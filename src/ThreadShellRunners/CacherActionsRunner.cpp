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
// This is a convenience class that is used to run actions on AosBuffArray. 
// There are two groups of actions: 
// 	actions before sorting
// 	actions after sorting
//
// There are the following ways of running actions:
// 1. Asynchronous Running
//    This means that the caller supply the AosBuffArray and actions before/after
//    the sorting. This class runs these actions in background. The control is
//    returned immediately. When all actions are finished, the caller is called back.
//
// 2. Synchronous Running
//    This means that the call is not returned until all actions are finished. 
//
// Actions can be grouped. Actions in the same group can run in parallel, but
// actions in different groups must run sequentially. 
//
// Modification History:
// 07/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ThreadShellRunners/CacherActionsRunner.h"

#include "API/AosApiG.h"
#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/DataCacherObj.h"
#include "Thread/ThreadShellMgr.h"
#include "ThreadShellRunners/ActionsRunner.h"
#include "ThreadShellRunners/ActionRunnerListener.h"

const int smMinSegmentSize = 1;
const int smMaxSegments = 100;



AosCacherActionsRunner::AosCacherActionsRunner(
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
		const AosRundataPtr &rdata)
:
OmnThrdShellProc(calledClassName),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mPageSize(page_size),
mNumSeqments(num_seq),
mDataCacher(cacher),
mActionsBeforeSort(actions_before_sort),
mActionsAfterSort(actions_after_sort),
mCaller(caller),
mValueRslt(userData),
mSplitMethodBeforeSort(split_method_before_sort),
mSplitMethodAfterSort(),
mDataRecord(record),
mCalledClassName(calledClassName),
mRundata(rdata)
{
}


AosCacherActionsRunner::~AosCacherActionsRunner()
{
}


bool
AosCacherActionsRunner::run()
{
	aos_assert_rr(mDataCacher, mRundata, false);
/*
	if (!mDataCacher->convertToReadCacher())
	{
		AosSetErrorU(mRundata, "not_read_cacher") << ": " << mDataCacher->toString() << enderr;
		return false;
	}
*/
	// This function runs 'actions_before_sort'. If 'actions_after_sort' is
	// not empty, it sorts the cacher, and then runs them.
	if (mActionsBeforeSort.size() > 0)
	{
		if (!run(mSplitMethodBeforeSort, mActionsBeforeSort, mRundata)) return false;
	}

	if (mActionsAfterSort.size() <= 0) return true;
	//mDataCacher->sort();
	return run(mSplitMethodAfterSort, mActionsAfterSort, mRundata);
}


bool 
AosCacherActionsRunner::procFinished()
{
	OmnNotImplementedYet;
	return false;
	//return mCaller->Finished();
}


bool 
AosCacherActionsRunner::actionFinished(
		const bool status,            
		const AosValueRslt &user_data,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosCacherActionsRunner::serializeFrom(
		const AosBuffPtr &buff,           
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCacherActionsRunner::serializeTo(
			const AosBuffPtr &buff, 
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosCacherActionsRunner::run(
		const AosSplitMethod::SplitMethod split_method,
		const vector<AosActionObjPtr> &actions, 
		const AosRundataPtr &rdata)
{
	if (actions.size() <= 0) return true;

	// Check whether it needs to split the cacher and run threads on 
	// segments. 
	switch (split_method)
	{
		case AosSplitMethod::eByPageSize:
		 return runByPaging(actions, rdata);

		case AosSplitMethod::eByNumberOfCores:
		 return runByCpuCores(actions, rdata);

		case AosSplitMethod::eByNumSegments:
		 return runOnCacher(mNumSeqments, actions, rdata);

	default:
		 break;
	}

	// Should never come here
	AosSetErrorU(rdata, "invalid_split_type") << ": " << split_method << enderr;
	return runOnCacher(1, actions, rdata);
}


bool
AosCacherActionsRunner::runByCpuCores(
		const vector<AosActionObjPtr> &actions,
		const AosRundataPtr &rdata)
{
	// This means that it want to run all actions in parallel. The number of
	// segments is determined by the number of CPU cores. 
	int num_segments = AosGetNumCpuCores();
	if (num_segments < 1) num_segments = 1;
	return runOnCacher(num_segments, actions, rdata);
}


bool
AosCacherActionsRunner::runByPaging(
		const vector<AosActionObjPtr> &actions,
		const AosRundataPtr &rdata)
{
	if (mPageSize <= 0)
	{
		// Invalid pagesize. Will run in single thread
		AosSetErrorU(rdata, "invalid_pagesize") << enderr;
		return runOnCacher(1, actions, rdata);
	}

	if (mDataCacher->size() < 0) 
	{
		// Will run in single thread.
		AosSetErrorU(rdata, "cacher_cannot_get_size") << enderr;
		return runOnCacher(1, actions, rdata);
	}
	int num_segments = mDataCacher->size() / mPageSize;
	if (num_segments < 1) num_segments = 1;
	return runOnCacher(num_segments, actions, rdata);
}


bool
AosCacherActionsRunner::runOnCacher(
		const int num_segments, 
		const vector<AosActionObjPtr> &actions, 
		const AosRundataPtr &rdata)
{
	// This function runs on cacher in 'num_segments'. Different segments
	// run in parallel. 
	int64_t size = mDataCacher->size();
	if (num_segments <= 1 || size < smMinSegmentSize)
	{
		if (num_segments < 1)
		{
			AosSetErrorU(rdata, "invalid_num_segments") << ": " << num_segments << enderr;
		}

		return runActions(0, -1, actions, rdata);
	}

	int num_segs = num_segments;
	if (num_segs > smMaxSegments) num_segs = smMaxSegments;

	// It runs in a number of segments. 
	int64_t num_entries_per_page = (size / num_segs) + 1;

	int64_t start = 0;
	AosValueRslt vv;
	for (int i=0; i<num_segs-1; i++)
	{
		bool rslt = runActions(
				start, start+num_entries_per_page-1, actions, rdata);
		aos_assert_rr(rslt, rdata, false);
		start += num_entries_per_page;
	}

	return runActions(start, -1, actions, rdata);
}


bool
AosCacherActionsRunner::runActions(
		const int64_t &startPos,
		const int64_t &endPos,
		const vector<AosActionObjPtr> &actions,
		const AosRundataPtr &rdata)
{
	OmnThrdShellProcPtr thisptr(this, false);	
	try
	{
		OmnThrdShellProcPtr runner = OmnNew AosActionsRunner(
			startPos, endPos, 0, mDataCacher, 0, 
			AosActionsRunner::eDataCacher, actions, thisptr, 
			mValueRslt, mDataRecord->clone(), mCalledClassName, rdata);
		OmnThreadShellMgr::getSelf()->proc(runner);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return false;
	}
	
	/*
	// This function runs all the actions in 'mActions' in groups 
	// against the input. Actions in the same group are run in parallel, 
	// and actions in different groups are run in sequential. 
	// This is handled by thread shells.
	if (mStartPos < 0)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		mStatus = false;
		return false;
	}

	for (u32 i=0; i<actions.size(); i++)
	{
		mCacher->firstRecordInRange(mStartPos);
		while (mCacher->nextRecord())
		{
		}
	}

	// Actions are grouped. It runs actions in the same group in parallel, 
	// and actions in different groups sequentially.
	if (mActions.size() <= 0) return true;
	
	if (mActions.size() == 1)
	{
		AosActionRunnerListener thisptr(this, false);
		return AosSingleActionRunner::runAction(mStartPos, mEndPos, mCacher, 
				mActions[0], mUserValue, mMaxErrors, thisptr, rdata);
	}

	while (num_actions_finished < mActions.size())
	{
		int group_id = (int)AosActionObj::eMaxGroupId;

		// Find the next group id, which is the smallest group ID
		// that is greater than 'last_group_id'.
		for (u32 i=0; i<mActions.size(); i++)
		{
			int gid = mActions[i]->getGroupId();
			if (gid < 0) gid = 0;
			if (gid > last_group_id)
			{
				if (gid < group_id) group_id = gid;
			}
		}

		if (group_id == (int)AosActionObj::eMaxGroupId)
		{
			// No more actions to run.
			return true;
		}

		// Collect all the actions whose group id is the same as 'group_id'.
		vector<AosCacherActionRunnerPtr> actions_to_run;
		for (u32 i=0; i<mActions.size(); i++)
		{
			if (mActions[i]->getGroupId() == group_id)
			{
				actions_to_run.push_back(OmnNew AosCacherActionRunner(start, end, mCacher, 
							mActions[i], rdata));
			}
		}

		OmnThreadShellMgr::procSync(actions_to_run);
	}
	*/

	return true;
}

