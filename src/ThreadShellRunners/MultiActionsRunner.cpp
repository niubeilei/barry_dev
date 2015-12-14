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
#include "ThreadShellRunners/MultiActionsRunner.h"

#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/DataCacherObj.h"
#include "Thread/ThreadShellMgr.h"
#include "ThreadShellRunners/ActionsRunner.h"
#include "ThreadShellRunners/ActionRunnerListener.h"
#include "ThreadShellRunners/CacherActionsRunner.h"

bool
AosMultiActionsRunner::runInForeground(
		const int &num_seq,
		const int64_t &page_size,
		const AosValueRslt &userData,
		const OmnString &calledClassName,
		const AosDataCacherObjPtr &cacher,
		const AosDataRecordObjPtr &record,
		const AosActionRunnerListenerPtr &caller,
		const vector<AosActionObjPtr> &actions_before_sort, 
		const vector<AosActionObjPtr> &actions_after_sort, 
		const AosSplitMethod::SplitMethod split_method_before_sort,
		const AosSplitMethod::SplitMethod split_method_after_sort, 
		const AosRundataPtr &rdata)
{
	AosCacherActionsRunner runner(num_seq, page_size, cacher, 
			actions_before_sort, actions_after_sort, caller, userData, 
			split_method_before_sort, split_method_after_sort, record, calledClassName, rdata);
	return runner.run();
}


bool
AosMultiActionsRunner::runInBackground(
		const int &num_seq,
		const int64_t &page_size,
		const AosValueRslt &userData,
		const OmnString &calledClassName,
		const AosDataCacherObjPtr &cacher,
		const AosDataRecordObjPtr &record,
		const AosActionRunnerListenerPtr &caller,
		const vector<AosActionObjPtr> &actions_before_sort, 
		const vector<AosActionObjPtr> &actions_after_sort, 
		const AosSplitMethod::SplitMethod split_method_before_sort,
		const AosSplitMethod::SplitMethod split_method_after_sort, 
		const AosRundataPtr &rdata)
{
	OmnThrdShellProcPtr runner = OmnNew AosCacherActionsRunner(num_seq, page_size,
			cacher, actions_before_sort, actions_after_sort, caller, userData, 
			split_method_before_sort, split_method_after_sort, record, calledClassName, rdata);
	return OmnThreadShellMgr::getSelf()->proc(runner);
}

