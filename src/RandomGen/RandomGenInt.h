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
#ifndef Omn_Random_RandomGenInt_h
#define Omn_Random_RandomGenInt_h

#include "RandomGen/RandomGen.h"
#include "Util/RCObjImp.h"

class AosRandomGenInt:public AosRandomGen
{
	OmnDefineRCObject;

private:
	int		mMin;
	int		mMax;
    int     mWeight;
	int     mLen;
public:
	AosRandomGenInt(const int version);
	AosRandomGenInt(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	~AosRandomGenInt();
    virtual bool config( 	AosRundata *rdata, 
				            const AosXmlTagPtr &conf);
    AosJimoPtr cloneJimo()const;
	virtual bool generateData(AosRundata *rdata, 
						AosValueRslt &value_rslt);
};
#endif

