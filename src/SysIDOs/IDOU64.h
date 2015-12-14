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
#ifndef Aos_SysIDO_U64IDO_h
#define Aos_SysIDO_U64IDO_h

#include "SEInterfaces/U64IDOObj.h"


class AosRundata;

class AosU64IDO : public AosU64IDOObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMethodSet,
		eMethodGet,
		eMethodCreateByName,
	};

private:
	OmnString	mName;
	u64			mValue;
	u32			mRepPolicy;

public:
	AosU64IDO(const int version);

	virtual bool set(AosRundata *rdata, const u64 value);
	virtual u64 get(AosRundata *rdata, const u64 dft);

	virtual void setName(const OmnString &s) {mName = s;}
	virtual OmnString getName() const {return mName;}

	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff);
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff) const;
	virtual AosJimoPtr cloneJimo() const;
};
#endif
