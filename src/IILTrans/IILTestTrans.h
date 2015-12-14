///////////////////////////////////////////////////////////////////////////
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
#ifndef AOS_IILTrans_IILTestTrans_h
#define AOS_IILTrans_IILTestTrans_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTestTrans : public AosIILTrans
{
	AosBuffPtr 		mBuff;
	i64				mReqSize;
	i64				mRespSize;
	i64				mSleep;
	AosIILType		mIILType;

public:
	AosIILTestTrans(const bool regflag);
	AosIILTestTrans(
			const OmnString &iilname,
			const i64 req_size,
			const i64 resp_size, 
			const i64 sleep,
			const AosRundataPtr &rdata);

	~AosIILTestTrans(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();

	virtual bool respCallBack();
	
	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);

	virtual bool needProc() const {return true;}
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;
};

#endif
