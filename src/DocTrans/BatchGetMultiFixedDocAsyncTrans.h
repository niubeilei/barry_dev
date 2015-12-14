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
// 10/20/2014	Created by Andy
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BatchGetMultiFixedDocAsyncTrans_h
#define Aos_DocTrans_BatchGetMultiFixedDocAsyncTrans_h

#include "TransUtil/DocTrans.h"

class AosBatchGetMultiFixedDocAsyncTrans : virtual public AosDocTrans
{

private:
	u64							mDataScanReqId;
	AosAsyncRespCallerPtr 		mRespCaller;
	
	vector<u64>					mDocids;
	vector<int>					mRecordLens;
	int							mVid;
	map<int, vector<u64> >		mDocidGroup;

public:
	AosBatchGetMultiFixedDocAsyncTrans(const bool regflag);
	AosBatchGetMultiFixedDocAsyncTrans(
		const u64 data_scan_req_id,
		const u32 vid,
		const vector<u64> &docids,
		const vector<int> &record_lens,
		const u64 snap_id,
		const AosAsyncRespCallerPtr &resp_caller);
	~AosBatchGetMultiFixedDocAsyncTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

	virtual bool respCallBack();
private:
	bool 			setErrResp();
	bool			splitDocid();

};
#endif

