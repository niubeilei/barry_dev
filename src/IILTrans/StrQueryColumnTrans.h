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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_StrQueryColumnTrans_h
#define AOS_IILTrans_StrQueryColumnTrans_h

#include "TransUtil/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Opr.h"


class AosIILTransStrQueryColumn : public AosIILTrans
{
	int				mFieldLen1;
	int				mFieldLen2;
	OmnString		mValue1;
	OmnString		mValue2;
	AosOpr			mOpr1;
	AosOpr			mOpr2;
	int				mStartIdx;
	int				mPageSize;
	int				mOrderBy;

public:
	AosIILTransStrQueryColumn(const bool regflag);
	~AosIILTransStrQueryColumn(){}

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
