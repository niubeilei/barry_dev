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
#ifndef Omn_Random_RandomGenU64Enum_h
#define Omn_Random_RandomGenU64Enum_h

#include "RandomGen/RandomGen.h"
#include "Util/RCObjImp.h"
#include <vector>
class AosRandomGenU64Enum:public AosRandomGen
{
	OmnDefineRCObject;
    
private:
	int mRecordNum;
	struct Record
	{
		u64 key;
		int weight;
	};
	vector<Record> mRecord;
public:
	AosRandomGenU64Enum(const int version);
	AosRandomGenU64Enum(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	~AosRandomGenU64Enum();
    virtual bool config( 	const AosRundataPtr &rdata, 
				            const AosXmlTagPtr &conf);
	virtual bool generateData(AosRundata *rdata, 
						AosValueRslt &value_rslt);
   // virtual OmnString getRandomU64();

	//virtual char getRandChar(int begin,int end);
	
	virtual AosJimoPtr cloneJimo() const;

};
#endif

