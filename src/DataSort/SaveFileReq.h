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
#ifndef AOS_DataSort_SaveFileReq_h
#define AOS_DataSort_SaveFileReq_h

#include "DataSort/DataSortReq.h"
#include "DataSort/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"

using namespace std;

class AosSaveFileReq : public AosDataSortReq
{
	OmnDefineRCObject;

	AosDataSortObjPtr	mCaller;
	AosBuffPtr			mBuff;
	AosRundataPtr		mRundata;

public:
	AosSaveFileReq(
		const AosDataSortObjPtr &caller,
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
	:
	mCaller(caller),
	mBuff(buff),
	mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
	{
	}
	
	~AosSaveFileReq()
	{
	}

	bool procReq()
	{
		bool rslt = mCaller->saveToFile(mBuff, mRundata);
		aos_assert_r(rslt, false);
		return rslt;
	}
};

#endif

