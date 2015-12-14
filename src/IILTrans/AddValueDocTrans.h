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
// 2014/05/19 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_AddValueDoc_h
#define AOS_IILTrans_AddValueDoc_h

#include "TransUtil/IILTrans.h"


class AosIILTransAddValueDoc : public AosIILTrans
{
	OmnString	mValue;
	u64			mDocid;
	AosIILType	mIILType;

public:
	AosIILTransAddValueDoc(const bool regflag);
	AosIILTransAddValueDoc(
			const u64 &iilid,
			const AosIILType &iiltype,
			const bool isPersis,
			const OmnString &value,
			const u64 &docid,
			const bool valueUnique,
			const bool docidUnique,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	AosIILTransAddValueDoc(
			const OmnString &iilname,
			const AosIILType &iiltype,
			const bool isPersis,
			const OmnString &value,
			const u64 &docid,
			const bool valueUnique,
			const bool docidUnique,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIILTransAddValueDoc(){}

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
	virtual bool needProc() const {return false;}
	virtual bool needCreateIIL() const {return true;}
	virtual int getSerializeSize() const;
};

#endif

