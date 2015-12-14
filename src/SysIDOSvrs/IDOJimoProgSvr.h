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
// 2015/04/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysIDOSvrs_IDOJimoProgSvr_h
#define Aos_SysIDOSvrs_IDOJimoProgSvr_h

#include "BSON/BSON.h"
#include "SEInterfaces/JimoCallPackage.h"


class AosRundata;

class AosIDOJimoProgSvr : public AosJimoCallPackage
{
	OmnDefineRCObject;

private:
	OmnMutex *	mLock;
	OmnString	mName;
	AosBSON		mBSON;

public:
	AosIDOJimoProgSvr(const int version);
	virtual ~AosIDOJimoProgSvr();

	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const;

	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff);
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff) const;
	virtual AosJimoPtr cloneJimo() const;

private:
	bool getStr(AosRundata *rdata, AosJimoCall &jimo_call);
	bool setStr(AosRundata *rdata, AosJimoCall &jimo_call);
};
#endif
