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
#ifndef AOS_IILTrans_StrIncrementDocidById_h
#define AOS_IILTrans_StrIncrementDocidById_h

#include "TransUtil/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"


class AosIILTransStrIncrementDocidById : public AosIILTrans
{

private:
	OmnString 	mKey;
	u64 		mIncValue;
	u64 		mInitValue;
	bool		mAddFlag;

public:
	AosIILTransStrIncrementDocidById(const bool regflag);
	AosIILTransStrIncrementDocidById(
		const u64 iilid,
		const bool isPersis,
		const OmnString &key,
		const u64 incValue,
		const u64 initValue,
		const bool addFlag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransStrIncrementDocidById(){}

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

private:
};

#endif
