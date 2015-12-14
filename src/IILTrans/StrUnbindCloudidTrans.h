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
#ifndef AOS_IILTrans_StrUnbindCloudid_h
#define AOS_IILTrans_StrUnbindCloudid_h

#include "TransUtil/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTransStrUnbindCloudid : public AosIILTrans
{
	OmnString	mCloudid;
	u64			mDocid;

public:
	AosIILTransStrUnbindCloudid(const bool regflag);
	AosIILTransStrUnbindCloudid(
		const OmnString &iilname,
		const OmnString cloudid,
		const u64 docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransStrUnbindCloudid(){}

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
	virtual int getSerializeSize() const;
	virtual bool needProc() const {return true;}
	virtual bool needCreateIIL() const {return true;}

private:
	bool		unbindCloudid(
					const AosIILObjPtr &iilobj,
					const OmnString &cloudid,
					const u64 &docid,
					const AosRundataPtr &rdata);
};

#endif

