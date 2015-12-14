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
#ifndef AOS_IILTrans_StrBatchIncMerge_h
#define AOS_IILTrans_StrBatchIncMerge_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTransStrBatchIncMerge : public AosIILTrans
{
	int							mLen;
	u64							mInitdocid;
	AosIILUtil::AosIILIncType 	mIncType;
	bool						mTrueDelete;	

public:
	AosIILTransStrBatchIncMerge(const bool regflag);
	AosIILTransStrBatchIncMerge(
		const OmnString &iilname,
		const int entry_len,
		const u64 init_value,
		const AosIILUtil::AosIILIncType inc_type,
		const bool true_delete,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransStrBatchIncMerge(){}

	// IILTrans Interface
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
