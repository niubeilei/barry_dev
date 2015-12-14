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
// 11/19/2015	Created by Andy 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_AsyncTestTrans_h
#define Aos_DocTrans_AsyncTestTrans_h

#include "TransUtil/DocTrans.h"
#include "DocTrans/Ptrs.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/DataProcObj.h"

class AosAsyncTestTrans : public AosDocTrans
{
private: 
	AosBuffPtr 		mBuff;
	i64				mReqSize;
	i64				mRespSize;
	i64				mSleep;
	AosDataProcObjPtr mRespCaller;

public:
	AosAsyncTestTrans(const bool regflag);

	AosAsyncTestTrans(
			const u64 docid,
			const i64 req_size,
			const i64 resp_size, 
			const i64 sleep,
			const AosDataProcObjPtr &resp_caller,
			const AosRundataPtr &rdata);

	~AosAsyncTestTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack(); 

};
#endif

