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
// 2014/08/27 Created by xianghui qi
////////////////////////////////////////////////////////////////////////////
#include "RandomGen/RandomGenIntEnum.h"
#include "Rundata/Rundata.h"

extern "C"
{ 
	AosJimoPtr AosCreateJimoFunc_AosRandomGenIntEnum_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosRandomGenIntEnum(version);
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
AosRandomGenIntEnum::AosRandomGenIntEnum(const int version)
:AosRandomGen(version)
{
}


AosRandomGenIntEnum::AosRandomGenIntEnum(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)// confirm the config is right
:AosRandomGen(1)
{
	if (!config(rdata.getPtr(), conf))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosRandomGenIntEnum::~AosRandomGenIntEnum()
{
}


bool
AosRandomGenIntEnum::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &conf)//get the info min and max
{
	// 	<def record_num="200">
	// 	<record int="12",weight="20">
	// 	<record int="33",weight="30">
	// 	<record int="77",weight="50">
	//  </def>	
	mRecordNum=conf->getAttrInt("record_num",-1);
	aos_assert_r(mRecordNum>0,false);
	struct Record info;
	AosXmlTagPtr record=conf->getFirstChild("record");
	aos_assert_rr(record,rdata,false);
	while(record)
	{
		info.key=record->getAttrInt("int",-1);
		aos_assert_r(info.key>0,false);
		info.weight=record->getAttrInt("weight",-1);
		aos_assert_r(info.weight>0,false);
		mRecord.push_back(info);
		record=conf->getNextChild();
	}
	return true;
}


bool 
AosRandomGenIntEnum::generateData(
		AosRundata *rdata, 
		AosValueRslt &value_rslt)
{	
	int key;
	int i=0,k,idx,ww=0;
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
	key=mRecord[i].key;
	//value_rslt.setCharStr1(str.data(), str.length(), false);
	value_rslt.setValue(key);
	return true;
}


//char
//AosRandomGenIntEnum::getRandChar(int begin,int end)
//{
//    char c=(char)OmnRandom::nextInt(begin,end);
//    return c;
//}
AosJimoPtr
AosRandomGenIntEnum::cloneJimo() const
{
	return OmnNew AosRandomGenIntEnum(*this);
}


