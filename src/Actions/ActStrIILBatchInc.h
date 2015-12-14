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
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActStrIILBatchInc_h
#define Aos_SdocAction_ActStrIILBatchInc_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Ptrs.h"


class AosActStrIILBatchInc : virtual public AosSdocAction
{
private:
	OmnString					mIILName;
	int							mLen;
	AosIILUtil::AosIILIncType 	mIncType;
	u64							mInitdocid;
	u64							mSnapId;
	u64							mTaskDocid;

public:
	AosActStrIILBatchInc(const bool flag);
	AosActStrIILBatchInc(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActStrIILBatchInc();

	virtual bool run(
			const AosBuffPtr &buff,
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

