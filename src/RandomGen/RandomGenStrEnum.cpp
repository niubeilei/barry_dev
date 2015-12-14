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
// 	[str1, str2, ..., str1]
// 	[male, female]
//
// Picker: randomly pick a value from the above list.
//
//
// Modification History:
// 2014/08/26 Created by xianghui qi
////////////////////////////////////////////////////////////////////////////
#include "RandomGen/RandomGenStrEnum.h"
#include "Rundata/Rundata.h"

extern "C"
 {
	 AosJimoPtr AosCreateJimoFunc_AosRandomGenStrEnum_1(const AosRundataPtr &rdata, const int version)
	 {
		 try
		 {
			 OmnScreen << "To create Jimo: " << endl;
			 AosJimoPtr jimo = OmnNew AosRandomGenStrEnum(version);
			 aos_assert_r(jimo, 0);
			 return jimo;
		 } 
		 catch (...)
		 {
			 AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			 return 0;
	     }
				   
		 OmnShouldNeverComeHere;
		 return 0;
	}
}
AosRandomGenStrEnum::AosRandomGenStrEnum(const int version)
:
AosRandomGen(version)
{
}


AosRandomGenStrEnum::AosRandomGenStrEnum(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)// confirm the config is right
:
AosRandomGen(1)
{
	if (!config(rdata, conf))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosRandomGenStrEnum::~AosRandomGenStrEnum()
{
}


bool
AosRandomGenStrEnum::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)//get the info min and max
{
	// 	<def record_num="200">
	// 		<record str="qi xiang hui",weight="20">
	// 		<record str="xiang hui qi",weight="30">
	// 		<record str="hui xiang qi",weight="50">
	//  </def>	
	mRecordNum=conf->getAttrInt("record_num",-1);
	aos_assert_r(mRecordNum>0,false);
	struct Record info;
	AosXmlTagPtr record=conf->getFirstChild("record");
	aos_assert_rr(record,rdata,false);
	while(record)
	{
		info.str=record->getAttrStr("str");
		aos_assert_r(info.str!="",false);
		info.weight=record->getAttrInt("weight",-1);
		aos_assert_r(info.weight>0,false);
		mRecord.push_back(info);
		record=conf->getNextChild();
	}
	return true;
}


bool 
AosRandomGenStrEnum::generateData(
		AosRundata *rdata, 
		AosValueRslt &value_rslt)
{	
	OmnString str;
	int i=0, k = 0, idx,ww=0;
	int n=mRecord.size();
	int array[n];
	for(;i<n;i++)
	{
		ww+=mRecord[i].weight;
		array[i]=ww;
	}
	for(i=0;i<n;i++)
	{
		idx=rand()%(ww-1);
		if(idx<array[i])
		{
			k=i;
			break;
		}
	}
	str=mRecord[i].str;
	value_rslt.setCharStr1(str.data(), str.length(), false);
	return true;
}


char
AosRandomGenStrEnum::getRandChar(int begin,int end)
{
    char c=(char)OmnRandom::nextInt(begin,end);
    return c;
}

AosJimoPtr
AosRandomGenStrEnum::cloneJimo() const
{
	return OmnNew AosRandomGenStrEnum(*this);
}


