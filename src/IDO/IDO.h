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
// 2015/03/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IDO_IDO_h
#define Aos_IDO_IDO_h

#include "IDO/Ptrs.h"
#include "Jimo/Jimo.h"
#include "SEInterfaces/JimoCallPackage.h"

class AosRundata;

class AosIDO : virtual public AosJimo
{
protected:
	OmnString		mIDOName;
	OmnString		mClassname;
	OmnString		mJimoName;
	int				mVersion;
	bool			mMayUseLocalCopy;
	u64				mDocid;
	
public:
	AosIDO();
	AosIDO(const u64 docid);
	AosIDO(const OmnString &name,
				const OmnString &classname, 
				const OmnString &jimo_name,
				const int version);
	~AosIDO();

	static AosIDOPtr createIDO(AosRundata *rdata, AosBuff *buff);

	void setIDOName(const OmnString &name) {mIDOName = name;}
	u64 getDocid(AosRundata *rdata) const {return mDocid;}
	void stopUsingLocalCopy(AosRundata *rdata) {mMayUseLocalCopy = false;}
	void startUsingLocalCopy(AosRundata *rdata) {mMayUseLocalCopy = true;}

	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff) = 0;
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff) const = 0;
	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call) = 0;
};
#endif

