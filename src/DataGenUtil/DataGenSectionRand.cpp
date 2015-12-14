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
// This is a random number generator. The input is:
// 		[start, stop, weight]
// 		[start, stop, weight]
// 		...
// 		[start, stop, weight]
//
// It generates a random number by first randomly determine which section
// to use, and then randomly generate a number in that section.
//
// Modification History:
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenUtil/DataGenSectionRand.h"

#include "ErrorMgr/ErrmsgId.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"

AosDataGenSectionRand::AosDataGenSectionRand(const bool reg)
:
AosDataGenUtil(AOSDATGGENTYPE_SECTION_RAND, AosDataGenUtilType::eSectionRand, reg)
{
}

AosDataGenSectionRand::AosDataGenSectionRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
AosDataGenUtil(AOSDATGGENTYPE_SECTION_RAND, AosDataGenUtilType::eSectionRand, false),
mWeights(0),
mStart(0),
mEnd(0)
{
	aos_assert(parse(config, rdata));
}

AosDataGenSectionRand::AosDataGenSectionRand()
:
AosDataGenUtil(AOSDATGGENTYPE_SECTION_RAND, AosDataGenUtilType::eSectionRand, false)
{
}

AosDataGenSectionRand::~AosDataGenSectionRand()
{
}


bool
AosDataGenSectionRand::parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	aos_assert_r(config, false);
	mStart.clear();
	aos_assert_r(mStart.empty(), false);
	mEnd.clear();
	aos_assert_r(mEnd.empty(), false);
	mWeights.clear();
	aos_assert_r(mWeights.empty(), false);
	int i = 0;
	AosXmlTagPtr tag = config->getFirstChild();
	while(tag)
	{
		u64 start = tag->getAttrU64("zky_min", 0); 
		mStart.push_back(start);
		u64 end = tag->getAttrU64("zky_max", 0);
		mEnd.push_back(end);
		int weights = tag->getAttrInt("zky_weights",0);
		for (int j = 0; j<weights; j++)
			mWeights.push_back(i);
		i++;
		tag = config->getNextChild();
	}
	return true;
}


bool
AosDataGenSectionRand::nextValue(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// <sdoc zky_repeatable="true|false" >
	// <xxx zky_min="xxx" zky_max="xxx" zky_weights="xxx">
	// <xxx zky_min="xxx" zky_max="xxx" zky_weights="xxx">
	// ...
	// </xxx>
	
	//value.reset();
	aos_assert_r(sdoc, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorUser(rdata, "data_type_incorrect") << datatype << enderr;
		return false;
	}

	OmnString vv;
	int idx = procWeights(sdoc);
	AosXmlTagPtr tag = sdoc->getChild(idx);
	aos_assert_rr(tag, rdata, false);
	return  AosDataGenUtil::nextValueStatic(value, tag, rdata);
}


int
AosDataGenSectionRand::procWeights(const AosXmlTagPtr &sdoc)
{
	aos_assert_r(sdoc, false);
	u64 Array[eMax];
	u64 sum = 0;
	int i = 0;
	AosXmlTagPtr tag = sdoc->getFirstChild();
	while(tag)
	{
		u64 weights = tag->getAttrU64("zky_weights", 0);
		aos_assert_r(i < eMax, false);
		if (i == 0) Array[i] = weights;
		else Array[i] = Array[i-1] + weights;
		sum += weights; 
		tag = sdoc->getNextChild();
		i++;
	}

	int idx = 0;
	int num = sdoc->getNumSubtags();
	aos_assert_r(i == num ,false);
	u64 rr = rand()%sum;
	for (int j = 0; j < num; j++)
	{
		if (rr <= Array[j])
		{
			idx = j; 
			break;
		}
	}
	return idx;
}

bool
AosDataGenSectionRand::nextValue(AosValueRslt &value, const AosRundataPtr &rdata)
{
	aos_assert_r(mWeights.size() > 0, false);
	int idx = rand() % mWeights.size();
	aos_assert_r(idx >= 0 && (u32)idx < mWeights.size(), false);
	idx = mWeights[idx];
	aos_assert_r(idx >= 0 && (u32)idx < mEnd.size() && (u32)idx < mStart.size(), false);
	int64_t size = mEnd[idx] - mStart[idx];
	int64_t vv = 0;
	if (size > 0)
	{
		vv = rand() % size + mStart[idx];
	}
	else
	{
		vv = mStart[idx];
	}
	value.setI64(vv);
	return true;
}

AosDataGenUtilPtr 
AosDataGenSectionRand::clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	return OmnNew AosDataGenSectionRand(config, rdata);
}
