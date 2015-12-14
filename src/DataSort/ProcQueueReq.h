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
#ifndef AOS_DataSort_ProcQueueReq_h
#define AOS_DataSort_ProcQueueReq_h

#include "DataSort/DataSortCacher.h"
#include "DataSort/DataSortReq.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"

using namespace std;

class AosProcQueueReq : public AosDataSortReq
{
	OmnDefineRCObject;

	AosDataSortCacherPtr	mCaller;

public:
	AosProcQueueReq(const AosDataSortCacherPtr &caller)
	:
	mCaller(caller)
	{
	}
	
	~AosProcQueueReq()
	{
	}

	bool procReq()
	{
		bool rslt = mCaller->procReq();
		aos_assert_r(rslt, false);
		return rslt;
	}
};

#endif

