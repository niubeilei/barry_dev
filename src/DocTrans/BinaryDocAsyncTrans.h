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
// 05/17/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BinaryDocAsyncTrans_h
#define Aos_DocTrans_BinaryDocAsyncTrans_h

#include "TransUtil/DocTrans.h"
#include "TransUtil/Ptrs.h"


class AosBinaryDocAsyncTrans : public AosDocTrans
{
	u64 				   mBinaryDocId;
	AosAsyncRespCallerPtr  mRespCaller;

public:
	AosBinaryDocAsyncTrans(const bool regflag);
	AosBinaryDocAsyncTrans(
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &binaryDocId,
		const bool need_save,	
		const bool need_resp,
		const u64 snap_id);
	~AosBinaryDocAsyncTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack();
	
	AosBuffPtr getDoc();

};
#endif


