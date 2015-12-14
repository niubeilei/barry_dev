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
#ifndef AOS_IILTrans_GetTotalNumDocsByName_h
#define AOS_IILTrans_GetTotalNumDocsByName_h

#include "TransUtil/Ptrs.h"
#include "TransUtil/IILTrans.h"


class AosIILTransGetTotalNumDocsByName : public AosIILTrans
{

public:
	AosIILTransGetTotalNumDocsByName(const bool regflag);
	AosIILTransGetTotalNumDocsByName(
			const OmnString &iilname,
			const u64 &snap_id,
			const bool need_save,
			const bool need_resp);
	~AosIILTransGetTotalNumDocsByName(){}

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
	virtual bool needCreateIIL() const {return false;}
	virtual int getSerializeSize() const;
};

#endif
