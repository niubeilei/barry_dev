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
#ifndef AOS_IILTrans_StrBatchAddMerge_h
#define AOS_IILTrans_StrBatchAddMerge_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTransStrBatchAddMerge : public AosIILTrans
{
	int			mLen;
	u64			mExecutorID;
	bool		mTrueDelete;	

public:
	AosIILTransStrBatchAddMerge(const bool regflag);
	AosIILTransStrBatchAddMerge(
			const OmnString &iilname,
			const int len,
			const u64 &executor_id,
			const bool true_delete,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIILTransStrBatchAddMerge(){}

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
