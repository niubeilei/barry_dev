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
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_QueryDocByNameAsync_h
#define AOS_IILTrans_QueryDocByNameAsync_h

#include "TransUtil/IILTrans.h"
#include "TransUtil/Ptrs.h"
#include "SEUtil/AsyncRespCaller.h"
#include "Util/Opr.h"


class AosIILTransQueryDocByNameAsync : public AosIILTrans
{
	AosQueryContextObjPtr	mQueryContext;
	AosQueryRsltObjPtr 		mQueryRslt;
	AosBitmapObjPtr			mQueryBitmap;
	AosAsyncRespCallerPtr 	mRespCaller;
	u64						mReqId;

public:
	AosIILTransQueryDocByNameAsync(const bool regflag);
	AosIILTransQueryDocByNameAsync(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &reqId,
		const u64 &snapId,
		const bool need_save,
		const bool need_resp);
	~AosIILTransQueryDocByNameAsync();

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool respCallBack();
	
	// IILTrans Interface	
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual bool needCreateIIL() const {return false;}
	virtual int getSerializeSize() const;
};

#endif
