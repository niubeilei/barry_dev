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
#ifndef Omn_Random_RandomGenCellPhone_h
#define Omn_Random_RandomGenCellPhone_h

#include "RandomGen/RandomGen.h"
#include "Util/RCObjImp.h"
#include <vector>



class AosRandomGenCellPhone : public AosRandomGen
{
	OmnDefineRCObject;

private:
	int				mRecordNum;
	int				mLen;
	OmnString       strLast;
    struct Record2
	{
		OmnString str;
		int       weight;
	};
	struct Record3
	{
		OmnString str;
	};
	vector<Record2> mRecord2;
	vector<Record3> mRecord3;
public:
	AosRandomGenCellPhone(const int version);
	AosRandomGenCellPhone(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	~AosRandomGenCellPhone();
    virtual bool config( 	AosRundata *rdata, 
				            const AosXmlTagPtr &conf);
	char
		 getRandChar(int begin,int end);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool generateData(AosRundata *rdata, 
				 AosValueRslt &value_rslt);
	OmnString getSecondNumber();
	OmnString getThirdNumber();
	OmnString getLastNumber();
};
#endif

