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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BatchGetFixedDocAsyncTrans_h
#define Aos_DocTrans_BatchGetFixedDocAsyncTrans_h

#include "TransUtil/DocTrans.h"

class AosBatchGetFixedDocAsyncTrans : virtual public AosDocTrans
{

private:
	u64							mDataScanReqId;
	AosAsyncRespCallerPtr 		mRespCaller;
	
	vector<u64>					mDocids;
	int							mRecordLen;
	int							mVid;

public:
	AosBatchGetFixedDocAsyncTrans(const bool regflag);
	AosBatchGetFixedDocAsyncTrans(
		const u64 data_scan_req_id,
		const u32 vid,
		const vector<u64> &docids,
		const int record_len,
		const u64 snap_id,
		const AosAsyncRespCallerPtr &resp_caller);
	~AosBatchGetFixedDocAsyncTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

	virtual bool respCallBack();
private:
	bool 			setErrResp();

};
#endif

