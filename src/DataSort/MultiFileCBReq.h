////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataSort_MultiFileCBReq_h
#define AOS_DataSort_MultiFileCBReq_h

#include "DataSort/DataSortReq.h"
#include "Rundata/Ptrs.h"
#include "Sorter/Ptrs.h"
#include "Util/Ptrs.h"

using namespace std;

class AosMultiFileCBReq : public AosDataSortReq 
{
	OmnDefineRCObject;

	AosMultiFileSorterPtr	mCaller;
	u64 					mReqId;
	int64_t 				mExpectedSize;
	bool 					mFinished;

public:
	AosMultiFileCBReq(
		const AosMultiFileSorterPtr &caller,
		const u64 reqid,
		const int64_t expected_size,
		const bool finished)
	:
	mCaller(caller),
	mReqId(reqid),
	mExpectedSize(expected_size),
	mFinished(finished)
	{
	}
	
	~AosMultiFileCBReq()
	{
	}

	bool procReq()
	{
		bool rslt = mCaller->procReq(mReqId, mExpectedSize, mFinished);
		aos_assert_r(rslt, false);
		return rslt;
	}
};

#endif

