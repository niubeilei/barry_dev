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
#ifndef AOS_IILTrans_CreateIIL_h
#define AOS_IILTrans_CreateIIL_h

#include "TransUtil/IILTrans.h"
#include "Util/Opr.h"


class AosIILTransCreateIIL : public AosIILTrans
{
	AosIILType 	mIILType;

public:
	AosIILTransCreateIIL(const bool regflag);
	AosIILTransCreateIIL(
			const u64 iilid,
			const AosIILType iiltype,
			const bool is_persis,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	AosIILTransCreateIIL(
			const OmnString &iilname,
			const AosIILType iiltype,
			const bool is_persis,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIILTransCreateIIL(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

	// IILTrans Interface
	virtual int getSerializeSize() const;
	virtual bool proc(
			const AosIILObjPtr &iilobj, 
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata){return true;};
	virtual AosIILType getIILType() const;
};

#endif
