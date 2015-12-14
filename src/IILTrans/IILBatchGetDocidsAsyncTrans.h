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
// 10/09/2013	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_IILBatchGetDocidsAsyncTrans_h
#define AOS_IILTrans_IILBatchGetDocidsAsyncTrans_h

#include "TransUtil/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "SEUtil/AsyncRespCaller.h"


class AosIILBatchGetDocidsAsyncTrans : public AosIILTrans
{

	AosBuffPtr  	mColumns;
	AosAsyncRespCallerPtr  mRespCaller;
	int				mFieldIdx;
public:
	AosIILBatchGetDocidsAsyncTrans(const bool regflag);
	AosIILBatchGetDocidsAsyncTrans(
		const AosBuffPtr &columns,
		const OmnString &iilname,
		const AosAsyncRespCallerPtr &resp_caller,
		const int field_idx,
		const bool isPersis,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILBatchGetDocidsAsyncTrans(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool respCallBack();
	
	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual int getSerializeSize() const;
	virtual AosIILType getIILType() const;
};

#endif
