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
#include "Query/TermIILType.h"

#include "HtmlServer/HtmlCode.h"
#include "QueryCond/QueryCond.h"
#include "UtilTime/TimeUtil.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEUtil/IILName.h"


AosTermIILType::AosTermIILType(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_IILTYPE, AosQueryType::eIILType, regflag),
mDataLoaded(false),
mOpr(eAosOpr_Invalid),
mIsSuperIIL(false),
mValue2("")
{
}


AosTermIILType::AosTermIILType(
		const OmnString &name, 
		const AosQueryType::E type, 
		const bool regflag)
:
AosQueryTerm(name, type, regflag),
mDataLoaded(false),
mOpr(eAosOpr_Invalid),
mIsSuperIIL(false),
mValue2("")
{
}


// AosTermIILType::AosTermIILType(const OmnString &name, const AosQueryType::E type)
// :
// AosQueryTerm(name, type, false),
// mIILId(0),
// mDataLoaded(false),
// mOpr(eAosOpr_Invalid)
// {
// }


AosTermIILType::AosTermIILType(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_IILTYPE, AosQueryType::eIILType, false),
mDataLoaded(false),
mOpr(eAosOpr_Invalid),
mIsSuperIIL(false),
mValue2("")
{
}


AosTermIILType::~AosTermIILType()
{
	if (mQueryContext) AosQueryContextObj::returnQueryContextStatic(mQueryContext);
	mQueryContext = 0;
}


bool 	
AosTermIILType::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) 
	{
		docid = AOS_INVDID;
		finished = true;
		return true;
	}
	return mQueryRslt->nextDocid(docid, finished, rdata);
}


bool 	
AosTermIILType::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return 0;
	return mQueryRslt->moveTo(startidx, mNoMoreDocs, rdata);
}


i64		
AosTermIILType::getTotal(const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return 0;
	return mQueryRslt->getTotal(rdata);
}


bool
AosTermIILType::setOrder(
		const OmnString &container, 
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	if (name == mFieldname && mCtnrObjid == container)
	{
		mOrder = true;
		mReverse = reverse;
		return true;
	}
	return false;
}


bool
AosTermIILType::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// It checks whether the doci 'docid' can be selected by this term.
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return false;
	return mQueryRslt->checkDocid(docid, rdata);
}


void	
AosTermIILType::reset(const AosRundataPtr &rdata)
{
	if (!mQueryRslt) return;
	mQueryRslt->reset();
}


bool
AosTermIILType::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;
	mDataLoaded = true;
	if (!mQueryRslt)
	{
		mQueryRslt = AosQueryRsltObj::getQueryRsltStatic();
	}

	// Chen Ding, 01/02/2013
	if (!mQueryContext)
	{
		// Chen Ding, 2013/02/07
		// mQueryContext = AosQueryContextObj::createQueryContextStatic();
		mQueryContext = getQueryContext();
	}
	mQueryContext->setReverse(mReverse);
	return getDocidsFromIIL(mQueryRslt, 0, mQueryContext, rdata);
}

bool 	
AosTermIILType::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	// Chen Ding, 2013/02/07
	if(!mQueryContext) 
	{
		mQueryContext = getQueryContext();
	}

	// Chen Ding, 2013/02/07
	// mQueryContext->setOpr(mOpr);
	// mQueryContext->setStrValue(mValue);
	// mQueryContext->setU64Value(atoll(mValue.data()));
	// mQueryContext->setReverse(mReverse);
	// Linda, 2012/12/19
	// if (mValue2 != "")
	// {
	// 	mQueryContext->setStrValue2(mValue2);
	// 	mQueryContext->setU64Value2(atoll(mValue2.data()));
	// }
	
	bool rslt = false;
	if (mIILId)
	{
		//rslt = AosIILClient::getSelf()->preQuerySafe(mIILId, mQueryContext, rdata);
		rslt = AosIILClientObj::getIILClient()->preQuerySafe(mIILId, mQueryContext, rdata);
	}else
	{
		//rslt = AosIILClient::getSelf()->preQuerySafe(mIILName, mQueryContext, rdata);
		rslt = AosIILClientObj::getIILClient()->preQuerySafe(mIILName, mQueryContext, rdata);
	}
	setTotalInRslt(mQueryContext->getTotalDocInRslt());
	return rslt;
}

bool
AosTermIILType::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(AosOpr_valid(mOpr), false);
	if (query_context)
	{
		// Chen Ding, 2013/02/07
		setQueryContext(query_context);
		// query_context->setOpr(mOpr);
		// query_context->setStrValue(mValue);
		// query_context->setU64Value(atoll(mValue.data()));
		// query_context->setReverse(mReverse);
		// Linda, 2012/12/19
		// if (mValue2 != "")
		// {
		// 	query_context->setStrValue2(mValue2);
		// 	query_context->setU64Value2(atoll(mValue2.data()));
		// }
	}
	else
	{
		// Chen Ding, 2013/02/09
		if (!mQueryContext) mQueryContext = getQueryContext();
	}
	
	if (mIILId)
	{
		return AosQueryColumn(mIILId, query_rslt, query_bitmap, query_context, rdata);
	}

	return AosQueryColumn(mIILName, query_rslt, query_bitmap, query_context, rdata);
}


AosQueryTermObjPtr
AosTermIILType::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermIILType(def, rdata);
}


void
AosTermIILType::toString(OmnString &str)
{
	OmnString rr = (mReverse)?"true":"false";
	OmnString odr = (mOrder)?"true":"false";
	str << "<term type=\"" << AOSTERMTYPE_IILTYPE << "\" "
		<< "reverse=\"" << rr << "\" "
		<< "order=\"" << odr<< "\" "
		<< "iilid=\"" << mIILId << "\" "
		<< "iilname=\"" << mIILName << "\" "
		<< "opr=\"" << mOpr << "\" "
		<< "value=\"" << mValue << "\"/>";
}


bool
AosTermIILType::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
	/*
	if (mIILId)
	{
		return AosIILClient::getSelf()->collectInfo(
				 mIILId, mOpr, mValue, mCondInfo, rdata);
	}

	return AosIILClient::getSelf()->collectInfo(
				 mIILName, mOpr, mValue, mCondInfo, rdata);
	*/
}


bool
AosTermIILType::retrieveContainers(
		const AosXmlTagPtr &def,
		AosHtmlCode &code,
		const AosRundataPtr &rdata)
{
	// This function retrieves the container used by this condition. 
	// It assumes the term 'def' is in the following format:
	// 	<term type="xxx" ...>
	// 		<selector type="xxx"
	// 			container="xxx"
	// 			.../>
	//		<cond .../>
	//	</term>
	aos_assert_rr(def, rdata, false);
	OmnString container = def->getAttrStr(AOSTAG_CONTAINER_NOZK);
	if (container == "") return true;

	// Found a container. 
	code.addContainer(container);
	return true;
}


// Chen Ding, 01/03/2013
AosQueryContextObjPtr 
AosTermIILType::getQueryContext() const
{
	AosQueryContextObjPtr context = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(context, 0);

	bool rslt = setQueryContext(context);
	aos_assert_r(rslt, 0);
	return context;
}


bool
AosTermIILType::setQueryContext(const AosQueryContextObjPtr &context) const
{
	aos_assert_r(context, false);
	context->setOpr(mOpr);
	context->setStrValue(mValue);
	context->setReverse(mReverse);

	if (mValue2 != "")
	{
		context->setStrValue2(mValue2);
	}

	context->clearFilter();
	if (!mFilters.empty())
	{
		for (u32 i=0; i<mFilters.size(); i++)
		{
			context->addFilter(mFilters[i]);
		}
	}
	
	return true;
}


// Chen Ding, 2014/01/29
bool 
AosTermIILType::getTimeCond(
		const AosRundataPtr &rdata, 
		const OmnString &time_fname, 
		int &num_matched,
		int *start_days, 
		int *end_days)
{
	if (time_fname != mFieldname) 
	{
		num_matched = 0;
		return true;
	}

	// The value must be a time value. It should have been converted
	// into Epoch Time (string). 
	i64 value=0, value2=0;
	// Ketty Temp 2014/02/17
	if(mFormat1 == "") mFormat1 = AOSTIMEFORMAT_YYYY_MM_DDHHMMSS;	
	if (!AosTimeUtil::str2Time(rdata, mFormat1, mValue, value)) return false;

	num_matched = 0;
	switch (mOpr)
	{
	case eAosOpr_gt:		
		 start_days[0] = value+1;
		 end_days[0] = AosTimeUtil::eMaxDay;
		 num_matched = 1;
		 return true;
		 
	case eAosOpr_ge:	
		 start_days[0] = value;
		 end_days[0] = AosTimeUtil::eMaxDay;
		 num_matched = 1;
		 return true;

	case eAosOpr_eq:			
		 start_days[0] = value;
		 end_days[0] = value;
		 num_matched = 1;
		 return true;

	case eAosOpr_lt:		
		 start_days[0] = AosTimeUtil::eMinDay;
		 end_days[0] = value-1;
		 num_matched = 1;
		 return true;

	case eAosOpr_le:	
		 start_days[0] = AosTimeUtil::eMinDay;
		 end_days[0] = value;
		 num_matched = 1;
		 return true;

	case eAosOpr_ne:			
		 // Not equal is handled in two parts:
		 // 	> start_day
		 // 	< start_day
		 start_days[0] = value+1;
		 end_days[0] = AosTimeUtil::eMaxDay;
		 
		 if (!AosTimeUtil::str2Time(rdata, mFormat2, mValue2, value2)) return false;
		 start_days[1] = AosTimeUtil::eMinDay;
		 end_days[1] = value2-1;
		 if (value > value2)
		 {
			 AosSetErrorUser(rdata, "termiiltype_value1_bigger_value2")
				 << "Value1: " << value << ", Value2: " << value2 << enderr;
			 return false;
		 }
		 num_matched = 2;
		 return true;

	case eAosOpr_Objid:
	case eAosOpr_like:
	case eAosOpr_date:
		 AosSetErrorUser(rdata, "termiiltype_opr_not_allowed") 
			 << "Operator: " << mOpr << ", IIL Name: " << mIILName << enderr;
		 return false;

	case eAosOpr_prefix:
		 // The time format is:
		 // 	yyyy-mm-dd hh:mm:ss
		 // Since it uses the epoch time portion, for values longer than 10
		 // ("yyyy-mm-dd"), it is the same as equal. 
		 if (mValue.length() > 10)
		 {
		 	 start_days[0] = value;
		 	 end_days[0] = value;
		 	 num_matched = 1;
		 	 return true;
		 }

		 if (mValue.length() == 7)
		 {
			 // It is "yyyy-mm". It covers the time:
			 // 	["yyyy-mm-01", "yyyy-mm-31"]
			 char *data = (char *)mValue.data();
			 char c = data[4];
			 data[4] = 0;
			 int year = atoi(data);
			 int month = atoi(&data[5]);
			 data[4] = c;
			 int day = AosTimeUtil::getNumDaysInMonth(year, month);
			 OmnString ss = mValue;
			 ss << "-01";
			 int vv;
			 if (!AosTimeUtil::str2EpochDay(rdata, ss, vv)) return false;
			 start_days[0] = vv;

			 char buff[100];
			 sprintf(buff, "%s-%02d", data, day);
			 ss = buff;
			 if (!AosTimeUtil::str2EpochDay(rdata, ss, vv)) return false;
			 end_days[0] = vv;
			 num_matched = 1;
			 return true;
		 }

		 if (mValue.length() == 4)
		 {
			 OmnString ss = mValue;
			 ss << "-01-01";
			 int vv;
			 if (!AosTimeUtil::str2EpochDay(rdata, ss, vv)) return false;
			 start_days[0] = vv;

			 ss = mValue;
			 ss << "-12-31";
			 if (!AosTimeUtil::str2EpochDay(rdata, ss, vv)) return false;
			 end_days[0] = vv;
			 num_matched = 1;
			 return true;
		 }

		 AosSetErrorUser(rdata, "termiiltype_internal_error") << enderr;
		 return false;

	case eAosOpr_an:			// Matching any
		 start_days[0] = AosTimeUtil::eMinDay;
		 end_days[0] = AosTimeUtil::eMaxDay;
		 num_matched = 1;
		 return true;

	case eAosOpr_range_ge_le:
		 if (!AosTimeUtil::str2Time(rdata, mFormat1, mValue, value)) return false;
		 if (!AosTimeUtil::str2Time(rdata, mFormat2, mValue2, value2)) return false;
		 start_days[0] = value;
		 end_days[0] = value2;
		 num_matched = 1;
		 return true;

	case eAosOpr_range_ge_lt:
		 if (!AosTimeUtil::str2Time(rdata, mFormat1, mValue, value)) return false;
		 if (!AosTimeUtil::str2Time(rdata, mFormat2, mValue2, value2)) return false;
		 start_days[0] = value;
		 end_days[0] = value2-1;
		 num_matched = 1;
		 return true;

	case eAosOpr_range_gt_le:
		 if (!AosTimeUtil::str2Time(rdata, mFormat1, mValue, value)) return false;
		 if (!AosTimeUtil::str2Time(rdata, mFormat2, mValue2, value2)) return false;
		 start_days[0] = value+1;
		 end_days[0] = value2;
		 num_matched = 1;
		 return true;

	case eAosOpr_range_gt_lt:
		 if (!AosTimeUtil::str2Time(rdata, mFormat1, mValue, value)) return false;
		 if (!AosTimeUtil::str2Time(rdata, mFormat2, mValue2, value2)) return false;
		 start_days[0] = value+1;
		 end_days[0] = value2-1;
		 num_matched = 1;
		 return true;

	default:
		 break;
	}

	AosSetErrorUser(rdata, "termiiltype_invalid_time_cond")
		<< AosOpr_toStr(mOpr) << enderr;
	return false;
}


/*
bool
AosTermIILType::initStatTimeCond(const AosRundataPtr &rdata, AosStatQueryCondInfo &cond)
{
	// Ketty 2014/04/14 temp.
	if(mFormat1 == "") mFormat1 = AOSTIMEFORMAT_YYYY_MM_DDHHMMSS;	
	
	i64 value=0;
	if (!AosTimeUtil::str2Time(rdata, mFormat1, mValue, value)) return false;

	switch (mOpr)
	{
	case eAosOpr_gt:		
		 cond.start_time = value + 1; 
		 return true;
		 
	case eAosOpr_ge:	
		 cond.start_time = value;
		 return true;

	case eAosOpr_eq:			
		 cond.start_time = value;
		 cond.end_time = value;
		 return true;

	case eAosOpr_lt:		
		 cond.end_time = value - 1;
		 return true;

	case eAosOpr_le:	
		 cond.end_time = value;
		 return true;

	case eAosOpr_ne:			
	default:
		 OmnNotImplementedYet;
		 return false;
	}
	return false;
}
*/
