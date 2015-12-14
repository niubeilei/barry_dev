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
// 02/22/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "QueryUtil/FieldFilter.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Base64/Base64.h"
#include "CounterUtil/CounterUtil.h"
#include "IILUtil/IILUtil.h"
#include "QueryCond/QueryCond.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "Util/Opr.h"


AosFieldFilter::AosFieldFilter()
{
}


AosFieldFilter::AosFieldFilter(const AosXmlTagPtr &xml)
{
	mType = toEnum(xml->getAttrStr("type", "statvalue"));
	if (mType == eStatValue)
	{
		mStatValue.stat_field_idx = xml->getAttrInt("stat_field_idx", -1); 
		mStatValue.opr = AosOpr_toEnum(xml->getAttrStr("opr", "eq"));
	}
	OmnString fieldstr = xml->getAttrStr("fieldstr", ""); 
	if (fieldstr != "")
	{
		// xx1,xx2,xx3
		AosStrSplit::splitStrBySubstr(fieldstr.data(), ",", mFieldStr, 100);
	}
}


AosFieldFilter::~AosFieldFilter()
{
}

OmnString
AosFieldFilter::getTypeStr(const Type type)
{
	if (type == eStatValue)          return "statvalue";

	OmnAlarm << "error FilterType, type:" << type << enderr;
	return "";
}


AosFieldFilter::Type
AosFieldFilter::toEnum(const OmnString &type)
{
	if (type == "statvalue")          return eStatValue;

	OmnAlarm << "error FilterType, typestr:" << type << enderr;
	return eInvalid;
}


OmnString
AosFieldFilter::toString()
{
	OmnString str;
	str << "<fieldfilter type=\"" << getTypeStr(mType) << "\" ";
	if (mType == eStatValue) 
	{
		str << "stat_field_idx=\"" << mStatValue.stat_field_idx << "\" "  
			<< "opr=\"" << AosOpr_toStr(mStatValue.opr)<< "\" ";
	}

	str << "fieldstr=\"";
	for (u32 i = 0; i < mFieldStr.size(); i++)
	{
		if (i > 0)  str << ","; 
		str << mFieldStr[i]; 
	}
	str << "\" />";
	return str;
}


bool
AosFieldFilter::cond(const OmnString &key)
{
	aos_assert_r(key != "", false);
	if (mType == eStatValue)
	{
		//cn.js.time.sz  cn.time.js   time.js
		vector<OmnString> str;
		int n = AosStrSplit::splitStrBySubstr(key.data(), AOS_COUNTER_SEGMENT_SEP2, str, 100);
		aos_assert_r(n > 0 && (u32)n == str.size(), false);
		if (mStatValue.stat_field_idx >= n || mStatValue.stat_field_idx < 0) 
		{
			OmnAlarm << "error!!  " << key  << enderr;
			return true;
		}

		OmnString kk = str[mStatValue.stat_field_idx];
		for (u32 i = 0; i < mFieldStr.size(); i++)
		{
			// Chen Ding, 2014/02/27
			// if(AosIILUtil::valueMatch(kk, mStatValue.opr, mFieldStr[i], false)) return true;
			if(AosIILUtil::valueMatch(kk, mStatValue.opr, mFieldStr[i], "", false)) return true;
		}
		return false;
	}
	return true;
}


void
AosFieldFilter::setStatValue(
		const int &stat_field_idx,
		const AosOpr &opr)
{
	mStatValue.stat_field_idx = stat_field_idx;
	mStatValue.opr = opr;
}


void
AosFieldFilter::addFieldStr(const OmnString &str)
{
	mFieldStr.push_back(str);
}
