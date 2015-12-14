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
#ifndef AOS_IILTrans_StrAddInlineSearchValue_h
#define AOS_IILTrans_StrAddInlineSearchValue_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Opr.h"


class AosIILTransStrAddInlineSearchValue : public AosIILTrans
{
	OmnString	mValue;
	u64			mSeqid;
	OmnString	mSplitType;

public:
	AosIILTransStrAddInlineSearchValue(const bool regflag);
	AosIILTransStrAddInlineSearchValue(
		const OmnString &iilname,
		const OmnString value,
		const u64 seqid,
		const OmnString splitType,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransStrAddInlineSearchValue(){}

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

	OmnString	getValue() const {return mValue;}
};

#endif
