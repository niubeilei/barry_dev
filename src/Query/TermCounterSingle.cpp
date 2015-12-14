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
// 06/30/2011 Created by Lynch yang 
////////////////////////////////////////////////////////////////////////////
#include "Query/TermCounterSingle.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterQueryType.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/ResolveCounterName.h"
#include "Porting/GetTime.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
#include "SEModules/LogMgr.h"
#include "UtilTime/TimeUtil.h"
#include "Util/TimeUtil.h" 
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosTermCounterSingle::AosTermCounterSingle(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_COUNTER_SINGLE, AosQueryType::eCounterSingle, regflag),
mStartTime(0),
mEndTime(0),
mTimeGran(AosTimeGran::eInvalid),
mTimeFormat(AosTime::getDftTimeFormat()),
mNumValues(0),
mDataLoaded(false),
mCrtTime(0),
mCrtIdx(-1),
mNumDocs(0),
mDftValue(0),
mFactor(0.0)
{
}


AosTermCounterSingle::AosTermCounterSingle(
					const AosXmlTagPtr &term, 
					const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_COUNTER_SINGLE, AosQueryType::eCounterSingle, false),
mStartTime(0),
mEndTime(0),
mTimeGran(AosTimeGran::eInvalid),
mTimeFormat(AosTime::getDftTimeFormat()),
mNumValues(0),
mDataLoaded(false),
mCrtTime(0),
mCrtIdx(-1),
mNumDocs(0),
mDftValue(0),
mFactor(0.0)
{
	mIsGood = parse(term, rdata);
}


AosTermCounterSingle::~AosTermCounterSingle()
{
}


void
AosTermCounterSingle::toString(OmnString &str)
{
	str << "<counterSingle " << AOSTAG_COUNTER_ID << "=\"" << mCounterId << "\" "
		<< "zky_cname " << "=\"" << mCounterName << "\" "
		<< "start_time" << "=\"" << mStartTime << "\" "
		<< "end_time" << "=\"" << mEndTime << "\" "
		<< "factor" << "=\"" << mFactor << "\" "
		<< "time_gran" << "=\"" << AosTimeGran::toChar(mTimeGran) << "\" />";
}


bool
AosTermCounterSingle::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// Check whether it has retrieved the counter values. If not, 
	// it retrieves the counters. 
	// After that, it checks it has returned the last counter. 
	// If yes, it is the end of the query. 
	// Otherwise, it returns the index.
	finished = false;
	if (!mDataLoaded) 
	{
		aos_assert_r(loadData(rdata), false);
		mCrtIdx = 0;
		docid = 1;
		if (mNumValues == 0) docid = 0;
		return true;
	}

	if (mNoMoreDocs)
	{
		docid = 0;
		mNoMoreDocs = true;
		finished = true;
		return true;
	}

	mCrtIdx++;
	mCrtTime = AosTime::nextTime(mCrtTime, mTimeGran, mTimeFormat, false);
	//if (mCrtIdx < 1)
	//{
	//	OmnAlarm << "Internal error" << enderr;
	//	mNoMoreDocs = true;
	//	finished = true;
	//	docid = 0;
	//	return false;
	//}

	if (mCrtIdx < 0 || mCrtIdx >= mNumValues)
	{
		mNoMoreDocs = true;
		finished = true;
		docid = 0;
		return true;
	}

	docid = mCrtIdx;
	finished = false;
	return true;
}


bool
AosTermCounterSingle::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;

	mDataLoaded = true;
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	// bool rslt = AosCounterClt::getSelf()->retrieveSingleCounter(
	bool rslt = AosRetrieveSingleCounter(
			mCounterId, mCounterName, mStatTypes, mStartTime, mEndTime, 
			mDftValue, mBuff, rdata);
	if (!rslt)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		mNoMoreDocs = true;
		return false;
	}

	// The results are stored in 'mBuff'.
	// There are data. The data are stored in the form:
	// 		dddd,
	// 		dddd,
	//
	// Example1: No time, only one stat type. 
	// 		There shall be only one value.
	//
	// Example2: It has time, only one stat type:
	// 		There shall be one value for each time in the time range: 
	// 				[mStartTime, mEndTime]
	//
	// Example3: It has time and has multiple stat types:
	// 		There shall be one value for each stat type and each time value:
	// 			value1, value2, ..., valuen		(for one time value)
	// 			value1, value2, ..., valuen		(for one time value)
	// 			...
	mBuff->reset();
	OmnString records, record; 
	if (!AosTime::isValidTimeRange(mStartTime, mEndTime))
	{
		// It is not time based. The 'mBuff' format should be:
		// 			stattype[1] :[value]
		// 			stattype[2] :[value]
		// 			stattype[3] :[value]
		// 			...
		// 			stattypen[value]
		// No time. 'mValues' is in the format:
		// mValues[stattype[1], value]
		// mValues[stattype[2], value]
		// ...
		// mValues[stattype[n], value]
		for (u64 i = 0; i< mStatTypes.size(); i++)
		{
			AosStatType::E statType = mStatTypes[i];
			i64 vv = mBuff->getI64(mDftValue);
			mValues[statType].push_back(vv);
		}
		mNumValues = 1;
		return true;
	}
	
	i64 end_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mEndTime, false);
	for (u32 i = 0; i< mStatTypes.size(); i++)
	{
		i64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, false);
		AosStatType::E stattype = mStatTypes[i];
		OmnString timestr = mBuff->getOmnStr("");
		i64 index = -1; 
		while (1)
		{
			// 'mBuff' is in the format:
			// stattype[1]:	[timestr, value]
			// 				[timestr, value]
			// 					...
			// 		
			// stattype[2]:	[timestr, value]
			// 				[timestr, value]
			// 					...
			// stattype[n]:	[timestr, value]
			// 				[timestr, value]
			// 					...
			
			// It has time .'mValues' is in the format:
			// mValues[stattype[1],(timestr + value)]
			// mValues[stattype[2],(timestr + value)]
			// ...
			// mValues[stattype[n],(timestr + value)]
			if (timestr == "") 
			{
				// There are no more data in 'mBuff'. 
				while (crt_time <= end_time) 
				{
					mValues[stattype].push_back(mDftValue);
					crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
				}
				break;
			}

			u64 unitime = timestr.parseU64(0);
			aos_assert_r(unitime, false);
			i64 tt = AosTime::convertUniTime(mTimeFormat, mTimeGran, unitime, false);

			i64 value = mBuff->getI64(mDftValue);

			// Contents in 'mBuff' are in the format:
			// 		[timestr, value]
			// 		[timestr, value]
			// 		...
			// It is possible that the entry [timestr, value] in 'mBuff' 
			// is not the current time 'crt_time'. The loop below will 
			// add a default value to mValues[stattype].
			while (crt_time < tt && crt_time <= end_time)
			{
				mValues[stattype].push_back(mDftValue);
				crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
			}
			aos_assert_rr(crt_time <= end_time, rdata, false);
			aos_assert_rr(crt_time == tt, rdata, false);
			if (index < 0)
			{
				mValues[stattype].push_back(value);
			}
			else
			{
				aos_assert_r(index >= 0 && index < (i64)mValues[stattype].size(), false);
				i64 vv = mValues[stattype][index];
				mValues[stattype][index] = vv + value;
			} 

			timestr = mBuff->getOmnStr("");
			if (timestr != "")
			{
				aos_assert_r(timestr.parseU64(0), false);
				if (tt == AosTime::convertUniTime(mTimeFormat, mTimeGran, timestr.parseU64(0), false)) 
				{
					index = mValues[stattype].size() -1;
					continue;
				}
			}
			index = -1;
			crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
		}
		mNumValues = mValues[stattype].size();
	}
	
	mCrtTime = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, false);
	return true;
}


bool 	
AosTermCounterSingle::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	setTotalInRslt(mNumValues);
	return true;
}

bool
AosTermCounterSingle::getDocidsFromIIL(
        const AosQueryRsltObjPtr &query_rslt,
        const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context, 
        const AosRundataPtr &rdata)
{
	//OmnShouldNeverComeHere;
    //if (!mDataLoaded) loadData(rdata);
	if (query_rslt)
	{
		query_rslt->setNumDocs(mNumValues);
		query_rslt->reset();
	}

	if(query_context)
	{
		query_context->setFinished(true);
	}

    return true;
}


i64		
AosTermCounterSingle::getTotal(const AosRundataPtr &rdata)
{
	if (!mDataLoaded)
	{
		aos_assert_r(loadData(rdata), false);
		mCrtIdx = -1;
	}
	return mNumValues;
}


bool	
AosTermCounterSingle::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


void	
AosTermCounterSingle::reset(const AosRundataPtr &rdata)
{
	mNoMoreDocs = false;
	//mAccumulates = 0;
	mCrtIdx = -1;
}


bool 	
AosTermCounterSingle::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosTermCounterSingle::setOrder(
		const OmnString &container,
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	return false;
}


AosXmlTagPtr
AosTermCounterSingle::getDoc(const u64 &value, const AosRundataPtr &rdata)
{
	// This term keeps the queried results in 'mValues[AosStatType::emax]'. 
	// Each entry in mValues[i] is a vector. If it is time based, it keeps
	// all the entries for the time. Otherwise, the vector should have only
	// one value.
	//
	// This function creates a record. If it is not time based, the record is:
	// 	<record AOSTAG_CNAME="xxx"
	// 			AOSTAG_STATNAME="xxx"
	// 			AOSTAG_STATNAME="xxx"
	// 			.../>
	//
	// If it is time based, it should be:
	// 	<records>
	// 		<record AOSTAG_CNAME="xxx"
	// 			AOSTAG_STATNAME="xxx"
	// 			AOSTAG_STATNAME="xxx"
	// 			.../>
	// 		...
	// 	</records>
	// so that every time when this function is called, it returns only one
	// record.

	if (!AosTime::isValidTimeRange(mStartTime, mEndTime))
	{
		// It is not time based. There shall be only one record. 
		AosXmlTagPtr record = createRecord(0, AosTime::eInvalidTime, rdata);
		mNoMoreDocs = true;
		return record;
	}

	AosXmlTagPtr record = createRecord(mCrtIdx, mCrtTime, rdata);
	//mCrtTime = AosTime::nextTime(mCrtTime, mTimeGran, mTimeFormat);
	return record;
}


AosXmlTagPtr
AosTermCounterSingle::createRecord(
		const i64 &index, 
		const u64 &time, 
		const AosRundataPtr &rdata)
{
	// This function creates a new record. If it is not time based, 
	// the record should be in the form:
	// 	<record AOSTAG_CNAME="xxx" AOSTAG_STATNAME="xxx" .../>
	//
	// If it is time based, it should be:
	// 	<records>
	// 		<record AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	// 		<record AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	// 		...
	// 	</records>
	OmnString cname;
	if (AosTime::isValidTime(time))
	{
		cname = AosTime::convertToStr(time, mTimeGran, mTimeFormat, false);
	}
	else
	{
		//cname = mCounterName;
		cname = AosCounterUtil::getCounterName(mCounterName);
		if (cname == "") cname = mCounterName;
	}

	OmnString record = "<record ";
	record << AOSTAG_CNAME << "=\"" << cname << "\" ";
	vector<OmnString> str;
	AosCounterUtil::splitCounterName(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}

	for (u32 i=0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		OmnString name = "zky_";
		name << AosStatType::toString(stattype);
		if (index < 0 || index >= (i64)mValues[stattype].size())
		{
			OmnAlarm << "Invalid index: " << index << enderr;
			record << name << "=\"" << mDftValue << "\" ";
		}
		else
		{
			i64 v1 = mValues[stattype][index];
			//calculateAverages(stattype, index, v1);
			//mAccumulates[stattype] += mValues[stattype][index];
			mAccumulates[stattype] += v1;
			i64 v2 = mAccumulates[stattype];

			if (mFactor != 0.0) 
			{
				double vv1 = v1 * mFactor;
				double vv2 = v2 * mFactor;
				record << name << "=\"" << vv1 << "\" ";
				name << "_accum";
				record << name << "=\"" << vv2 << "\" ";
			}
			else
			{
				record << name << "=\"" << v1 << "\" ";
				name << "_accum";
				record << name << "=\"" << v2 << "\" ";
			}
		}
	}

	if (mAverage)
	{
		//avg = sum/count;
		AosStatType::E stattype = AosStatType::eAverage;
		OmnString name = "zky_";
		name << AosStatType::toString(stattype);
		double vv1 = calculateAverages(index);
		record << name << "=\"" << vv1 << "\" ";

	}
	record << "/>";

	try
	{
		AosXmlParser parser;
		return parser.parse(record, "" AosMemoryCheckerArgs);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_create_record");
		OmnAlarm << rdata->getErrmsg() << enderr;
	   	return 0; 
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosTermCounterSingle::parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	//	The 'term' should be in the format:
	// <statterm type="counterSingle" 
	// 			sum="true|false"
	// 			aver="true|false"
	// 			factor="xxx"
	// 			max="true|false"
	// 			min="true|false"
	// 			zky_counter_id="xxx">
	// 		<counter_names>
	// 		     <termname zky_value_type="attr" zky_xpath="xxxx">
	//         	  	<zky_docselector zky_docselector_type="retrieveddoc"/>
	// 	   	     </termname>
	// 		     <termname type="const">xxx</termname>
	// 	     			....
	// 	   	   	 <termname type="const">xxx</termname>
	// 	 	</counter_names>
	// 		<zky_time_condition zky_timeformat="xxx" zky_starttime="xxx" "zky_endtime="xxx"
	// 		zky_timegran = "yer|mnt|day".../>
	// 	</statterm>
	//
	// 	If 'factor' is not specified, it defaults to 1. If it is not 0, the value
	// 	will be multiplied by 'factor'. For instance, if factor="0.001", it means
	// 	to multiply '0.001' to the values.
	aos_assert_r(term, 0);

	mIILName = "";
	mIsGood = true;

	mReverse = term->getAttrBool("reverse", false);
	mFactor = term->getAttrDouble("factor", 0.0);

	AosStatType::retrieveStatTypes(mStatTypes, term);
	mAverage = term->getAttrBool(AOSSTATTYPE_AVERAGE, false);

	for (u32 i = 0; i<AosStatType::eMaxEntry; i++) mAccumulates[i] = 0;
	// Get the counter id
	mCounterId = term->getAttrStr(AOSTAG_COUNTER_ID);
	if (mCounterId == "") mCounterId = term->getNodeText(AOSTAG_COUNTER_ID);
	if (mCounterId == "")
	{
		AosSetErrorU(rdata, "missing_counter_id") << ": " << term->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		mIsGood = false;
		return false;
	}
	mCounterId = AosIILName::composeCounterIILName(mCounterId);

	AosTimeUtil::parseTimeNew(term, AOSTAG_TIME_CONDITION, 
			mStartTime, mEndTime, mTimeGran, mTimeFormat, false, rdata);

	bool need_proc_member = false;
	if (!AosTime::isValidTimeRange(mStartTime, mEndTime)) need_proc_member = true; 

	// Retrieve the counter name
	bool rslt = AosResolveCounterName::resolveCounterName(
			term, AOSTAG_COUNTERNAMES, mCounterName, need_proc_member, rdata);
	//if (!AosTime::isValidTimeRange(mStartTime, mEndTime))
	//{
	//	//No time  xxx|$$|xxx|$$|xxx|$_|member
	//	OmnString member = term->getAttrStr(AOSTAG_MEMBER);
	//	if (member == "") 
	//	{
	//		mIsGood = false;
	//		return false;
	//	}
	//
	//	mCounterName = AosCounterUtil::composeCounterName(mCounterName, member);  
	//}

	if (!rslt || mCounterName == "")
	{
		mIsGood = false;
		return false;
	}
	return true;
}


AosQueryTermObjPtr
AosTermCounterSingle::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterSingle(def, rdata);
}


bool	
AosTermCounterSingle::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


double
AosTermCounterSingle::calculateAverages(const i64 &index)
{
	//For each end_day, we want to calculate the average stay time (i.e., the number of days) all patients checked out on that given day. 
	//	The counter is defined as:
	//		DT_CounterName
	//		DN_CounterName
	//		where 'D' for daily, 'T' for total, and 'N' for number of times.
	//	Total = Sum(DT_CounterName.Day1, DT_CounterName.Day2])
	//	Number = Sum([DN_CounterName.Day1, DN_CounterName.Day2])
	// average = Total/Number;
	
	AosStatType::E sumtype = AosStatType::eSum;
	AosStatType::E countype = AosStatType::eCount;
	if (mValues[sumtype].size() == 0 || mValues[countype].size() == 0)
	{
		OmnAlarm << "Invalid index: " << AosStatType::eSum << enderr;
		return 0;
	}
	aos_assert_r(index >= 0 && index < (i64)mValues[sumtype].size(), 0);
	aos_assert_r(index >= 0 && index < (i64)mValues[countype].size(), 0);
	i64 total = mValues[sumtype][index];
	i64 number = mValues[countype][index];
	if (total == 0 || number == 0) return 0;
	double value = (double)total /number;
	return value;
}
