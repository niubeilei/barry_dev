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
#ifndef AOS_IILTrans_U64GetDocidByName_h
#define AOS_IILTrans_U64GetDocidByName_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "TransUtil/Ptrs.h"


class AosIILTransU64GetDocidByName : public AosIILTrans
{
	u64		mKey;

public:
	AosIILTransU64GetDocidByName(const bool regflag);
	AosIILTransU64GetDocidByName(
		const OmnString &iilname,
		const u64 &key,
		const bool need_save, 
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransU64GetDocidByName(){}

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
};

#endif
