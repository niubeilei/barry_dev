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
//
// Modification History:
// 2014/08/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomGenU64_h
#define Omn_Random_RandomGenU64_h

#include "RandomGen/RandomGen.h"
#include "Util/RCObjImp.h"

class AosRandomGenU64:public AosRandomGen
{
	OmnDefineRCObject;

private:
	int		mMin;
	int		mMax;
public:
	AosRandomGenU64(const int version);
	AosRandomGenU64(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	~AosRandomGenU64();
    virtual bool config( 	const AosRundataPtr &rdata, 
				            const AosXmlTagPtr &conf);

	virtual bool generateData(AosRundata *rdata, 
						AosValueRslt &value_rslt);

	virtual AosJimoPtr cloneJimo() const;
};
#endif

