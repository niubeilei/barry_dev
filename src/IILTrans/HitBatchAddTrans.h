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
#ifndef AOS_IILTrans_HitBatchAdd_h
#define AOS_IILTrans_HitBatchAdd_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Ptrs.h"


class AosIILTransHitBatchAdd : public AosIILTrans
{
	vector<u64>				mDocIds;
	u64						mTaskDocid;

public:
	AosIILTransHitBatchAdd(const bool regflag);
	AosIILTransHitBatchAdd(
			const OmnString &iilname,
			const vector<u64> &docids,
			const u64 &snap_id,
			const u64 &task_docid,
			const bool need_save,
			const bool need_resp);
	~AosIILTransHitBatchAdd(){}

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
