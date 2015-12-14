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
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryCond/CondRange.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryCond/CondNames.h"
#include "QueryCond/CondIds.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/TimeUtil.h"
#include "Util/DataTypes.h"
#include "XmlUtil/XmlTag.h"


AosCondRange::AosCondRange(const bool regflag)
:
AosQueryCond(AOSCONDTYPE_RANGE, AosCondId::eRange, regflag),
mOpr(eAosOpr_Invalid)
{
}


AosCondRange::~AosCondRange()
{
}


bool
AosCondRange::parse(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	// The condition should be:
	// 	<cond type="xxx">
	// 		<max inclusive="true|false"><![CDATA[xxx]]></max>
	// 		<min inclusive="true|false"><![CDATA[xxx]]></min>
	// 	</cond>
	//
	// It defines a range: [min, max]. If min is not specified, it defaults
	// to no min, or it becomes [-, max]. If max is not specified, it defaults
	// to no max. 
	// 1. If both min and max not specified, it is equivalent to "any". 
	// 2. If min not specified, [-1, max].
	// 3. If max not specified, [min, -1]. 
	// 4. Otherwise, it is [min, max]. 
	//
	// In addition, whether it is inclusive or not is determined by 
	//
	aos_assert_rr(def, rdata, false);

	mMin = def->xpathQuery("min/_#text");
	mMax = def->xpathQuery("max/_#text");

	if ((mMin == "") && (mMax == ""))
	{
		mOpr = eAosOpr_an;
		return true;
	}
	mLeftInclusive = (def->xpathQuery("min/inclusive") == "true");
	mRightInclusive = (def->xpathQuery("max/inclusive") == "true");

	if (mMin == "")
	{
		// This means it is [-1, max]. 
		if (mRightInclusive) 
		{
			mOpr = eAosOpr_le;
		}
		else
		{
			mOpr = eAosOpr_lt;
		}
		return true;
	}

	if (mMax == "")
	{
		// This means it is [min, -1]
		if (mRightInclusive) 
		{
			mOpr = eAosOpr_ge;
		}
		else
		{
			mOpr = eAosOpr_gt;
		}
		return true;
	}

	// else means it is [min,max]
	// mOpr = eAosOpr_range;
	// Chen Ding, CHENDING20130920
	mOpr = eAosOpr_range_ge_le;	
	return true;
}


bool
AosCondRange::toString(OmnString &str)
{
	str << "<cond type=\"" << AosOpr_toStr(mOpr) << "\" >"
		<< "<min><![CDATA[" << mMin << "]]></min>"
		<< "<max><![CDATA[" << mMax << "]]></max>"
		<< "</cond>";
	return true;
}

