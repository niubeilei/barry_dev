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
#ifndef Aos_ThreadShellRunners_MultiActionsRunner_h
#define Aos_ThreadShellRunners_MultiActionsRunner_h

#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "ThreadShellRunners/Ptrs.h"
#include "ThreadShellRunners/SplitMethod.h"
#include "Util/ValueRslt.h"

class AosMultiActionsRunner
{
public:
	bool runInBackground(
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
				const AosRundataPtr &rdata);

	bool runInForeground(
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
				const AosRundataPtr &rdata);
};
#endif

