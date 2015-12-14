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
// 2013/09/06	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_HitRlbTesterCheckTrans_h
#define AOS_IILTrans_HitRlbTesterCheckTrans_h

#if 0
#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Opr.h"


class AosIILTransHitRlbTesterCheck : public AosIILTrans
{
	u64			mDocid;

public:
	AosIILTransHitRlbTesterCheck(const bool regflag);
	AosIILTransHitRlbTesterCheck(
		const OmnString &iilname,
		const u64 &docid,
		const u64 snap_id);
	~AosIILTransHitRlbTesterCheck(){}

	// Trans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual OmnString getStrType(){ return "eRlbTester_checkIIL"; };
	
	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;

};

#endif
#endif
