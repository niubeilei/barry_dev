////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2015/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysIDO_SeqnoIDO_h
#define Aos_SysIDO_SeqnoIDO_h

#include "SEInterfaces/SeqnoIDOObj.h"


class AosRundata;

class AosSeqnoIDO : public AosSeqnoIDOObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMethodNextSeqno,
	};

private:
	u64		mSeqno;
	u32		mRepPolicy;

public:
	AosSeqnoIDO(const int version);

	virtual u64 nextSeqno(AosRundata *rdata);

	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff);
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff) const;
	virtual AosJimoPtr cloneJimo() const;
};
#endif
