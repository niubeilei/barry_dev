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
#ifndef Omn_Random_RandomGenStrEnum_h
#define Omn_Random_RandomGenStrEnum_h

#include "RandomGen/RandomGen.h"
#include "Util/RCObjImp.h"
#include <vector>
class AosRandomGenStrEnum:public AosRandomGen
{
	OmnDefineRCObject;
    
private:
	int mRecordNum;
	struct Record
	{
		OmnString str;
		int weight;
	};
	vector<Record> mRecord;
public:
	AosRandomGenStrEnum(const int version);
	AosRandomGenStrEnum(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	~AosRandomGenStrEnum();
    virtual bool config( 	const AosRundataPtr &rdata, 
				            const AosXmlTagPtr &conf);
	virtual bool generateData(AosRundata *rdata, 
						AosValueRslt &value_rslt);
    virtual OmnString getRandomStr(){return "";};

	virtual char getRandChar(int begin,int end);

	virtual AosJimoPtr cloneJimo() const;

};
#endif

