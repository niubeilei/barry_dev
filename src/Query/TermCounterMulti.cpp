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
#include "Query/TermCounterMulti.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterQueryType.h"
#include "CounterUtil/CounterUtil.h"
#include "Query/TermAnd.h"
#include "QueryClient/QueryTrans.h"
#include "QueryClient/QueryClient.h"
#include "SEUtil/Ptrs.h"
#include "SearchEngine/DocServer.h"
#include "UtilTime/TimeInfo.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosTermCounterMulti::AosTermCounterMulti(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_COUNTER_MULTI, AosQueryType::eCounterMulti, regflag),
mStartTime(0),
mEndTime(0),
mNumValues(0),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0)
{
}


AosTermCounterMulti::AosTermCounterMulti(
					const AosXmlTagPtr &term, 
					const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_COUNTER_MULTI, AosQueryType::eCounterMulti, false),
mStartTime(0),
mEndTime(0),
mNumValues(0),
mRdata(rdata),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0)
{
	//AosXmlTagPtr term = def->getFirstChild("term");
	// 'term' should be in the form:
	// <statterm type="counterMulti">
	// 	<zky_ctype>xxxx</zky_ctype>
	// 	<zky_starttime>xxxx</zky_starttime>
	// 	<zky_endtime>xxxx</zky_endtime>
	//	<zky_cnames>
	//		<zky_cname>
	//			<termname zky_value_type="attr" zky_xpath="xxxx">
	//				<zky_docselector zky_docselector_type="retrieveddoc"/>
	//			</termname>
	//			<termname type="const">xxx</termname>
	//			....
	//			<termname type="const">xxx</termname>
	//		</zky_cname>
	//		....
	//	</zky_cnames>
	//</statterm>
	
	bool  rslt = parse(term, rdata);
	if (!rslt)
	{
		OmnString errmsg = "Failed to Consturct!";
		OmnAlarm << errmsg  << enderr;

	}
}


AosTermCounterMulti::AosTermCounterMulti(
					const AosXmlTagPtr &term, 
					const AosQueryTermObjPtr &orterm,
					const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_COUNTER_MULTI, AosQueryType::eCounterMulti, false),
mStartTime(0),
mEndTime(0),
mNumValues(0),
mRdata(rdata),
mOrTerm(orterm),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0)
{

	bool rslt = parse(term, mRdata);
    if (!rslt)
    {
        rdata->setError() << "Failed to Consturct!";
        OmnAlarm << rdata->getErrmsg()  << enderr;
    }
}


AosTermCounterMulti::~AosTermCounterMulti()
{
}


void
AosTermCounterMulti::toString(OmnString &str)
{
	OmnString counternamestr;
	for (u32 i = 0; i< mCnames.size(); i++)
	{
		if (counternamestr != "") counternamestr << ",";
		counternamestr << mCnames[i]; 
	}
	str << "<counterMulti " << AOSTAG_COUNTER_ID << "=\"" << mCounterId << "\" "
		<< "zky_cname " << "=\"" << counternamestr << "\" "
		<< "start_time" << "=\"" << mStartTime << "\" "
		<< "end_time" << "=\"" << mEndTime << "\" "
		<< "factor" << "=\"" << mFactor << "\" "
		<< "time_gran" << "=\"" << AosTimeGran::toChar(mTimeGran) << "\" />";
}


bool
AosTermCounterMulti::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	finished = false;

	//if (!mDataLoaded)
	//{
	//	loadData(rdata);
	//	mCrtIdx = 0;
	//	docid = 1;
	//	return true;
	//}

	if (mNoMoreDocs)
	{
		docid = 0;
		mNoMoreDocs = true;
		finished = true;
		return true;
	}

	// In the current implementations, if it is not time based, the data
	// is loaded in one call. Otherwise, the data is loaded for each 
	// counter.
	if (AosTime::isValidTimeRange(mStartTime, mEndTime))
	{
		if (!mDataLoaded) 
		{
			aos_assert_r(loadTimedData(rdata), false);
			mCrtIdx = 0;
			docid = 1;
			return true;
		}
	}
	else
	{
		if (!mDataLoaded) 
		{
			aos_assert_r(loadData(rdata), false);
			mCrtIdx = 0;
			docid = 1;
			return true;
		}
	}

	//mCrtIdx++;
	mCrtIdx = mCrtIdx + mStatTypes.size();
	if (mCrtIdx < 1)                           
	{                                          
		OmnAlarm << "Internal error" << enderr;
		mNoMoreDocs = true;                    
		finished = true;                       
		docid = 0;                             
		return false;                          
	}                                          

	if (mCrtIdx < 0 || mCrtIdx >= (i64)mCnames.size())
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


i64		
AosTermCounterMulti::getTotal(const AosRundataPtr &rdata)
{
	//if (!mDataLoaded) loadData(rdata);
	if (!mDataLoaded)
	{
		if (AosTime::isValidTimeRange(mStartTime, mEndTime))
		{
			aos_assert_r(loadTimedData(rdata), false);
		}
		else
		{                                                 
			aos_assert_r(loadData(rdata), false); 
		}                                        
		mCrtIdx = -1; 
	}
	return  mNumValues;
}


bool	
AosTermCounterMulti::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


void	
AosTermCounterMulti::reset(const AosRundataPtr &rdata)
{
	mNoMoreDocs = false;
	mCrtIdx = -1;
}


bool 	
AosTermCounterMulti::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosTermCounterMulti::setOrder(
		const OmnString &container, 
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	return false;
}


AosXmlTagPtr
AosTermCounterMulti::getDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	if (!AosTime::isValidTimeRange(mStartTime, mEndTime))
	{
		// It is not time based. There shall be only one record. 
		return createRecord(mCrtIdx, rdata);
	}

	return createTimedRecord(mCrtIdx, rdata);
}


bool
AosTermCounterMulti::parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	//	The 'term' should be in the format:
	// <statterm type="counterSingle" 
	// 			reverse="false" 
	// 			sum="true|false"
	// 			aver="true|false"
	// 			max="true|false"
	// 			min="true|false"
	// 			factor = "xxx"
	// 			counter_id="xxx">
	// 		<counter_names>
	// 			     <termname zky_value_type="attr" zky_xpath="xxxx">
	//     	    	  	<zky_docselector zky_docselector_type="retrieveddoc"/>
	// 	   		     </termname>
	// 			     <termname type="const">xxx</termname>
	// 	      			....
	// 	   		   	 <termname type="const">xxx</termname>
	// 	 	 </counter_names>
	// 		<AOSTAG_TIME_CONDITION .../>
	// 	</statterm>
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

	mCNameTmp = term->getFirstChild(AOSTAG_COUNTERNAMES);
	aos_assert_r(mCNameTmp, false);

	AosTimeUtil::parseTimeNew(term, AOSTAG_TIME_CONDITION, 
			mStartTime, mEndTime, mTimeGran, mTimeFormat, false, rdata);
	return true;
}


bool
AosTermCounterMulti::loadData(const AosRundataPtr &rdata)
{
	aos_assert_rr(!mDataLoaded, rdata, false);
	aos_assert_rr(!mBuff, rdata, false);
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
aos_assert_r(!AosTime::isValidTimeRange(mStartTime, mEndTime), 0);

	bool rslt = getCounterNames(rdata);
	aos_assert_r(rslt, false);
	if (mCnames.size() == 0)
	{
		AosSetErrorU(rdata, "Counter name is empty");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mDataLoaded = true;
	if (!AosTime::isValidTimeRange(mStartTime, mEndTime))
	{
		// bool rslt = AosCounterClt::getSelf()->retrieveMultiCounters(
		bool rslt = AosRetrieveMultiCounters(
			mCounterId, mCnames, mDftValue, mBuff, rdata);
		if (!rslt)
		{
			OmnAlarm << rdata->getErrmsg() << enderr;
			mNoMoreDocs = true;
			return false;
		}
		mBuff->reset();
		//mNumValues = mCnames.size()/mStatTypes.size(); 
		return true;
	}
	
	// It is time based.
	OmnNotImplementedYet;
	return false;
}


bool
AosTermCounterMulti::loadTimedData(const AosRundataPtr &rdata)
{
	// In the current implementations, timed data are loaded every time when 
	// 'nextDocid(...)' is called. 
	if (mDataLoaded) return  true;
	mDataLoaded = true;
aos_assert_r(AosTime::isValidTimeRange(mStartTime, mEndTime), 0);
	if (!mBuff) mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	aos_assert_rr(AosTime::isValidTimeRange(mStartTime, mEndTime), rdata, false);

	bool rslt = getCounterNames(rdata);
	aos_assert_r(rslt, false);
	if (mCnames.size() == 0)
	{
		AosSetErrorU(rdata, "Counter name is empty");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// rslt = AosCounterClt::getSelf()->retrieveMultiCounters(
	rslt = AosRetrieveMultiCounters(
			mCounterId, mCnames, mStartTime, mEndTime, mDftValue, mBuff, rdata);
	if (!rslt)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		mNoMoreDocs = true;
		return false;
	}
	mBuff->reset();
	return true;
}


AosXmlTagPtr
AosTermCounterMulti::createRecord(const i64 &index, const AosRundataPtr &rdata)
{
	// This function creates a the non-timed record. The record format:
	// 	<record AOSTAG_CNAME="xxx" AOSTAG_STATNAME="xxx" .../>
	//
	// If it is no time based, it returns the following record:
	// 	<record AOSTAG_CNAME="xxx" AOSTAG_STATNAME="xxx" ..../>
	// 	<record AOSTAG_CNAME="xxx" AOSTAG_STATNAME="xxx" ..../>
	// 	<record AOSTAG_CNAME="xxx" AOSTAG_STATNAME="xxx" ..../>
	
	aos_assert_r(!AosTime::isValidTimeRange(mStartTime, mEndTime), 0);
	aos_assert_rr(index >= 0 && index < (i64)mCnames.size(), rdata, 0);
	OmnString cname = AosCounterUtil::getCounterName(mCnames[index]);
	aos_assert_rr(cname != "", rdata, 0);
	//OmnString name = AosCounterUtil::getContainerPart(cname);
	//if (name == "") name = cname;
	OmnString record = "<record ";
	record << AOSTAG_CNAME << "=\"" << cname << "\" ";

	vector<OmnString> str;
	AosCounterUtil::splitCounterName(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}
	// The data is in 'mBuff', which is in the form:
	// 		stattype[0]value
	// 		stattype[1]value
	// 		stattype[2]value
	// 		...
	// 		stattype[0]value
	// 		stattype[1]value
	// 		stattype[2]value
	// 		...
	//
	
	// 'values' is in the format: 
	//	stattype[0] : value
	//	stattype[1] : value
	//	stattype[2] : value
	i64 values[AosStatType::eMaxEntry];
	for (u32 i=0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		u64 value = mBuff->getU64(mDftValue);
		i64 vv = mDftValue;
		if ((i64)value != mDftValue) vv = AosCounterUtil::convertValueToInt64(value);
		values[stattype] = vv;
	}
	
	for (u32 i=0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		OmnString name = "zky_";
		name << AosStatType::toString(stattype);

		//u64 value = mBuff->getU64(mDftValue);
		//i64 v1 = mDftValue;
		//if ((i64)value != mDftValue) v1 = AosCounterUtil::convertValueToInt64(value);
		i64 v1 = values[stattype];
		//calculateAverages(values, stattype, v1);

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

	if (mAverage)
	{
		//avg = sum/count;
		AosStatType::E stattype = AosStatType::eAverage;
		OmnString name = "zky_";
		name << AosStatType::toString(stattype);
		double vv1 = calculateAverages(values);
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


AosXmlTagPtr
AosTermCounterMulti::createTimedRecord(const i64 &index, const AosRundataPtr &rdata)
{
	// The record format is:
	// 	<record AOSTAG_CNAME="xxx" ...>
	// 		<time AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	// 		<time AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	// 		...
	// 	</record>
	aos_assert_r(AosTime::isValidTimeRange(mStartTime, mEndTime), 0);
	aos_assert_rr(mBuff, rdata, 0);
	aos_assert_rr(index >= 0 && index < (i64)mCnames.size(), rdata, 0);
	OmnString cname = mCnames[index];
	aos_assert_rr(cname != "", rdata, 0);

	// The data is in 'mBuff', which is in the following format:
	// 		[timestr, value],
	// 		[timestr, value], 
	// 		...
	// 		"",					// This is for the first stattype
	// 		[timestr, value],
	// 		[timestr, value], 
	// 		...
	// 		"",					// This is for the second stattype
	// 		...
	u64 crt_time = 0;
	u64 end_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mEndTime, false);
	vector<i64> values[AosStatType::eMaxEntry];
	for (u32 i = 0; i< mStatTypes.size(); i++)
	{
		crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, false);
		AosStatType::E stattype = mStatTypes[i];
		OmnString timestr = mBuff->getOmnStr("");
		i64 index = -1;
		while (1)
		{
			// 'mBuff' is in the format:
			// 		[timestr, value]
			// 		[timestr, value]
			// 		...
			// 		""
			if (timestr == "") 
			{
				// There are no more data in 'mBuff'. Need to generate a record
				// for each remaining time slot.
				while (crt_time <= end_time) 
				{
					values[stattype].push_back(mDftValue);
					crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
				}
				break;
			}

			u64 unitime = timestr.parseU64(0);
			aos_assert_r(unitime, 0);
			u64 tt = AosTime::convertUniTime(mTimeFormat, mTimeGran, unitime, false);

			i64 value = mBuff->getI64(mDftValue);
			// It is possible that the entry [timestr, value] in 'mBuff' 
			// is not the current time 'crt_time'. The loop below will 
			// add a default value to mValues[stattype].
			while (crt_time < tt && crt_time <= end_time)
			{
				values[stattype].push_back(mDftValue);
				crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
			}
			aos_assert_rr(crt_time <= end_time, rdata, 0);
			aos_assert_rr(crt_time == tt, rdata, 0);
			if (index < 0)
			{
				values[stattype].push_back(value);
			}
			else
			{
				aos_assert_r(index >= 0 && index < (i64)values[stattype].size(), 0);
				i64 vv = values[stattype][index];
				values[stattype][index] = vv + value;
			}

			timestr = mBuff->getOmnStr("");
			if (timestr != "")
			{
				unitime = timestr.parseU64(0);
				aos_assert_r(unitime, 0);
				if ((i64)tt == AosTime::convertUniTime(mTimeFormat, mTimeGran, unitime, false)) 
				{
					index = values[stattype].size() -1;
					continue;
				}
			}
			index = -1;
			crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
		}
	}

	// Time to create the records now.
	OmnString records;
	u32 idx = 0;
	crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, false);
	while (crt_time <= end_time)
	{
		cname = AosTime::convertToStr(crt_time, mTimeGran, mTimeFormat, false);
		records << "<time ";
		records << AOSTAG_CNAME << "=\"" << cname << "\" ";

		for (u32 i=0; i<mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			OmnString name = "zky_";
			name << AosStatType::toString(stattype);
			if (idx >= values[stattype].size())
			{
				OmnAlarm << "Invalid idx: " << idx << enderr;
				records << name << "=\"" << mDftValue << "\" ";
			}
			else
			{
				i64 v1 = values[stattype][idx]; 
				//mAccumulates[stattype] += values[stattype][idx];
				//calculateAverages(values, stattype, idx, v1);
				mAccumulates[stattype] += v1;
				i64 v2 = mAccumulates[stattype];

				if (mFactor != 0.0)
				{
					double vv1 = v1 * mFactor;
					double vv2 = v2 * mFactor;
					records << name << "=\"" << vv1 << "\" ";
					name << "_accum";
					records << name << "=\"" << vv2 << "\" ";
				}
				else
				{
					records << name << "=\"" << v1 << "\" ";
					name << "_accum";
					records << name << "=\"" << v2 << "\" ";
				}
			}
		}
		if (mAverage)
		{
			//avg = sum/count;
			AosStatType::E stattype = AosStatType::eAverage;
			OmnString name = "zky_";
			name << AosStatType::toString(stattype);
			double vv1 = calculateAverages(values, idx);
			records << name << "=\"" << vv1 << "\" ";
		}
		records << "/>";
		crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
		idx++;
	}

	//cname = AosCounterUtil::getCounterName(mCnames[index]);
	cname = AosCounterUtil::getCounterNamePart(mCnames[index]);
	OmnString contents = "<record ";
	contents << AOSTAG_USING_XML_AS_RESULT << "=\"" << "true" << "\" ";
	contents << AOSTAG_CNAME << "=\"" << cname << "\" ";

	vector<OmnString> str;
	AosCounterUtil::splitCounterName(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		contents << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}

	for (u32 i=0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		i64 v1 = mAccumulates[stattype];
		OmnString name = "zky_";
		name << AosStatType::toString(stattype) << "_accum";
		if (mFactor != 0.0)
		{
			double vv1 = v1 * mFactor;
			if (stattype == AosStatType::eAverage && mAccumulates[AosStatType::eSum] && v1)
			{
				vv1 = mAccumulates[AosStatType::eSum]/v1 * mFactor;
			}
			contents << name << "=\"" << vv1 << "\" ";
		}
		else
		{
			contents << name << "=\"" << v1 << "\" ";
		}
		mAccumulates[stattype] = 0;
	}
	contents << ">" << records << "</record>";

	try
	{
		AosXmlParser parser;
		return parser.parse(contents, "" AosMemoryCheckerArgs);
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

/*
OmnString
AosTermCounterMulti::getCounterCname(const AosXmlTagPtr &cnametmp)
{
	OmnString realCname, recordsStr;
	bool rslt = AosResolveCounterName::resolveCounterName(cnametmp, realCname, mRdata);
	aos_assert_r(rslt, "");
	if (realCname != "")
	{
		recordsStr << "<record counter_name=\"" << realCname << "\"/>";
	}
	return recordsStr;
}
*/


AosQueryTermObjPtr
AosTermCounterMulti::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterMulti(def, rdata);
}


bool	
AosTermCounterMulti::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTermCounterMulti::getCounterNames(const AosRundataPtr &rdata)
{
	mCnames.clear();
	aos_assert_r(mOrTerm, false);
	aos_assert_r(mCNameTmp, false);
	AosQueryRsltObjPtr queryData = mOrTerm->getQueryData();
	aos_assert_r(queryData, false);

	// The caller has already called queryData->nextDocid() once, so we need
	// to go back one step. 
	//queryData->backOffStep(1);
	mNumValues = queryData->getNumDocs();

	u64 docid = 0;
	bool rslt = queryData->nextDocid(docid, mNoMoreDocs, rdata);
	aos_assert_r(rslt, false);
	i64 num_records = 0;
	i64 numCnames = mCNameTmp->getNumSubtags();
	aos_assert_r(numCnames > 0, false);
	while (docid && num_records < mNumValues)
	{
		AosXmlTagPtr docXml = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (!docXml)
        {
            OmnAlarm << "Failed to retrieve the doc: " << docid << enderr;
			queryData->nextDocid(docid, mNoMoreDocs, rdata);
            continue;
        }

		rdata->setRetrievedDoc(docXml, true);
	
		// mCnames', which is in the form:
		//	mStatTypes.size() == 2 (sum, arg)
		//	statTypes[0]+cname
		//	statTypes[1]+cname
		//	statTypes[0]+cname
		//	statTypes[1]+cname
		//	...
		//
		bool need_proc_member = false;
		if (!AosTime::isValidTimeRange(mStartTime, mEndTime)) need_proc_member = true;

		OmnString realCname;
		rslt = AosResolveCounterName::resolveCounterName(mCNameTmp, realCname, need_proc_member, rdata);
		aos_assert_rr(realCname != "", rdata, false);

		for (u32 i=0; i<mStatTypes.size(); i++)
		{
			OmnString cname;
			if (AosTime::isValidTimeRange(mStartTime, mEndTime))
			{
				cname = AosCounterUtil::composeTimeCounterKey(realCname, mStatTypes[i]);
			}
			else
			{
				cname = AosCounterUtil::composeCounterKey(realCname, mStatTypes[i]);
			}
			mCnames.push_back(cname);
		}
		queryData->nextDocid(docid, mNoMoreDocs, rdata);
	}

	mNoMoreDocs = false;
	//sort(mCnames.begin(), mCnames.end());
//	vector<OmnString>::iterator itr = mCnames.begin();
//	for (; itr != mCnames.end(); itr++)
//	{
//	    cout << "Array: " << (*itr).data() << endl;
//	}
	return true;
}


double
AosTermCounterMulti::calculateAverages(const i64 values[AosStatType::eMaxEntry])
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
	aos_assert_r(values, 0);
	i64 total = values[sumtype];
	i64 number = values[countype];
	if (total <= 0 || number <= 0) return 0;
	double vv = (double)total /number;
	return vv;
}


double
AosTermCounterMulti::calculateAverages(
			const vector<i64> *values, 
			const i64 &index)
{
	aos_assert_r(values, 0);
	AosStatType::E sumtype = AosStatType::eSum;
	AosStatType::E countype = AosStatType::eCount;
	if (values[sumtype].size() == 0|| values[countype].size() == 0)
	{
		OmnAlarm << "Invalid index: " << AosStatType::eSum << " : " << AosStatType::eCount<< enderr;
		return 0;
	}

	aos_assert_r(index >= 0 && index < (i64)values[sumtype].size(), 0);
	aos_assert_r(index >= 0 && index < (i64)values[countype].size(), 0);
	i64 total = values[sumtype][index];
	i64 number = values[countype][index];
	if (total == 0 || number == 0) return 0;
	double vv = (double)total /number;
	return vv;
}

