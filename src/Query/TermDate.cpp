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
// 04/11/2011 Created by Linda
////////////////////////////////////////////////////////////////////////////
#include "Query/TermDate.h"

#include "SEUtil/IILName.h"
#include "Alarm/Alarm.h"
#include "Query/TermInrange.h"
#include "Query/TermArith.h"
#include "Query/TermTag.h"
#include "Query/TermAnd.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "Util/Opr.h"
#include "Util/StrSplit.h"
#include "UtilTime/TimeInfo.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"



AosTermDate::AosTermDate(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_DATE, AosQueryType::eDate, regflag)
{
}


AosTermDate::AosTermDate(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_DATE, AosQueryType::eDate, false)
{
	//<term type="date" reverse="false" order="false>
	//	<selector zky_dtype="1" aname="zky_ctmeop__d" zky_num="10" zky_mode="h|f" zky_ttype="y|m|d|w|H|M|S"/>
	//</term>
	//
	//<term type="date" reverse="false" order="false>
	//	<selector zky_dtype="2" aname="zky_ctmepo__d" zky_start="1337681267" zky_end="1337681267"/>
	//</term>
	aos_assert(def);
	AosXmlTagPtr selector = def->getFirstChild("selector");
	aos_assert(selector);

	OmnString tt = selector->getAttrStr("zky_dtype", "1");
	i64 start=0, end=0;
	if (tt == "1")
	{
		i64 num = selector->getAttrInt64("zky_num", 0);
		OmnString mode = selector->getAttrStr("zky_mode", "h");
		OmnString type = selector->getAttrStr("zky_ttype", "d");

		num = mode=="h"?(-1*num):num;
		AosQrUtil::calculateTimeToEpoch(num, type, start, end, rdata);
		aos_assert(start !=0 && end != 0);

		if (start < end)
		{
			mStartTime = start;
			mEndTime = end;
		}
		else
		{
			mStartTime = end;
			mEndTime = start;
		}
	}
	else if (tt == "2")
	{
		start = selector->getAttrU64("zky_start", 0);
		end = selector->getAttrU64("zky_end", 0);
		aos_assert(start != 0 && end != 0 && start<=end);
		mStartTime = start;
		mEndTime = end;
	}
	mReverse = selector->getAttrBool("reverse", false);
	mOrder = selector->getAttrBool("order", false);
	OmnString attr = selector->getAttrStr("aname", "");
	aos_assert(attr!= "");
	mIILName = AosIILName::composeAttrIILName(attr);
	mIsGood = true;
}

void
AosTermDate::toString(OmnString &str)
{
	str << "<arith iilname=\"" << mIILName
		<< "\" reverse=\"" << mReverse
		<< "\" order=\"" << mOrder
		<< "\" start=\"" << mStartTime
		<< "\" end=\"" << mEndTime << "\"/>";
}


bool 
AosTermDate::convertQuery(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosQueryTermObjPtr
AosTermDate::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermDate(def, rdata);
}


bool
AosTermDate::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
OmnScreen << "*****************TermDate getDocidsFromIIL" << endl;
	query_rslt->setWithValues(true);
	query_context->setOpr(eAosOpr_range_ge_le);
	query_context->setStrValue(OmnStrUtil::ulltoa(mStartTime));
	query_context->setStrValue2(OmnStrUtil::ulltoa(mEndTime));
	AosQueryColumn(mIILName, query_rslt, 0, query_context, rdata);
	return true; 
}






















