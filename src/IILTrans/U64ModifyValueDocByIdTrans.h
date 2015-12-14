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
// 10/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_U64ModifyValueDocById_h
#define AOS_IILTrans_U64ModifyValueDocById_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTransU64ModifyValueDocById : public AosIILTrans
{

private:
	u64			mDocid;
	u64			mOldValue;
	u64			mNewValue;

public:
	AosIILTransU64ModifyValueDocById(const bool regflag);
	AosIILTransU64ModifyValueDocById(
		const u64 iilid,
		const u64 oldvalue,
		const u64 newvalue,
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransU64ModifyValueDocById(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	
	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;

	u64	getOldValue() const {return mOldValue;}
	u64	getNewValue() const {return mNewValue;}
	u64 getDocid() const {return mDocid;}
};

#endif
