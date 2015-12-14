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
#ifndef AOS_IILTrans_StrIncrementDocidByName_h
#define AOS_IILTrans_StrIncrementDocidByName_h

#include "TransUtil/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTransStrIncrementDocidByName : public AosIILTrans
{
	OmnString	mKey;
	u64 		mIncValue;
	u64 		mInitValue;
	bool		mAddFlag;

public:
	AosIILTransStrIncrementDocidByName(const bool regflag);
	AosIILTransStrIncrementDocidByName(
		const OmnString &iilname,
		const bool isPersis,
		const OmnString &key,
		const u64 inc_value,
		const u64 init_value,
		const bool add_flag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransStrIncrementDocidByName(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	
	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual int getSerializeSize() const;
	virtual AosIILType getIILType() const;
};

#endif
