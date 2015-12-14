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
// 11/28/2012	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "Query/TermCounterCompare.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterUtil/Ptrs.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterClt/CounterClt.h"
#include "Query/TermOr.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosTermCounterCompare::AosTermCounterCompare(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_COUNTER_COMPARE, AosQueryType::eCounterCompare, regflag),
mCounterId(""),
mNumValues(0),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mUseIILName(false),
mUseEpochTime(false),
mQueryRsltOpr("norm")
{
}


AosTermCounterCompare::AosTermCounterCompare(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_COUNTER_COMPARE, AosQueryType::eCounterCompare, false),
mCounterId(""),
mNumValues(0),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mUseIILName(false),
mUseEpochTime(false),
mQueryRsltOpr("norm")
{
	// This constructor creates a new instance of the term. 'def' is the
	// XML definition of the term and the 'def' should be in fellow form:
	// <statterm type="AOSTERMTYPE_COUNTERSUB" order="true|false" reverse="true|false">
	//      <zky_ctnr>lynch_ctnr</zky_ctnr>
	//      <zky_cnames>
	//     		<zky_cname>
	//	           <termname zky_value_type="attr" zky_xpath="xxxx">
	//	 	            <zky_docselector zky_docselector_type="retrieveddoc"/>
	//	           </termname>
	//	           <termname type="const">xxx</termname>
	//	                ....
	//	           <termname type="const">xxx</termname>
	//      	</zky_cname>
	//      </zky_cnames>
    //      <zky_orderBy>name|value</zky_orderBy>
	//      <zky_ctype>MMYYYY</zky_ctype>
	//      <zky_starttime>2011-01-20 00:00:00</zky_starttime>
	//      <zky_endtime>2011-08-20 00:00:00</zky_endtime>
	//  </statterm>
	
	mIsGood = parse(def, rdata);
}


AosTermCounterCompare::~AosTermCounterCompare()
{
}


bool 	
AosTermCounterCompare::nextDocid(
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
		if(!loadData(rdata)) mNumValues=0;
		finished = true;
		docid = 0;
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
AosTermCounterCompare::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermCounterCompare::getDocidsFromIIL(
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
AosTermCounterCompare::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	// This function creates a record. If it is not time based, the record is:
	// <records>
	// 		<xml>
	// 			<result1 zky_sum="70"/>
	// 			<result2 zky_sum="69"/>
	// 			<cmp zky_sum="-1"/>
	// 		</xml>
	// 		<xml>
	// 			<result1 zky_sum="11"/>
	// 			<result2 zky_sum="4"/>
	// 			<cmp zky_sum="-7"/>
	// 		</xml>
	// 		<xml>
	// 			<result1 zky_sum="98"/>
	// 			<result2 zky_sum="29"/>
	// 			<cmp zky_sum="-69"/>
	// 		</xml>
	// 		<xml>
	// 			<result1 zky_sum="55"/>
	// 			<result2 zky_sum="59"/>
	// 			<cmp zky_sum="4"/>
	// 		</xml>
	// </records>
	 
	return  createRecord(mCrtIdx, rdata);
}

AosXmlTagPtr
AosTermCounterCompare::createRecord(i64 &index, const AosRundataPtr &rdata)
{
	if (mOrderByValue)
	{
		char *buff;
		aos_assert_r(mArray->nextValue(&buff), NULL);
		OmnString kk(buff);
		i64 start_pos = (eMaxCnameLen -1) + 1 * sizeof(i64); 
		aos_assert_r(start_pos < mBuffLen, NULL);
		index = *(i64 *)&buff[start_pos];

		aos_assert_r(index >= 0 && index < (i64)mComparedKeys.size(), 0);
		aos_assert_r(mComparedKeys[index] == kk, 0);
	}

	OmnString record = "<xml>";

	for (u32 i=0; i<mCounters.size(); i++)
	{
		aos_assert_r(index >= 0 && index < (i64)mCounters[i].mKeys.size(), 0);
		if(i == 0)
		{
			record << "<result1 ";
			OmnString cname = AosCounterUtil::getCounterName(mCounters[0].mKeys[index]);
			vector<OmnString> str;
			AosCounterUtil::splitTerm2(cname, str);
			for (u32 i = 0; i< str.size(); i++)
			{
				record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
			}
			for (u32 i = 0; i< mCounters[0].mStatTypes.size(); i++)
			{
				AosStatType::E stattype = mCounters[0].mStatTypes[i];
				OmnString name = "zky_";
				name << AosStatType::toString(stattype);

				aos_assert_r(index < (i64)mCounters[0].mValues[stattype].size(), 0);
				i64 v1 = mCounters[0].mValues[stattype][index];

				record << name << "=\"" << v1 << "\" ";
			}
			record << " />";
		}
		else
		{
			record << "<result2 ";
			OmnString cname = AosCounterUtil::getCounterName(mCounters[1].mKeys[index]);
			vector<OmnString> str;
			AosCounterUtil::splitTerm2(cname, str);
			for (u32 i = 0; i< str.size(); i++)
			{
				record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
			}
			for (u32 i = 0; i< mCounters[1].mStatTypes.size(); i++)
			{
				AosStatType::E stattype = mCounters[1].mStatTypes[i];
				OmnString name = "zky_";
				name << AosStatType::toString(stattype);

				aos_assert_r(index < (i64)mCounters[1].mValues[stattype].size(), 0);
				i64 v1 = mCounters[1].mValues[stattype][index];

				record << name << "=\"" << v1 << "\" ";
			}
			record << " />";
		}
	}

	record << "<cmp ";
	for (u32 i = 0; i< mComparedStatTypes.size(); i++)
	{
		AosStatType::E stattype = mComparedStatTypes[i];
		OmnString name = "zky_";
		name << AosStatType::toString(stattype);

		aos_assert_r(index < (i64)mComparedValues[stattype].size(), 0);
		i64 v1 = mComparedValues[stattype][index];

		record << name << "=\"" << v1 << "\" ";
	}
	record << " />";

	record << "</xml>";

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
	
	return NULL;
}

bool 	
AosTermCounterCompare::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function moves the position
	// to 'pos' in the array. If pos is bigger than the size of the array, 
	// it returns false. Otherwise, it loads the data as needed, and changes
	// the position to 'pos'. 
	//
	// This is the default implementation (check AosQueryTerm::moveTo(pos, rdata).
	// If this is what you need, do not override this function.
	OmnNotImplementedYet;
	return false;
}


i64		
AosTermCounterCompare::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	if (!mDataLoaded)
	{
		if(!loadData(rdata)) mNumValues=0;
	}
	return mNumValues;
}


void	
AosTermCounterCompare::reset(const AosRundataPtr &rdata)
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
AosTermCounterCompare::toString(OmnString &str)
{
	// This function converts the term into an XML.
}


bool
AosTermCounterCompare::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;
	mDataLoaded = true;

	for (u32 i=0; i<mCounters.size(); i++)
	{
		mCounters[i].mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
//		bool rslt = AosCounterClt::getSelf()->retrieveCounters2(mCounterId, 
//				mCounters[i].mRangeKeys, mCounters[i].mStatTypes, mCounters[i].mEntryType, 
//				mCounters[i].mCounterQuery,  mCounters[i].mBuff, mUseIILName, rdata);
		AosQueryRsltObjPtr queryRslt;
		bool rslt = AosCounterClt::getSelf()->retrieveCounters2(
			mCounterId, mCounters[i].mRangeKeys, mCounters[i].mStatTypes, mCounters[i].mCounterQuery,
			mCounters[i].mBuff, queryRslt, mUseIILName, mQueryRsltOpr, rdata);
		aos_assert_rr(rslt, rdata, false);
		mCounters[i].mBuff->reset();
		if (!mCounters[i].mBuff || !rslt)
		{
			mNoMoreDocs = true;
			return true;
		}
		if(i == 0)
		{
			rslt = processData(mCounters[0],mCounters[1],true,rdata);
		}
		else
		{
			rslt = processData(mCounters[0],mCounters[1],false,rdata);
		}
		mNumValues = mCounters[i].mKeys.size();
		aos_assert_r(rslt, false);
	}

	if (!mOrderByValue) return true; 
	bool rslt = valueSort(rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosTermCounterCompare::valueSort(const AosRundataPtr &rdata)
{
	if(mComparedKeys.size() > 100000)
	{
		mOrderByValue = false;
		OmnAlarm << "mComparedKeys.size()>100000" <<enderr; 
		return true;
	}
	mArray->resetRecordLoop();
	char buff[mBuffLen];
	i64 start_pos;
	for (u32 i = 0; i< mComparedKeys.size(); i++)
	{
		aos_assert_r(mComparedKeys[i].length() < mBuffLen, false);
		strcpy(buff, mComparedKeys[i].data());	

		AosStatType::E stattype = mOrderByKey;
		start_pos = (eMaxCnameLen-1) + 0 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64*)&buff[start_pos] = mComparedValues[stattype][i];

		start_pos = (eMaxCnameLen-1) + 1 * sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64*)&buff[start_pos] = i;

		mArray->appendEntry(buff, mBuffLen, rdata.getPtr());
	}

	mArray->sort();
	mArray->resetRecordLoop();
	return true;
	
}

bool
AosTermCounterCompare::processData(struct counter &counterobj1, struct counter &counterobj2, const bool firstdata, const AosRundataPtr &rdata)
{
	// It is not time based. The 'mBuff' format should be:
	//          stattype[0] :[cname, value]
	//          			 [cname, value]
	//          			...
	//          stattype[1] :[cname, value]
	//          			 [cname, value]
	//          			 ...
	//          ...
	//          stattype[n] :[cname, value]
	//          			 [cname, value]
	//          			 ...

	// 1.two mehod process the data: process data1,  data2 and compared data
	// 2.check if the length of data1 and the length of data2 is equal.
	if(firstdata)
	{
		for (u32 i = 0; i<counterobj1.mStatTypes.size(); i++)
		{
			AosStatType::E stattype = counterobj1.mStatTypes[i];
			while(1)
			{
				OmnString cname = counterobj1.mBuff->getOmnStr("");
				if (cname == "") break;
				OmnString key = AosCounterUtil::getAllTerm2(cname);
				aos_assert_r(key !="", false);
				i64 value = counterobj1.mBuff->getI64(mDftValue);
				if (counterobj1.mIndex.count(key) == 0)
				{
					counterobj1.mValues[stattype].push_back(value);
					counterobj1.mIndex[key] = counterobj1.mValues[stattype].size() -1; 
					counterobj1.mKeys.push_back(key);
				}
				else
				{
					OmnAlarm << "there are no same keys!"<< enderr;
					return false;
				}
			}
		}
	}
	else
	{
		for (u32 i = 0; i<counterobj2.mStatTypes.size(); i++)
		{
			AosStatType::E stattype = counterobj2.mStatTypes[i];
			mComparedStatTypes.push_back(stattype);
			while(1)
			{
				OmnString cname = counterobj2.mBuff->getOmnStr("");
				if (cname == "") break;
				OmnString key = AosCounterUtil::getAllTerm2(cname);
				aos_assert_r(key !="", false);
				i64 value = counterobj2.mBuff->getI64(mDftValue);
				if (counterobj2.mIndex.count(key) == 0)
				{
					counterobj2.mValues[stattype].push_back(value);
					i64 counter2_length = counterobj2.mValues[stattype].size();
					aos_assert_r(counter2_length>=1, false);
					i64 value1 = counterobj1.mValues[stattype][counter2_length-1];
					mComparedValues[stattype].push_back(value1-value);
					counterobj2.mIndex[key] = counterobj2.mValues[stattype].size() -1; 
					counterobj2.mKeys.push_back(key);
					mComparedKeys.push_back(key);
				}
				else
				{
					OmnAlarm << "there are no same keys!"<< enderr;
					return false;
				}
			}
		}
		for (u32 i = 0; i<counterobj2.mStatTypes.size(); i++)
		{
			AosStatType::E stattype = counterobj1.mStatTypes[i];
			if(counterobj1.mValues[stattype].size() != counterobj2.mValues[stattype].size())
			{
				OmnAlarm << "the length of two counter is not equal"<< enderr;
				return false;
			}
		}

	}

	return true;
}


bool
AosTermCounterCompare::parse(
		const AosXmlTagPtr &term,
		const AosRundataPtr &rdata)
{
	// <statterm type="AOSTERMTYPE_COUNTERSUB" 
	// 			order="true|false" 
	// 			reverse="true|false"
	// 			sum="true|false"
	// 			aver="true|false"
	// 			max="true|false"
	// 			min="true|false"
	// 			factor="xxx"
	// 			zky_counter_id="xxx">
	// 		<counters>
	// 			<counter>
	// 				<zky_time_condition zky_timeformat="xxx" .../>
	// 				<aggregate  zky_aggregate_type='mergeEntries' zky_mergefield_index = '0' />
	//      		<counter_names>
	//      			...
	//      		</counter_names>
	// 			</counter>
	// 			<counter>
	// 				<zky_time_condition zky_timeformat="xxx" .../>
	// 				<aggregate  zky_aggregate_type='mergeEntries' zky_mergefield_index = '0' />
	//      		<counter_names>
	//      			...
	//      		</counter_names>
	// 			</counter>
	// 		</counters>
	// 		<compare method="add|subtract|multiply|divide" />
	//      <zky_time_condition/>
	//  </statterm>

	aos_assert_r(rdata, false);
	aos_assert_rr(term, rdata, false);

	mUseEpochTime = term->getAttrBool("use_epochtime", false);
	mQueryRsltOpr = term->getAttrStr("operator", "norm");

	// Retrieve Counter ID
	mCounterId = term->getAttrStr(AOSTAG_COUNTER_ID);
	if (mCounterId == "") mCounterId = term->getNodeText(AOSTAG_COUNTER_ID);
	if (mCounterId == "")
	{
		mIsGood = false;
		return false;
	}

	mCompare = term->getFirstChild("compare");
	aos_assert_rr(mCompare, rdata, false);
	mCompareMethod = mCompare->getAttrStr("method");
	if (mCompareMethod == "")
	{
		mIsGood = false;
		return false;
	}
	
	mUseIILName= term->getAttrBool("use_iilname", false);
	if (!mUseIILName)
	{
		mCounterId = AosIILName::composeCounterIILName(mCounterId);
	}

	//retrieve time conditions start
	AosXmlTagPtr counters = term->getFirstChild("counters");
	AosXmlTagPtr countertag = counters->getFirstChild("counter");
	i64 num_counter = 0;
	while(countertag)
	{
		num_counter++;
		struct counter counterobj;
		// Retrieve StatTypes
		AosStatType::retrieveStatTypes(counterobj.mStatTypes, term);
		AosTimeUtil::parseTimeNew(countertag, AOSTAG_TIME_CONDITION,
			     counterobj.mStartTime, counterobj.mEndTime, 
				 counterobj.mTimeGran, counterobj.mTimeFormat, 
				 mUseEpochTime, rdata);
		if (counterobj.mTimeGran == AosTimeGran::eInvalid) counterobj.mTimeGran = AosTimeGran::eNoTime;
		AosXmlTagPtr tt = countertag->getFirstChild(AOSTAG_TIME_CONDITION);
		if (tt)
		{
			counterobj.mTimeOpr = tt->getAttrStr("zky_timeopr", "and");
		}
		if (counterobj.mStartTime > counterobj.mEndTime) 
		{
			OmnAlarm << "missing cname or startime > endtime!"<< enderr;
	    	mIsGood = false;
	    	return false;
		}
		// Retrieve the counter name
		i64 level = 0;
		bool rslt = AosResolveCounterName::resolveCounterName2(
	        countertag, AOSTAG_COUNTERNAMES, counterobj.mCname, level, rdata);
		aos_assert_r(rslt, false);
		level = level + 2;

		if (term->getAttrInt64("zky_level", -1) >= 0)
		{
			level = term->getAttrInt64("zky_level", -1);
		}

		// compose Entry Type
		OmnString append_bit_flag = term->getAttrStr("append_bit", "false");
		if (append_bit_flag == "false")
		{
			append_bit_flag = term->getNodeText("append_bit");
		}

		i64 append_bit = (append_bit_flag == "true"?1:0); 
		counterobj.mEntryType = AosCounterUtil::composeTimeEntryType(level, append_bit, AosStatType::eInvalid, counterobj.mTimeGran);
		//Aggregate start 
		i64 fieldindex = 0;
		AosXmlTagPtr aggtag = countertag->getFirstChild("aggregate");
		if (aggtag)
		{
			OmnString aggregateType = aggtag->getAttrStr("zky_aggregate_type", "");
			if (aggregateType != "")
			{
				counterobj.mCounterQuery = OmnNew AosCounterQuery();
				counterobj.mCounterQuery->setAggregateType(aggregateType);
				fieldindex = aggtag->getAttrInt64("zky_mergefield_index", -1);
				counterobj.mCounterQuery->setMergeFieldIndex(fieldindex);  
				counterobj.mCounterQuery->setConvertEntriesConfig(aggtag);
			}
		}
		//Aggregate end 

		//get cname start
		if (!AosTime::isValidTimeRange(counterobj.mStartTime, counterobj.mEndTime)) 
		{
			//counterobj.mRangeKeys.push_back(counterobj.mCname);
			AosCounterUtil::composeTerm2(counterobj.mEntryType, "", counterobj.mCname, 0);
		}
		else
		{
			u64 crt_time = AosTime::convertUniTime(counterobj.mTimeFormat, counterobj.mTimeGran, counterobj.mStartTime, mUseEpochTime); 
			u64 end_time = AosTime::convertUniTime(counterobj.mTimeFormat, counterobj.mTimeGran, counterobj.mEndTime, mUseEpochTime);

			//if (counterobj.mCname != "")
			//{
			counterobj.mRangeKeys.push_back(AosCounterUtil::composeTerm2(counterobj.mEntryType, "", counterobj.mCname, crt_time));
			counterobj.mRangeKeys.push_back(AosCounterUtil::composeTerm2(counterobj.mEntryType, "", counterobj.mCname, end_time));
			//}
			//else
			//{
			//	OmnString name;
			//	name << crt_time;
			//	counterobj.mRangeKeys.push_back(name);
	
			//	name = "";
			//	name << end_time;
			//	counterobj.mRangeKeys.push_back(name);
			//}
		}

		//get cname end 
		
		mCounters.push_back(counterobj);
		countertag = counters->getNextChild();
	}
	mOrderByValue = term->getAttrBool(AOSTAG_ORDER_BY_VALUE, false);
	mReverse = term->getAttrBool("reverse", false);
	bool rslt = false;
	if (mOrderByValue)
	{
		rslt = createBuffArray(term, rdata);
		aos_assert_r(rslt, false);
	}

	aos_assert_rr(num_counter == 2, rdata, false);
	//retrieve time conditions end 
	return true;
}


bool
AosTermCounterCompare::createBuffArray(
		const AosXmlTagPtr &term,
		const AosRundataPtr &rdata)
{
	OmnString order_key = term->getAttrStr("zky_order_key", "sum");
	mOrderByKey = AosStatType::toEnum(order_key); 
	if (!AosStatType::isValid(mOrderByKey)) mOrderByKey = AosStatType::eSum;
	bool found = false;
	for (u32 i=0; i< mCounters[0].mStatTypes.size(); i++)
	{
		if (mCounters[0].mStatTypes[i] ==  mOrderByKey) found = true;
	}

	aos_assert_r(found, false);

	mBuffLen = eMaxCnameLen + sizeof(i64) * 2;
	OmnString config = "<zky_buffarray";
	config << " zky_stable = \"false\" >"
		<< "<zky_cmparefunc  "
		<< " cmpfun_type=\"custom\" "
		<< " cmpfun_size=\"" << mBuffLen << "\" "
		<< " cmpfun_alphabetic=\"true\" "
		<< " cmp_pos1 = \"" << eMaxCnameLen-1 << "\" "
		<< " cmp_datatype1 = \"i64\"  "
		<< " cmp_size1 = \"" << sizeof(i64) << "\" "
		<< " cmpfun_reserve=\"" << mReverse <<"\" "
		<< "/> </zky_buffarray>";
	AosXmlParser parser;
	AosXmlTagPtr array_tag = parser.parse(config, "" AosMemoryCheckerArgs);
	aos_assert_r(array_tag, false);
	mArray = OmnNew AosBuffArray(array_tag, rdata.getPtr());
	aos_assert_r(mArray, false);
	return true;
}


AosQueryTermObjPtr
AosTermCounterCompare::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterCompare(def, rdata);
}


