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
// This is a Term Template. When creating a new term, one should copy
// this file and modify it. 
//
// Modification History:
// 02/27/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Query/TermCounterAll.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/CounterTimeInfo.h"
#include "CounterUtil/RecordFormat.h"
#include "CounterUtil/CounterParameter.h"
#include "SEUtil/IILName.h"
#include "Query/TermOr.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosTermCounterAll::AosTermCounterAll(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_COUNTER_ALL, AosQueryType::eCounterAll, regflag),
mDataLoaded(false),
mCrtIdx(-1),
mNumValues(0),
mTime(0),
mFactor(0.0),
mUseIILName(false),
mRecordFormat(0)
{
}


AosTermCounterAll::AosTermCounterAll(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_COUNTER_ALL, AosQueryType::eCounterAll, false),
mDataLoaded(false),
mCrtIdx(-1),
mNumValues(0),
mTime(0),
mFactor(0.0),
mUseIILName(false),
mRecordFormat(0)
{
	// This constructor creates a new instance of the term. 'def' is the
	// XML definition of the term and the 'def' should be in fellow form:
	// <stat>
	// <counter zky_ctobjid='giccreators_query_cond_countersimple_h' type= 'counterMultiSub2'>
	// <cond sum='true' reverse="false">
	// <zky_counter_id></zky_counter_id>
	// <use_epochtime></use_epochtime>
	// <use_iilname></use_iilname>
	// <append_bit></append_bit>
	// <zky_level></zky_level>
	// <zky_time_condition zky_timeformat='2fmt'>
	// <zky_starttime></zky_starttime>
	// <zky_endtime></zky_endtime>
	// <zky_timegran></zky_timegran>
	// </zky_time_condition>
	// <zky_cname_format>term0.term2.time.time3</zky_cname_format>
	// <counter_names>
	// <termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>
	// <![CDATA[cn,mg]]></termname>
	// <termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>
	// <![CDATA[js,hl]]></termname>
	// <termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>
	// <![CDATA[sz,cs]]></termname>
	// </counter_names>
	// </cond>
	// <middle_result>
	// <aggregate zky_aggregate_type='mergeEntries' zky_mergefield_index = '0'/>
	// <filling zky_type="time">
	// <zky_fill_field></zky_fill_field>
	// <zky_fill_value></zky_fill_value>
	// </filling>
	// <sort zky_type="value">
	// <zky_order_by_value>false</zky_order_by_value>
	// </sort>
	// </middle_result>
	// <result>
	// <format zky_type= "onedime">
	// <zky_x></zky_x>
	// <zky_ypsize></zky_ypsize>
	// </format>
	// <datatype zky_type="double|int64">
	// <factor></factor>
	// <need_convert_int64></need_convert_int64>
	// </datatype>
	// </result>
	// </counter>
	// </stat>

	mIsGood = parse(def, rdata);
}


AosTermCounterAll::~AosTermCounterAll()
{
}


bool 	
AosTermCounterAll::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid that is selected by this term.
	// What it does is to retrieve the data based on the condition, and then 
	// returns the next docid. Normally you do not need to override this function. 
	// Refer to AosQueryTerm::nextDocid(...) to see whether it is the one you
	// need. If yes, do not override it. Remove this function. 
	finished = false;
	if (mNoMoreDocs)
	{
		docid = 0;
		finished = true;
		return true;
	}

	if (!mDataLoaded)
	{
		aos_assert_r(loadData(rdata), false);
		mCrtIdx = 0;
		docid = 1;
		if (mNumValues == 0) docid = 0;
		return true;
	}

	mCrtIdx++;
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
AosTermCounterAll::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermCounterAll::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	if (query_rslt) 
	{
		query_rslt->appendDocid(mNumValues);
		query_rslt->reset();
	}
	if(query_context)
	{
		query_context->setFinished(true);
	}
	return true;
}


AosXmlTagPtr
AosTermCounterAll::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	// This function creates a record. If it is not time based, the record is:
	//   <record AOSTAG_CNAME="xxx"
	//           AOSTAG_STATNAME="xxx"
	//           AOSTAG_STATNAME="xxx"
	//          .../>
	// 
	//  If it is time based, it should be:
	//   <records>
	//       <record AOSTAG_CNAME="xxx"
	//           AOSTAG_STATNAME="xxx"
	//           AOSTAG_STATNAME="xxx"
	//           .../>
	//       ...
	//   </records>
	
	return createRecord(mCrtIdx, rdata);
}


AosXmlTagPtr
AosTermCounterAll::createRecord(i64 &index, const AosRundataPtr &rdata)
{
	aos_assert_r(index >= 0 && index < mNumValues, 0);
	aos_assert_r(mRecordFormat, 0);
	OmnString record = mRecordFormat->createRecord(index, rdata);
	aos_assert_r(record.length() < 20000000, 0);
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
	return 0;
}


bool 	
AosTermCounterAll::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function moves the position
	// to 'pos' in the array. If pos is bigger than the size of the array, 
	// it returns false. Otherwise, it loads the data as needed, and changes
	// the position to 'pos'. 
	//
	// This is the default implementation (check AosQueryTerm::moveTo(pos, rdata).
	// If this is what you need, do not override this function.
	aos_assert_r(pos >= 0 && pos <= mNumValues, false);
	if (pos == 0)
	{
		mCrtIdx = -1;
		return true;
	}
	mCrtIdx = pos -1;
	return true;
}


i64		
AosTermCounterAll::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	if (!mDataLoaded)
	{
		aos_assert_r(loadData(rdata), 0);
		mCrtIdx = -1;
	}
	return mNumValues;
}


void	
AosTermCounterAll::reset(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids, and a position pointer that
	// is used to indicate which element the term is currently at. 
	// This function resets the position pointer to the beginning of the array.
	// If this is what you need (refer to AosQueryTerm::reset()), do not override
	// this member function.
	mNoMoreDocs = false;
    mCrtIdx = -1;
}

void
AosTermCounterAll::toString(OmnString &str)
{
	// This function converts the term into an XML.
	OmnString counter_ids;
	for (u32 i = 0; i < mCounterIds.size(); i++)
	{
		if (counter_ids != "") counter_ids << ","; 
		counter_ids << mCounterIds[i];
	}

	OmnString cname;
	for (u32 i = 0; i < mKeys.size(); i++)
	{
		if (cname != "") cname << ",";
		cname << mKeys[i];
	}

	str << "<counterAll " << AOSTAG_COUNTER_ID << "=\"" << counter_ids << "\" "
		<< "zky_cname " << "=\"" << cname << "\" "
		<< "start_time" << "=\"" << mTime->getStartTime() << "\" "
		<< "end_time" << "=\"" << mTime->getEndTime() << "\" "
		<< "factor" << "=\"" << mFactor << "\" "
		<< "time_gran" << "=\"" << AosTimeGran::toChar(mTime->getTimeGran()) << "\" />";
}


bool
AosTermCounterAll::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;

	mDataLoaded = true;
	aos_assert_r(mKeys.size() > 0, false);
	aos_assert_r(mRecordFormat, false);

	mParm->cnames = mKeys;
	bool rslt = AosCounterClt::getSelf()->retrieveCounters2ByNormal(mParm, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (!rslt)
	{
		mNoMoreDocs = true;
		return true;
	}
	mNumValues = mRecordFormat->getNumValues(); 
	return true;
}



bool
AosTermCounterAll::parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	mIsGood = false;
	aos_assert_r(rdata, false);
	aos_assert_rr(term, rdata, false);

	mParm = OmnNew AosCounterParameter();

	bool rslt = parseResult(term, rdata);
	aos_assert_r(rslt, false);

	rslt = parseCond(term, rdata);
	aos_assert_r(rslt, false);

	rslt = parseMiddleResult(term, rdata);
	aos_assert_r(rslt, false);
	mIsGood = true;
	return true;
}

bool
AosTermCounterAll::parseCond(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	AosXmlTagPtr cond = term->getFirstChild("cond");
	if (!cond) return false;

	aos_assert_r(mRecordFormat, false);

	mReverse = cond->getAttrBool("reverse", false);
	// Retrieve StatTypes
	AosStatType::retrieveStatTypes(mStatTypes, cond);
	mRecordFormat->setStatTypes(mStatTypes);
	mParm->stat_types = mStatTypes;

	mUseIILName = cond->getNodeText("use_iilname") == "true"?true:false;
	mParm->use_iilname = mUseIILName;

	// Retrieve Counter ID
	bool rslt = retrieveCounterIds(cond);
	aos_assert_r(rslt, false);

	OmnString append_bit_flag = cond->getNodeText("append_bit");
	i64 append_bit = (append_bit_flag == "true"?1:0); 

	mParm->cname_format = cond->getNodeText("zky_cname_format");	

	OmnString levelstr = cond->getNodeText("zky_level");
	aos_assert_r(levelstr != "", false);
	i64 level = atoi(levelstr.data());

	// retrieve string header
	OmnString str_header = "";
	rslt = AosResolveCounterName::resolveCounterName2(
			cond, "counter_header", str_header, rdata);
	aos_assert_r(rslt, false);

	// Retrieve the counter name
	OmnString cname;
	rslt = AosResolveCounterName::resolveCounterName2(
	        cond, AOSTAG_COUNTERNAMES, cname, rdata);
	aos_assert_r(rslt, false);

	mTime = OmnNew AosCounterTimeInfo();
	rslt = mTime->parseTime(cond, AOSTAG_TIME_CONDITION, rdata);
	aos_assert_r(rslt, false);
	mRecordFormat->setTimeObj(mTime);

	// compose Entry Type
	u16 entry_type = AosCounterUtil::composeTimeEntryType(level, append_bit, AosStatType::eInvalid, mTime->getTimeGran());

	rslt = getCounterNames(cname, entry_type, str_header, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTermCounterAll::parseMiddleResult(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	AosXmlTagPtr xml = term->getFirstChild("middle_result");
	if (!xml) return true;

	//parse filling config 
	bool rslt = retrieveFillingConf(xml, rdata);
	aos_assert_r(rslt, false);

	//Aggregate Multiple 
	rslt = retrieveAggregateConf(xml, rdata);
	aos_assert_r(rslt, false);

	//sort
	rslt = retrieveSortConf(xml, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTermCounterAll::parseResult(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	AosXmlTagPtr xml = term->getFirstChild("result");
	if (!xml) return true;

	//Two-Dimensional Results
	bool rslt = retrieveRecordFormatConf(xml, rdata);
	aos_assert_r(rslt, false);

	rslt = retrieveDataTypeConf(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosTermCounterAll::retrieveDataTypeConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	AosXmlTagPtr xml = term->getFirstChild("datatype");
	if (!xml) return true;

	OmnString type = xml->getAttrStr("zky_type");
	if (!(type == "double" || type == "int64")) return true;

	OmnString factor = xml->getNodeText("factor");
	mFactor = (factor == ""?0:atof(factor.data()));

	bool need_convert_int64 = xml->getNodeText("need_convert_int64") == "true"?true:false;
	aos_assert_r(mRecordFormat, false);
	mRecordFormat->setFactor(mFactor);
	mParm->need_convert_int64 = need_convert_int64;
	return true;
}


bool
AosTermCounterAll::retrieveFillingConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	AosXmlTagPtr xml = term->getFirstChild("filling");
	if (!xml) return true;

	OmnString ff = xml->getNodeText("zky_fill_field");
	if (ff == "") return true;

	OmnString type = xml->getAttrStr("zky_type"); 
	aos_assert_r(type == "time", false);
	
	i64 filling_field = atoi(ff.data()); 
	OmnString vv = xml->getNodeText("zky_fill_value");
	i64 dft = (vv == ""?0:atoi(vv.data()));
	mRecordFormat->setFilling(type, filling_field, dft);
	return true;
}

bool
AosTermCounterAll::retrieveCounterIds(const AosXmlTagPtr &term)
{
	OmnString counterids = term->getNodeText(AOSTAG_COUNTER_ID);
	if (counterids == "") return false;

	OmnString word;
	OmnStrParser1 parser(counterids, ",");
	while ((word = parser.nextWord()) != "")
	{
		if (!mUseIILName)  word = AosIILName::composeCounterIILName(word);
		mCounterIds.push_back(word);
	}

	aos_assert_r(mCounterIds.size() > 0 && mCounterIds.size() <= eMaxNumCounterId, false);
	mRecordFormat->setCounterIds(mCounterIds);
	mParm->sort_counter_id = mCounterIds[0];
	mParm->counter_ids = mCounterIds;
	return true;
}


bool
AosTermCounterAll::retrieveAggregateConf(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr tag = term->getFirstChild("aggregate");
	if (!tag) return true;

	OmnString aggregateType = tag->getAttrStr("zky_aggregate_type", "");
	if (aggregateType  == "") return true;

	AosCounterQueryPtr counterQuery = OmnNew AosCounterQuery();
	counterQuery->setAggregateType(aggregateType);

	i64 fieldindex = tag->getAttrInt64("zky_mergefield_index", -1);
	counterQuery->setMergeFieldIndex(fieldindex);  
	
	counterQuery->setConvertEntriesConfig(tag);

	mParm->counter_query = counterQuery;
	return true;
}


bool
AosTermCounterAll::retrieveSortConf(
		const AosXmlTagPtr &term,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr xml = term->getFirstChild("sort");
	aos_assert_r(xml, false);
	
	if (xml->getAttrStr("zky_type") != "value") return true;
	bool orderByValue = xml->getNodeText(AOSTAG_ORDER_BY_VALUE) == "true"?true:false;
	aos_assert_r(mRecordFormat, false);
	mParm->order_by_value = orderByValue;
	mParm->reverse = mReverse;
	return true;
}



bool
AosTermCounterAll::retrieveRecordFormatConf(
		const AosXmlTagPtr &term,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr xml = term->getFirstChild("format");
	aos_assert_r(xml, false);
	
	mRecordFormat = AosRecordFormat::createFormat(xml, rdata);
	aos_assert_r(mRecordFormat, false);
	
	mParm->setRecordFormat(mRecordFormat);
	return true;
}


bool
AosTermCounterAll::getCounterNames(
		const OmnString &cname,
		const u16 &entry_type,
		const OmnString &str_header, 
		const AosRundataPtr &rdata)
{
	if (!mTime->isValidTimeRange())
	{
		mKeys.push_back(AosCounterUtil::composeTerm2(entry_type, str_header, cname, 0));
		return true;
	}

	u64 crt_time = mTime->convertUniTime(mTime->getStartTime()); 
	u64 end_time = mTime->convertUniTime(mTime->getEndTime());

	mKeys.push_back(AosCounterUtil::composeTerm2(entry_type, str_header, cname, crt_time));
	mKeys.push_back(AosCounterUtil::composeTerm2(entry_type, str_header, cname, end_time));
OmnScreen << "key1: " << mKeys[0] << " , key2:" << mKeys[1] << endl;
	return true;
}


AosQueryTermObjPtr
AosTermCounterAll::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterAll(def, rdata);
}

