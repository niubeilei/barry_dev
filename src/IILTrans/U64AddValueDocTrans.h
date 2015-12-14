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
// 2014/05/20 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_U64AddValueDoc_h
#define AOS_IILTrans_U64AddValueDoc_h

#include "TransUtil/IILTrans.h"


class AosIILTransU64AddValueDoc : public AosIILTrans
{
	u64			mValue;
	u64			mDocid;

public:
	AosIILTransU64AddValueDoc(const bool regflag);
	AosIILTransU64AddValueDoc(
			const u64 &iilid,
			const bool isPersis,
			const u64 &value,
			const u64 &docid,
			const bool valueUnique,
			const bool docidUnique,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	AosIILTransU64AddValueDoc(
			const OmnString &iilname,
			const bool isPersis,
			const u64 &value,
			const u64 &docid,
			const bool valueUnique,
			const bool docidUnique,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIILTransU64AddValueDoc(){}

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

