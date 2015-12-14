////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// 1. Length Pick: this is a random integer generator.
// 2. Prefix charaset and length picker
// 3. Body
// 4. Postfix charset and length picker
//
//
// Modification History:
// 2014/08/21 Created by xianghui,qi
////////////////////////////////////////////////////////////////////////////
#include "RandomGen/RandomGenCellPhone.h"
#include "Rundata/Rundata.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosRandomGenCellPhone_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRandomGenCellPhone(version);
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



AosRandomGenCellPhone::AosRandomGenCellPhone(const int version)
:
AosRandomGen(version)
{
}


AosRandomGenCellPhone::AosRandomGenCellPhone(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)// confirm the config is right
:
AosRandomGen(1)
{
	if (!config(rdata.getPtr(), conf))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosRandomGenCellPhone::~AosRandomGenCellPhone()
{
}


bool
AosRandomGenCellPhone::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &conf)//get the info min and max
{
	// 	<def record_num="200" len="11">
	// 	<recrod2  str="3" weight="33">
	// 	<record3  str="0123456789"/>
	// 	</record2>
	// 	
	// 	<record2  str="5" weight="33">
	// 	<record3  str="012356789"/>
	// 	</record2>
	//
	// 	<record2  str="8" weight="33">
	//  <record3  str="01235679"/>
	// 	</record2>
	//
	// 	<recordlast str="0123456789">
	// 	</def>
	mRecordNum=conf->getAttrInt("record_num",-1);
	aos_assert_r(mRecordNum>0,false);
	mLen=conf->getAttrInt("len",-1);
	aos_assert_r(mLen>0,false);
	AosXmlTagPtr record2=conf->getFirstChild("record2");
	struct Record2 r2;
	struct Record3 r3;
	while(record2)
	{
		r2.str=record2->getAttrStr("str");
		aos_assert_r(r2.str!="",false);
		r2.weight=record2->getAttrInt("weight",-1);
		aos_assert_r(r2.weight>0,false);
		mRecord2.push_back(r2);
		AosXmlTagPtr record3=record2->getFirstChild("record3");
		aos_assert_rr(record3,rdata,false);
		r3.str=record3->getAttrStr("str");
		aos_assert_r(r3.str!="",false);
		mRecord3.push_back(r3);
		record2=conf->getNextChild("record2");
	}
	AosXmlTagPtr recordlast=conf->getFirstChild("recordlast");
	aos_assert_rr(recordlast,rdata,false);
	strLast=recordlast->getAttrStr("str");
	aos_assert_r(strLast!="",false);
	return true;
}


bool 
AosRandomGenCellPhone::generateData(
		AosRundata *rdata, 
		AosValueRslt &value_rslt)
{
	
	OmnString str,second,third,last;
    OmnString first="1";
	second=getSecondNumber();
	third=getThirdNumber();
	last=getLastNumber();
	first<<second;
	first<<third;
	first<<last;
	str=first;
	value_rslt.setCharStr1(str.data(), str.length(), false);
	return true;
}


char
AosRandomGenCellPhone::getRandChar(int begin,int end)//min and max means len;
{
    char c=(char)OmnRandom::nextInt(begin,end);
    return c;
}

OmnString AosRandomGenCellPhone::getSecondNumber()
{
	int i=0;
	int idx,ww=0;
	int n=mRecord2.size();
	int array[n];
	int k;
	for(;i<n;i++)
	{
		ww+=mRecord2[i].weight;
		array[i]=ww;
	}
	for(i=0;i<n;i++)
	{
		idx=rand()%ww;
		if(idx<array[i])
		{
			k=i;
			break;
		}
	}
	/*idx=rand()%ww;
	if(idx<array[0])
		k=0;*/
	OmnString secondStr=mRecord2[k].str;
	return secondStr;
}
OmnString AosRandomGenCellPhone::getThirdNumber()
{
	OmnString secordStrIdx=getSecondNumber();
	OmnString thirdStr="";
	char buf[2];
	int n=mRecord2.size();
	int i=0;
	int idx;
	//get the idx
	for(;i<n;i++)
	{
		if(secordStrIdx==mRecord2[i].str)
		{
			idx=i;
			break;
		}
	}
	OmnString thirdStrTest=mRecord3[idx].str;
	int len=thirdStrTest.length();
	int k=rand()%(len-1);
	buf[0]=thirdStrTest[k];
	buf[1]='\0';
	thirdStr<<buf;
	return thirdStr;
}
OmnString AosRandomGenCellPhone::getLastNumber()
{
	int i;
	int idx;
	int lastlen=mLen-3;
	char buf[lastlen];
	int len=strLast.length();
	for(i=0;i<lastlen;i++)
	{
		idx=rand()%(len-1);
		buf[i]=strLast[idx];
	}
	OmnString last="";
	buf[lastlen]='\0';
	last<<buf;
	return last;
}


AosJimoPtr 
AosRandomGenCellPhone::cloneJimo() const
{
	return OmnNew AosRandomGenCellPhone(*this);
}



