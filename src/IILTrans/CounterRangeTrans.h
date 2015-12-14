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
// 2014/05/15 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_CounterRange_h
#define AOS_IILTrans_CounterRange_h

#include "CounterUtil/CounterQuery.h"
#include "TransUtil/IILTrans.h"


class AosIILTransCounterRange : public AosIILTrans
{
	AosQueryContextObjPtr	mQueryContext;
	AosQueryRsltObjPtr 		mQueryRslt;
	AosBitmapObjPtr			mQueryBitmap;
	AosCounterQueryPtr		mCounterQuery;

public:
	AosIILTransCounterRange(const bool regflag);
	AosIILTransCounterRange(
			const u64 &iilid,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const AosCounterQueryPtr &counter_query,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	AosIILTransCounterRange(
			const OmnString &iilname,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const AosCounterQueryPtr &counter_query,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIILTransCounterRange(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();

	// IILTrans Interface
	virtual bool proc(
			const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual bool needProc() const {return true;}
	virtual bool needCreateIIL() const {return false;}
	virtual int getSerializeSize() const;
};

#endif

