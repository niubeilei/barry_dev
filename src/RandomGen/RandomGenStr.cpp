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
#include "RandomGen/RandomGenStr.h"
#include "Rundata/Rundata.h"

extern "C"
{ 
	AosJimoPtr AosCreateJimoFunc_AosRandomGenStr_1(const AosRundataPtr &rdata, const int version)
	{
		try                                                                                                            
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosRandomGenStr(version);
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
AosRandomGenStr::AosRandomGenStr(const int version)
:
AosRandomGen(version)
{
}


AosRandomGenStr::AosRandomGenStr(
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


AosRandomGenStr::~AosRandomGenStr()
{
}


bool
AosRandomGenStr::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &conf)//get the info min and max
{
	// 	<def record_num="200">
	// 	  <body>
	// 	    <recordlen>
	// 		    <recordlength min="0"  max="10" weight="10"/>
	// 		    <recordlength min="3"  max="34" weight="40"/>
	// 		    <recordlength min="20" max="66" weight="50">
	//      </recordlen>
	// 		<ranges>
	//			<range begin="32" end="47"  weight="60"/> 
    //    		<range begin="47" end="57"  weight="30"/> 
    //			<range begin="65" end="124" weight="30"/>
	//		</ranges>
	//	  </body>
	//
	//	  <pre>
	//	       <prestr str="iuueddp0_85"/>
	//	       <prelength min="0" max="10" weight="60"/>
	//	       <prelength min="6" max="12" weight="90"/>
	//	       <prelength min="1" max="5"  weight="30"/>
	//	  </pre>
	// 
	//	  <post>
	//	       <poststr str="ijjkkld998_3"/>
	//	       <postlength min="0" max="8"  weight="40"/>
	//	       <postlength min="4" max="10" weight="49"/>
	//	       <postlength min="3" max="6"  weight="30"/>
	//	  </post>
	// 	</def>
	mRecordNum=conf->getAttrInt("record_num",-1);
	aos_assert_r(mRecordNum>=0,false);
	AosXmlTagPtr body=conf->getFirstChild("body");
	aos_assert_rr(body,rdata,false);
	AosXmlTagPtr recordlen=body->getFirstChild("recordlen");
	aos_assert_rr(recordlen,rdata,false);
	AosXmlTagPtr recordlength=recordlen->getFirstChild("recordlength");
	aos_assert_rr(recordlength,rdata,false);
	struct RecordLen record;
    while(recordlength)
	{
		record.min=recordlength->getAttrInt("min",-1);
		aos_assert_r(record.min>=0,false);
		record.max=recordlength->getAttrInt("max",-1);
		aos_assert_r(record.max>record.min,false);
		record.weight=recordlength->getAttrInt("weight",-1);
		aos_assert_r(record.weight>=0,false);
		mRecordlen.push_back(record);
		recordlength=recordlen->getNextChild();
	}

	struct RangeStr rangestr;
	AosXmlTagPtr ranges=body->getFirstChild("ranges");
	aos_assert_rr(ranges,rdata,false);
	AosXmlTagPtr range=ranges->getFirstChild("range");
	aos_assert_rr(range,rdata,false);
	while(range)
	{
		rangestr.begin=range->getAttrInt("begin",-1);
		aos_assert_r(rangestr.begin>=0,false);
		rangestr.end=range->getAttrInt("end",-1);
		aos_assert_r(rangestr.end>rangestr.begin,false);
		rangestr.weight=range->getAttrInt("weight",-1);
		aos_assert_r(rangestr.weight>=0,false);
		mRange.push_back(rangestr);
		range=ranges->getNextChild();
	}

	AosXmlTagPtr pre=conf->getFirstChild("pre");
	aos_assert_rr(pre,rdata,false);
	AosXmlTagPtr prestr=pre->getFirstChild("prestr");
	aos_assert_rr(prestr,rdata,false);
	mPrebuf=prestr->getAttrStr("str");
	aos_assert_r(mPrebuf!="",false);
	AosXmlTagPtr prelength=pre->getFirstChild("prelength");
	aos_assert_rr(prelength,rdata,false);
	struct PreStr prestrnew;
	while(prelength)
	{
		prestrnew.min=prelength->getAttrInt("min",-1);
		aos_assert_r(prestrnew.min>=0,false);
		prestrnew.max=prelength->getAttrInt("max",-1);
		aos_assert_r(prestrnew.min<=prestrnew.max,false);
		prestrnew.weight=prelength->getAttrInt("weight",-1);
		aos_assert_r(prestrnew.weight>=0,false);
		mPreStr.push_back(prestrnew);
		prelength=pre->getNextChild();
	}

	AosXmlTagPtr post=conf->getFirstChild("post");
	aos_assert_rr(post,rdata,false);
	AosXmlTagPtr poststr=post->getFirstChild("poststr");
	aos_assert_rr(poststr,rdata,false);
	mPostbuf=poststr->getAttrStr("str");
	aos_assert_r(mPostbuf!="",false);
	AosXmlTagPtr postlength=post->getFirstChild("postlength");
	aos_assert_rr(postlength,rdata,false);
	struct PostStr poststrnew;
	while(postlength)
	{
		poststrnew.min=postlength->getAttrInt("min",-1);
		aos_assert_r(poststrnew.min>=0,false);
		poststrnew.max=postlength->getAttrInt("max",-1);
		aos_assert_r(poststrnew.max>=0,false);
		poststrnew.weight=postlength->getAttrInt("weight",-1);
		aos_assert_r(prestrnew.weight>=0,false);
		mPostStr.push_back(poststrnew);
		postlength=post->getNextChild();
	}
	return true;
}


bool 
AosRandomGenStr::generateData(
		AosRundata *rdata, 
		AosValueRslt &value_rslt)
{	
	OmnString str,prestr,poststr;
	int len=getBodyLength();
	int idx=0,k,i=0,ww=0;
	int n=mRange.size();
	int array[n];
	char buf[len];
	for(;i<n;i++)
	{
		ww+=mRange[i].weight;
		array[i]=ww;
	}
	for(i=0;i<len;i++)
	{
		idx=rand()%ww;
		if(idx<=mRange[0].weight)
			k=0;
		else if(idx<=mRange[0].weight+mRange[1].weight)
			k=1;
		else if(idx<=mRange[0].weight+mRange[1].weight+mRange[2].weight)
			k=2;
		buf[i]=getRandChar(mRange[k].begin,mRange[k].end);
	}
	buf[len]='\0';
	str<<buf;
	prestr=getPreStr();
	poststr=getPostStr();
	prestr<<str;
	prestr<<poststr;
	bool rslt = value_rslt.setCharStr1(prestr.data(), prestr.length(), true);
	aos_assert_r(rslt, false);

	return true;
}


char
AosRandomGenStr::getRandChar(int begin,int end)
{
    char c=(char)OmnRandom::nextInt(begin,end);
    return c;
}

int AosRandomGenStr::getBodyLength()
{
	int len=OmnRandom::intByRange(mRecordlen[0].min,mRecordlen[0].max,mRecordlen[0].weight,
			mRecordlen[1].min,mRecordlen[1].max,mRecordlen[1].weight,
			mRecordlen[2].min,mRecordlen[2].max,mRecordlen[2].weight);
	return len;
}
int AosRandomGenStr::getPreLength()
{
	int len=OmnRandom::intByRange(mPreStr[0].min,mPreStr[0].max,mPreStr[0].weight,
			mPreStr[1].min,mPreStr[1].max,mPreStr[1].weight,
			mPreStr[2].min,mPreStr[2].max,mPreStr[2].weight);
	return len;
}
int AosRandomGenStr::getPostLength()
{
	int len=OmnRandom::intByRange(mPostStr[0].min,mPostStr[0].max,mPostStr[0].weight,
			mPostStr[1].min,mPostStr[1].max,mPostStr[1].weight,
			mPostStr[2].min,mPostStr[2].max,mPostStr[2].weight);
	return len;
}
OmnString AosRandomGenStr::getPreStr()
{
	int len=getPreLength();
	int i=0,k;
	char buf[len];
	for(;i<len;i++)
	{
		k=rand()%(mPrebuf.length());
		buf[i]=mPrebuf[k];
	}
	buf[len]='\0';
	return buf;
}

OmnString AosRandomGenStr::getPostStr()
{
	int len=getPostLength();
	int i=0,k;
	//char buf[len];
	char *bufPost = new char[len];
	for(;i<len;i++)
	{
		k=rand()%(mPostbuf.length());
		bufPost[i]=mPostbuf[k];
	}
	bufPost[len]='\0';
	return bufPost;
}

AosJimoPtr 
AosRandomGenStr::cloneJimo() const
{
	return OmnNew AosRandomGenStr(*this);
}
