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
#ifndef AOS_IILTrans_HitAddDocById_h
#define AOS_IILTrans_HitAddDocById_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Opr.h"


class AosIILTransHitAddDocById : public AosIILTrans
{
private:
	u64			mDocid;
	
public:
	AosIILTransHitAddDocById(const bool regflag);
	AosIILTransHitAddDocById(
			const u64 &transid,
			const u64 &iilid,
			const bool isPersis,
			const u64 &docid,
			const AosRundataPtr &rdata);

	~AosIILTransHitAddDocById(){}

	// Trans Interface
	virtual bool	serializeTo(const AosBuffPtr &buff);
	virtual bool	serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();

	// IILTrans Interface
	virtual bool	proc(
						const AosIILObjPtr &iilobj,
						AosBuffPtr &resp_buff,
						const AosRundataPtr &rdata);
	// Ketty 2012/10/24	
	//virtual bool	initTrans(
	//					const AosXmlTransPtr &trans, 
	//					const AosXmlTagPtr &trans_doc,
	//					const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual int		getSerializeSize() const;
	
	u64				getDocid() const {return mDocid;}
};

#endif
