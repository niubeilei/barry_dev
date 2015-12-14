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
#ifndef Aos_DocTrans_DocTestTrans_h
#define Aos_DocTrans_DocTestTrans_h

#include "TransUtil/DocTrans.h"

class AosDocTestTrans : public AosDocTrans
{
private: 
	AosBuffPtr 		mBuff;
	i64				mReqSize;
	i64				mRespSize;
	i64				mSleep;

public:
	AosDocTestTrans(const bool regflag);

	AosDocTestTrans(
			const u64 docid,
			const i64 req_size,
			const i64 resp_size, 
			const i64 sleep,
			const AosRundataPtr &rdata);

	~AosDocTestTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

	virtual bool respCallBack();

};
#endif

