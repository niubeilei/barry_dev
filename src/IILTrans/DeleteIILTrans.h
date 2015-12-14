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
#ifndef AOS_IILTrans_DeleteIIL_h
#define AOS_IILTrans_DeleteIIL_h

#include "TransUtil/IILTrans.h"


class AosIILTransDeleteIIL : public AosIILTrans
{
	bool		mTrueDelete;	

public:
	AosIILTransDeleteIIL(const bool regflag);
	AosIILTransDeleteIIL(
			const u64 &iilid,
			const bool true_delete,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	AosIILTransDeleteIIL(
			const OmnString &iilname,
			const bool true_delete,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIILTransDeleteIIL(){}

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

