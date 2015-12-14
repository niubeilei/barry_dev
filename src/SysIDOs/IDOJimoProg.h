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
// 2015/04/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysIDO_IDOJimoProg_h
#define Aos_SysIDO_IDOJimoProg_h

#include "BSON/BSON.h"
#include "SEInterfaces/IDOJimoProgObj.h"


class AosRundata;

class AosIDOJimoProg : public AosIDOJimoProgObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMethodSetStr,
		eMethodGetStr,
	};

private:
	AosBSON		mBSON;

public:
	AosIDOJimoProg(const int version);

	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff);
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff) const;
	virtual AosJimoPtr cloneJimo() const;

	virtual bool setStr(AosRundata *rdata, 
						const OmnString &name,
						const OmnString &value);
	virtual OmnString getStr(AosRundata *rdata, 
						const OmnString &name, 
						const OmnString &dft) const;
};
#endif
