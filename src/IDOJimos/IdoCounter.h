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
// 2015/06/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IDOJimos_IdoCounter_h
#define Aos_IDOJimos_IdoCounter_h

#include "IDO/IDO.h"

class AosRundata;
class AosBuff;

class AosIdoCounter : public AosIDO
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
	AosIdoCounter(const int version);
	virtual ~AosIdoCounter();
	
	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff);
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff) const;
	virtual AosJimoPtr cloneJimo() const;
	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);

private:
	bool increment(AosRundata *rdata, AosJimoCall &jimo_call);
};
#endif
