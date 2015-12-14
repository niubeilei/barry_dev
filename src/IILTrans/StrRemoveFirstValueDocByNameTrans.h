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
#ifndef AOS_IILTrans_StrRemoveFirstValueDocByName_h
#define AOS_IILTrans_StrRemoveFirstValueDocByName_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Opr.h"


class AosIILTransStrRemoveFirstValueDocByName : public AosIILTrans
{
	OmnString	mValue;
	bool		mReverse;

public:
	AosIILTransStrRemoveFirstValueDocByName(const bool regflag);
	AosIILTransStrRemoveFirstValueDocByName(
		const OmnString &iilname,
		const OmnString &value,
		const bool reverse,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);
	~AosIILTransStrRemoveFirstValueDocByName(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	
	// IILTrans Interface
	virtual bool needCreateIIL() const {return false;}
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;

	OmnString	getValue() const {return mValue;}
};

#endif
