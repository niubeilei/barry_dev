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
#ifndef Omn_Random_RandomGen_h
#define Omn_Random_RandomGen_h

#include "Util/RCObject.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"                                                                                                                  
#include "aosUtil/Types.h"
#include "Random/RandomUtil.h"
#include "Random/RandomObj.h"
#include "SEInterfaces/RandomGenObj.h"
#include "Util/RCObjImp.h"                                                                                 
#include "XmlParser/Ptrs.h"
#include "Util/ValueRslt.h"


class AosRandomGen : public AosRandomGenObj
{
private:

public:
	AosRandomGen(const int version);
	~AosRandomGen();
    //virtual AosJimoPtr cloneJimo() const;
	static AosRandomGenPtr createRandomGen(
					    const AosRundataPtr &rdata, 
						const AosXmlTagPtr &conf);

	virtual bool generateData(AosRundata *rdata, 
						AosValueRslt &value_rslt) = 0;
//	virtual AosJimoPtr cloneJimo() const;
};

#endif

