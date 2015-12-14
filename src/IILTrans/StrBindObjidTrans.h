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
#ifndef AOS_IILTrans_StrBindObjid_h
#define AOS_IILTrans_StrBindObjid_h

#include "TransUtil/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTransStrBindObjid : public AosIILTrans
{
	OmnString	mObjid;
	u64			mDocid;

public:
	AosIILTransStrBindObjid(const bool regflag);
	AosIILTransStrBindObjid(
		const OmnString &iilname,
		const OmnString &objid,
		const u64 docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransStrBindObjid(){}

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
	bool		bindObjid(
					const AosIILObjPtr &iilobj,
					const OmnString &objid,
					const u64 &docid,
					const AosRundataPtr &rdata);
};

#endif

