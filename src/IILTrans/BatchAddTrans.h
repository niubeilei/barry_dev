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
// 03/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_BatchAdd_h
#define AOS_IILTrans_BatchAdd_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTransBatchAdd : public AosIILTrans
{
	int			mLen;
	AosBuffPtr	mBuff;
	u64			mExecutorID;
	u64			mTaskDocid;
	bool		mCheckMd5;
	OmnString	mMd5;
	AosIILType	mIILType;
	bool		mCompressFlag;

public:
	AosIILTransBatchAdd(const bool regflag);
	AosIILTransBatchAdd(
			const OmnString &iilname,
			const AosIILType &iiltype,
			const int len,
			const AosBuffPtr &buff,
			const u64 &executor_id,
			const u64 &snap_id,
			const u64 &task_docid,
			const bool check_md5,
			const OmnString &md5,
			const bool need_save,
			const bool need_resp);
	AosIILTransBatchAdd(
			const u64 &iilid,
			const AosIILType &iiltype,
			const int len,
			const AosBuffPtr &buff,
			const u64 &executor_id,
			const u64 &snap_id,
			const u64 &task_docid,
			const bool check_md5,
			const OmnString &md5,
			const bool need_save,
			const bool need_resp);
	~AosIILTransBatchAdd(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	
	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual bool needProc() const {return true;}
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;
};

#endif
