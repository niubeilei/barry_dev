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
#ifndef Omn_Random_RandomGenStr_h
#define Omn_Random_RandomGenStr_h

#include "RandomGen/RandomGen.h"
#include "Util/RCObjImp.h"
#include <vector>
class AosRandomGenStr:public AosRandomGen
{
	OmnDefineRCObject;

private:
	int mRecordNum;
	OmnString mPrebuf,mPostbuf;
	struct RecordLen
	{
		int min;
		int max;
		int weight;
	};
	struct RangeStr
	{
		int begin;
		int end;
		int weight;
	};
	struct PreStr
	{
		int min;
		int max;
		int weight;
	};
	struct PostStr
	{
		int min;
		int max;
		int weight;
	};
	vector<RecordLen>    mRecordlen;
	vector<RangeStr>     mRange;
	vector<PreStr>       mPreStr;
	vector<PostStr>      mPostStr;
public:
	AosRandomGenStr(const int version);
	AosRandomGenStr(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	~AosRandomGenStr();
    virtual bool config( 	AosRundata *rdata, 
				            const AosXmlTagPtr &conf);
	virtual bool generateData(AosRundata *rdata, 
						AosValueRslt &value_rslt);
    //virtual OmnString getRandomStr();

	virtual char getRandChar(int begin,int end);

	virtual OmnString getPreStr();

	virtual OmnString getPostStr();

	virtual int getBodyLength();
	virtual int getPreLength();
	virtual int getPostLength();
	virtual AosJimoPtr cloneJimo() const;
};
#endif

