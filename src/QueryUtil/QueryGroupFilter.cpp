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
// Modification History:
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryUtil/QueryGroupFilter.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Base64/Base64.h"
#include "IILUtil/IILUtil.h"
#include "QueryCond/QueryCond.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "Util/Opr.h"


AosQueryGroupFilter::AosQueryGroupFilter(const AosQueryGroupFilterPtr &rhs)
{
	mGroupFilterType = rhs->mGroupFilterType;
	mOpr = rhs->mOpr;
	// Chen Ding, 2014/02/26
	// mStrValue = rhs->mStrValue;
	// mU64Value = rhs->mU64Value;
	mStrValue1 = rhs->mStrValue1;
	mStrValue2 = rhs->mStrValue2;
	mU64Value1 = rhs->mU64Value1;
	mU64Value2 = rhs->mU64Value2;
	mSaperator = rhs->mSaperator;
	mCreateDefault = rhs->mCreateDefault;
	mValueType = rhs->mValueType;	
}


AosQueryGroupFilter::AosQueryGroupFilter(const AosXmlTagPtr &xml)
:
mGroupFilterType(AosQueryContextObj::eGroupFilterKey),
mOpr(eAosOpr_an),
mU64Value1(0),
mU64Value2(0),
mCreateDefault(false),
mValueType(eStr)
{
	mGroupFilterType = (AosQueryContextObj::GroupFilterType)(xml->getAttrInt(
				"filterkey",(int)AosQueryContextObj::eGroupFilterKey));
	mOpr = AosOpr_toEnum(xml->getAttrStr("opr","eq"));
	mStrValue1 = xml->getAttrStr("strValue","");
	mStrValue2 = xml->getAttrStr("strValue2","");
	mSaperator = xml->getAttrStr("saperator","");
	mU64Value1 = xml->getAttrU64("u64Value",0);
	mU64Value2 = xml->getAttrU64("u64Value2",0);
	mCreateDefault = xml->getAttrBool("createdefault",false);
	mValueType = (ValueType)(xml->getAttrInt("valuetype",(int)eStr));
	
	//OmnScreen << "query filter:" << xml->toString() << endl;
	if (mGroupFilterType <= AosQueryContextObj::eInvalidGroupFilterType || 
		mGroupFilterType >= AosQueryContextObj::eMaxGroupFilterType)
	{
		OmnAlarm << "Invalid filter Type: " << mGroupFilterType << enderr;
	}
}

AosQueryGroupFilter::AosQueryGroupFilter(
			const AosQueryContextObj::GroupFilterType filterType,
			const AosOpr	 opr,
			const OmnString	 &value1,
			const OmnString	 &value2,
			const OmnString	 &saperator,
			const bool	     createDefault) 
:
mGroupFilterType(filterType),
mOpr(opr),
mStrValue1(value1),
mStrValue2(value2),
mU64Value1(0),
mU64Value2(0),
mSaperator(saperator),
mCreateDefault(createDefault),
mValueType(eStr)
{
	if (mGroupFilterType <= AosQueryContextObj::eInvalidGroupFilterType || 
		mGroupFilterType >= AosQueryContextObj::eMaxGroupFilterType)
	{
		OmnAlarm << "Invalid filter Type: " << mGroupFilterType << enderr;
	}
}
			

AosQueryGroupFilter::AosQueryGroupFilter(
			const AosQueryContextObj::GroupFilterType filterType,
			const AosOpr	 opr,
			const u64		 &value1,
			const u64		 &value2,
			const OmnString	 &saperator,
			const bool	     createDefault) 
:
mGroupFilterType(filterType),
mOpr(opr),
mU64Value1(value1),
mU64Value2(value2),
mSaperator(saperator),
mCreateDefault(createDefault),
mValueType(eU64)
{
	if (mGroupFilterType <= AosQueryContextObj::eInvalidGroupFilterType || 
		mGroupFilterType >= AosQueryContextObj::eMaxGroupFilterType)
	{
		OmnAlarm << "Invalid filter Type: " << mGroupFilterType << enderr;
	}
}


AosQueryGroupFilter::~AosQueryGroupFilter()
{
}


OmnString	
AosQueryGroupFilter::toString()const
{
	OmnString str;
	str << "<query_group_filter ";

	if(mGroupFilterType != AosQueryContextObj::eGroupFilterKey)str << "filterkey=\"" << (int)mGroupFilterType << "\" ";
	if(mOpr != eAosOpr_eq)str << "opr=\"" << AosOpr_toStr(mOpr)<< "\" ";
	if(mStrValue1 != "")str << "strValue1=\"" << mStrValue1 << "\" "; 
	if(mStrValue2 != "")str << "strValue1=\"" << mStrValue2 << "\" "; 
	if(mU64Value1) str << "u64Value1=\"" << mU64Value1 << "\" ";  
	if(mU64Value2) str << "u64Value2=\"" << mU64Value2 << "\" ";  
	if(mSaperator != "")str << "saperator=\"" << mSaperator<< "\" "; 
	if(mCreateDefault)str << "createdefault=\"" << mCreateDefault << "\" "; 
	if(mValueType!= eStr)str << "valuetype=\"" << (int)eU64<< "\" "; 

	str << " />";
		
	//OmnScreen << "query filter:" << str << endl;
	return str;
}


bool		
AosQueryGroupFilter::isValid(
		const OmnString &value,
		bool &createDefault, 
		const bool num_alpha)
{
	switch(mGroupFilterType)
	{
		case AosQueryContextObj::eGroupFilterKey:
			if(AosIILUtil::valueMatch(value.data(),mOpr,mStrValue1, mStrValue2, num_alpha))
			{
				return true;
			}
			break;
		case AosQueryContextObj::eGroupFilterFirstStrKey:
			{
				// get first key
				vector<OmnString> keys;
				AosStrSplit::splitStrByChar(value, mSaperator.data(), keys, 4);
				if(keys.size() <= 0) break;
				if(AosIILUtil::valueMatch(keys[0].data(),mOpr,mStrValue1, mStrValue2, num_alpha))
				{
					return true;
				}
			}				
			break;
		case AosQueryContextObj::eGroupFilterSecondStrKey:
			{
				// get second key
				vector<OmnString> keys;
				AosStrSplit::splitStrByChar(value, mSaperator.data(), keys, 4);
				if(keys.size() <= 1) break;
				if(AosIILUtil::valueMatch(keys[1].data(),mOpr,mStrValue1, mStrValue2, num_alpha))
				{
					return true;
				}
			}				
			break;
		case AosQueryContextObj::eGroupFilterFirstU64Key:
			{
				// get first key
				vector<OmnString> keys;
				AosStrSplit::splitStrByChar(value, mSaperator.data(), keys, 4);
				if(keys.size() <= 0) break;
				if(AosIILUtil::valueMatch(atoll(keys[0].data()),mOpr,mU64Value1, mU64Value2))
				{
					return true;
				}
			}				
			break;
		case AosQueryContextObj::eGroupFilterSecondU64Key:
			{
				// get second key
				vector<OmnString> keys;
				AosStrSplit::splitStrByChar(value, mSaperator.data(), keys, 4);
				if(keys.size() <= 1) break;
				if(AosIILUtil::valueMatch(atoll(keys[1].data()),mOpr,mU64Value1, mU64Value2))
				{
					return true;
				}
			}				
			break;

		default:
			OmnAlarm << "Invalid filter type: " << mGroupFilterType << enderr;
			break;
	}
	createDefault = mCreateDefault;
	return false;
}


bool		
AosQueryGroupFilter::isValid(const u64 &value,bool &createDefault)
{
	if(AosIILUtil::valueMatch(value,mOpr,mU64Value1, mU64Value2))
	{
		return true;
	}
	createDefault = mCreateDefault;
	return false;
}

	
AosQueryGroupFilterPtr
AosQueryGroupFilter::clone() const
{
	AosQueryGroupFilterPtr thisptr((AosQueryGroupFilter*)this, false);
	AosQueryGroupFilterPtr filter = OmnNew AosQueryGroupFilter(thisptr);
	return filter;
}

