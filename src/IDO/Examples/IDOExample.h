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
#ifndef Aos_IDO_Examples_IDOExample_h
#define Aos_IDO_Examples_IDOExample_h

#include "IDO/IDO.h"

class AosRundata;
class AosBuff;

class AosIDOExample : public AosIDO
{
	OmnDefineRCObject;
public:
	enum 
	{
		eInvalidIDOFunc, 

		eIDOFunc_Increment
	};

private:
	i64			mCount;
	OmnString	mName;
	OmnMutex *	mLock;

public:
	AosIDOExample(const int version);
	virtual ~AosIDOExample();
	
	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff);
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff) const;
	virtual AosJimoPtr cloneJimo() const;
	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);

private:
	bool increment(AosRundata *rdata, AosJimoCall &jimo_call);
};
#endif
