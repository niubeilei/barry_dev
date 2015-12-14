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
// 09/18/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Query/TermCounterSub2.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Conds/Condition.h"
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

#include <boost/array.hpp>


AosTermCounterSub2::AosTermCounterSub2(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_COUNTER_SUB2, AosQueryType::eCounterSub2, regflag),
mCounterId(""),
mNumValues(0),
mOrderByValue(false),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0),
mAverage(false),
mArray(0),
mUseIILName(false),
mCounterQuery(0),
mQueryRsltOpr("norm"),
mShowAccum(false),
mFilter(false)
{
}


AosTermCounterSub2::AosTermCounterSub2(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_COUNTER_SUB2, AosQueryType::eCounterSub2, false),
mCounterId(""),
mNumValues(0),
mOrderByValue(false),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0),
mAverage(false),
mArray(0),
mUseIILName(false),
mCounterQuery(0),
mQueryRsltOpr("norm"),
mShowAccum(false),
mFilter(false)
{
	// This constructor creates a new instance of the term. 'def' is the
	// XML definition of the term and the 'def' should be in fellow form:
	
	for (i64 i = 0; i<AosStatType::eMaxEntry; i++)
	{
	    mAccumulates[i] = 0;
	}
	mIsGood = parse(def, rdata);
}


AosTermCounterSub2::~AosTermCounterSub2()
{
}


bool 	
AosTermCounterSub2::nextDocid(
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

	if (AosTime::isValidTimeRange(mCTime.start_time, mCTime.end_time))
	{
		if (!mDataLoaded)
		{
			aos_assert_r(loadTimedData(rdata), false);
			mCrtIdx = 0;
			docid = 1;
			if (mNumValues == 0) docid = 0;
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
			if (mNumValues == 0) docid = 0;
			return true;
		}
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
AosTermCounterSub2::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		OmnString &value, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	value = "";
	return nextDocid(parent_term, docid, finished, rdata);
}


bool
AosTermCounterSub2::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermCounterSub2::getDocidsFromIIL(
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
AosTermCounterSub2::getDoc(
		const u64 &docid,
		const AosRundataPtr &rdata) 
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
	
	return  createRecord(mCrtIdx, rdata);
}


AosXmlTagPtr
AosTermCounterSub2::getDoc(
		const u64 &docid,
		const OmnString &k,
		const AosRundataPtr &rdata)
{
	return getDoc(docid, rdata);
}


AosXmlTagPtr
AosTermCounterSub2::createRecord(
		i64 &index,
		const AosRundataPtr &rdata)
{
	//if (mTwoDime.row_idx >= 0 && mTwoDime.col_idx >= 0)
	if (mTwoDime.row_idx >= 0) 
	{
		return processRecordByTwoDime(index, rdata);
	}
	return processRecordByOneDime(index, rdata);
}


AosXmlTagPtr
AosTermCounterSub2::processRecordByOneDime(
		i64 &index,
		const AosRundataPtr &rdata)
{
	i64 idx = index;
	if (mOrderByValue)
	{
		mArray->setRecordLoop(idx);

		char *buff;
		aos_assert_r(mArray->nextValue(&buff), NULL);
		OmnString kk(buff);
		i64 start_pos = (eMaxCnameLen -1) + 1 * sizeof(i64); 
		aos_assert_r(start_pos < mBuffLen, NULL);
		idx = *(i64 *)&buff[start_pos];

		aos_assert_r(idx >= 0 && idx < (i64)mKeys.size(), 0);
		aos_assert_r(mKeys[idx] == kk, 0);
	}

	aos_assert_r(idx >= 0 && idx < (i64)mKeys.size(), 0);
	OmnString cname = AosCounterUtil::getCounterName(mKeys[idx]);
	OmnString record = "<record ";
	//record << AOSTAG_CNAME << "=\"" << cname << "\" ";

	vector<OmnString> str;
	AosCounterUtil::splitTerm2(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}

	for (u32 i = 0; i< mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		OmnString name = "zky_";
		name << AosStatType::toString(stattype);

		aos_assert_r(idx < (i64)mValues[stattype].size(), 0);
		i64 v1 = mValues[stattype][idx];
		mAccumulates[stattype] += v1; 
		i64 v2 = mAccumulates[stattype];

		if (mFactor != 0.0)
		{
			double vv1 = v1 * mFactor;
			double vv2 = v2 * mFactor;
			record << name << "=\"" << vv1 << "\" ";
			if (mShowAccum)
			{
				name << "_accum";
				record << name << "=\"" << vv2 << "\" ";
			}
		}
		else
		{
			record << name << "=\"" << v1 << "\" ";
			if (mShowAccum)
			{
				name << "_accum";
				record << name << "=\"" << v2 << "\" ";
			}
		}
	}

	if (mAverage)
	{
		//avg = sum/count;
		AosStatType::E stattype = AosStatType::eAverage;	
		OmnString name  = "zky_";
		name << AosStatType::toString(stattype);
		double vv1 = calculateAverages(idx);
		record << name << "=\"" << vv1 << "\" ";
	}
	record << "/>";

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



AosXmlTagPtr
AosTermCounterSub2::processRecordByTwoDime(i64 &index, const AosRundataPtr &rdata)
{
	// The record format is:
	//  <record AOSTAG_CNAME="xxx" ...>
	//      <time AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	//      <time AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	//      ...
	//  </record>
	//  ...

	aos_assert_r(index >= 0 && index < (i64)mKeys.size(), 0);
	aos_assert_r(mTwoDime.col_keys.size() > 0, 0);
	OmnString cname = mKeys[index];
	//i64 row = mTwoDime.row_idx;
	//if (mTwoDime.row_idx > mTwoDime.col_idx) row = mTwoDime.row_idx - 1;
	//aos_assert_r(row >= 0, 0);

	//OmnString row_key = AosCounterUtil::getTerm2(cname, row);
	OmnString row_key = cname;
	u64 crtYIdx = 0; 
	OmnString records;
	i64 idx = mIndex[row_key];
	i64 crtPsize = 0;
	while (crtYIdx <= mTwoDime.col_keys.size() -1)
	{
		if (mTwoDime.col_psize > 0 && ++crtPsize > mTwoDime.col_psize) break;
		records << "<time " 
			<< AOSTAG_CNAME <<"=\"" << row_key << "\" ";
		vector<OmnString> str;
		AosCounterUtil::splitTerm2(mTwoDime.col_keys[crtYIdx], str);
		for (u32 i = 0; i< str.size(); i++)
		{
			records << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
		}

		for (u32 i = 0; i< mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			OmnString name = "zky_";
			name << AosStatType::toString(stattype);

			aos_assert_r(idx >= 0 && idx < (i64)mValues[stattype].size(), NULL);
			i64 v1 = mValues[stattype][idx];
			mAccumulates[stattype] += v1; 
			i64 v2 = mAccumulates[stattype];

			if (mFactor != 0.0)
			{
				double vv1 = v1 * mFactor;
				double vv2 = v2 * mFactor;
				records << name << "=\"" << vv1 << "\" ";
				if (mShowAccum)
				{
					name << "_accum";
					records << name << "=\"" << vv2 << "\" ";
				}
			}
			else
			{
				records << name << "=\"" << v1 << "\" ";
				if (mShowAccum)
				{
					name << "_accum";
					records << name << "=\"" << v2 << "\" ";
				}
			}
		}
		
		if (mAverage)
		{
			// avg = sum/count;
			AosStatType::E stattype = AosStatType::eAverage;
			OmnString name = "zky_";
			name << AosStatType::toString(stattype);
			double vv1 = calculateAverages(index);
			records << name << "=\"" << vv1 << "\" ";
		}

		records << "/>";
		idx ++;
		crtYIdx ++;
	}

	//OmnString cname = AosCounterUtil::getCounterNamePart(key);
	//cname = AosCounterUtil::getCounterName(cname);
	OmnString contents = "<record ";
	contents << AOSTAG_USING_XML_AS_RESULT << "=\"" << "true" << "\" ";
	//contents << AOSTAG_CNAME << "=\"" << cname << "\" ";

	//vector<OmnString> str;
	//AosCounterUtil::splitTerm2(cname, str);
	//for (u32 i = 0; i< str.size(); i++)
	//{
	//	contents << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	//}
	contents << AOSTAG_CNAME << "=\"" << cname << "\" ";

	for (u32 i=0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		if (mShowAccum)
		{
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
		}
		mAccumulates[stattype] = 0;
	}
	contents << ">" << records << "</record>";

	aos_assert_r(contents.length() < 20000000, 0);
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
	return 0;
}



bool 	
AosTermCounterSub2::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function moves the position
	// to 'pos' in the array. If pos is bigger than the size of the array, 
	// it returns false. Otherwise, it loads the data as needed, and changes
	// the position to 'pos'. 
	//
	// This is the default implementation (check AosQueryTerm::moveTo(pos, rdata).
	// If this is what you need, do not override this function.
	
	//OmnNotImplementedYet;
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
AosTermCounterSub2::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	if (!mDataLoaded)
	{
		if (AosTime::isValidTimeRange(mCTime.start_time, mCTime.end_time))
		{
			aos_assert_r(loadTimedData(rdata), false);
		}
		else
		{
			aos_assert_r(loadData(rdata), false);

		}
		mCrtIdx = -1;
	}
	return mNumValues;
}


void	
AosTermCounterSub2::reset(const AosRundataPtr &rdata)
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
AosTermCounterSub2::toString(OmnString &str)
{
	// This function converts the term into an XML.
	//aos_assert(mRequest);
	//str << mRequest->toString();
	str << "<counterSub " << AOSTAG_COUNTER_ID << "=\"" << mCounterId << "\" "
		<< "zky_cname1 " << "=\"" << mCname1 << "\" "
		<< "zky_cname2 " << "=\"" << mCname2 << "\" "
		<< "start_time" << "=\"" << mCTime.start_time << "\" "
		<< "end_time" << "=\"" << mCTime.end_time << "\" "
		<< "factor" << "=\"" << mFactor << "\" "
		<< "time_gran" << "=\"" << AosTimeGran::toChar(mCTime.time_gran) << "\" />";
}


bool
AosTermCounterSub2::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;

	mDataLoaded = true;
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mQueryRslt = AosQueryRsltObj::getQueryRsltStatic();
	bool rslt = AosCounterClt::getSelf()->retrieveCountersPrefix(
		mCounterId, mKeys, mStatTypes, mCounterQuery,
		mBuff, mQueryRslt, mUseIILName, mQueryRsltOpr, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (!mBuff || !rslt)
	{
		mNoMoreDocs = true;
		return true;
	}
	mBuff->reset();
	mKeys.clear();
	//if (mTwoDime.row_idx >= 0 && mTwoDime.col_idx >= 0)
	if (mTwoDime.row_idx >= 0) 
	{
		rslt = processDataByTwoDime(rdata);
	}
	else
	{
		rslt = processDataByOneDime(rdata);
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTermCounterSub2::processDataByOneDime(const AosRundataPtr &rdata)
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

	bool rslt = true;
	mCTime.resetCrtTime();
	OmnString cname;
	i64 value;
	for (u32 i = 0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		while(1)
		{
			//OmnString cname = mBuff->getStr("");
			retrieveBuffValue(cname, value, true, rdata);			
			if (cname == "") break;
			OmnString key = AosCounterUtil::getAllTerm2(cname);
			aos_assert_r(key !="", false);
			//i64 value = mBuff->getI64(mDftValue);
			if (mFilter)
			{
				aos_assert_r(mFilterCond, false);                                

				vector<OmnString> str;
				AosCounterUtil::splitTerm2(key, str);

				OmnString name = "termcounter_";
				for (u32 i = 0; i< str.size(); i++)
				{
					name = "termcounter_";
					name << "cname" << i;
					rdata->setArg1(name, str[i]);
				}

				name = "termcounter_";
				name << AosStatType::toString(stattype);
				rdata->setArg1(name, value);

				rslt = AosCondition::evalCondStatic(mFilterCond, rdata);
				if (!rslt) continue;
			}

			if (mIndex.count(key) == 0)
			{
				mValues[stattype].push_back(value);
				mIndex[key] = mValues[stattype].size() -1; 
				mKeys.push_back(key);
				for (u32 j = 0; j<mStatTypes.size(); j++) 
				{
					if (stattype != mStatTypes[j]) mValues[mStatTypes[j]].push_back(mDftValue);
				}
			}
			else
			{
				i64 index = mIndex[key];
				aos_assert_r(index >= 0 && index < (i64)mValues[stattype].size(), false);
				i64 vv = mValues[stattype][index];
				mValues[stattype][index] = vv + value;
			}
		}
	}

	mNumValues = mKeys.size();

	if (!mOrderByValue) return true; 
	rslt = valueSort(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTermCounterSub2::findColumnKeys(
		map<OmnString, i64> &column, 
		const AosRundataPtr &rdata)
{
	OmnString cname;
	i64 value;
	mCTime.resetCrtTime();
	for (u32 i = 0; i<mStatTypes.size(); i++)
	{
		while(1)
		{
			//OmnString cname = mBuff->getStr("");
			//if (cname == "") break;
			//mBuff->getI64(mDftValue);
			retrieveBuffValue(cname, value, !mTwoDime.row_order_flag, rdata);
			if (cname == "") break;

			cname = AosCounterUtil::getAllTerm2(cname);
			//OmnString col_key = AosCounterUtil::getTerm2(cname, mTwoDime.col_idx);
			OmnString col_key = AosCounterUtil::decomposeRemoveTerm(cname, mTwoDime.row_idx, false);
			aos_assert_r(col_key !="", false);
			if (column.count(col_key) == 0) 
			{
				column[col_key] = mTwoDime.col_keys.size();
				mTwoDime.col_keys.push_back(col_key);
			}
		}
	}
	aos_assert_r(column.size() >0, false);
	return true;
}


bool
AosTermCounterSub2::processDataByTwoDime(const AosRundataPtr &rdata)
{
	//In the statistics:
	//	epoch_week.dayofweek.call_duration
	//it will retrieve the following results for a given day d:
	//		[d.0.call_duration1, docid]
	//		[d.0.call_duration2, docid]
	//			...
	//		[d.1.call_duration2, docid]
	//		[d.1.call_duration2, docid]
	//			...
	//		[d.6.call_duration1, docid]
	//		[d.6.call_duration2, docid]
	//			...
	//		Or in other word, it is a two-demensional array.
	//		Sum of Two-Dimensional Results
	//		For the above statistics, if it is for a day range, it may generate too many results. Data should be assembled in IIL:
	//			[0.call_duration1, sum]
	//			[0.call_duration2, sum]
	//			...
	//			[1.call_duration1, sum]
	//			[1.call_duration2, sum]
	//			...
	//		Note that epoch_day is dropped off. 
	
	map<OmnString, i64> column;
	bool rslt = findColumnKeys(column, rdata);
	aos_assert_r(rslt, false);

	mBuff->reset();
	mCTime.resetCrtTime();
	u32 idx = 0;
	i64 value;
	OmnString cname;
	vector<OmnString> t_keys;
	map<OmnString, boost::array<i64, AosStatType::eMaxEntry> > t_accumulates;
	for (u32 i = 0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		while(1)
		{
			//OmnString cname = mBuff->getStr("");
			//if (cname == "") break;
			retrieveBuffValue(cname, value, mTwoDime.row_order_flag, rdata);
			if (cname == "") break;

			cname = AosCounterUtil::getAllTerm2(cname);
			//OmnString row_key = AosCounterUtil::decomposeRemoveTerm(cname, mTwoDime.col_idx, false);
			OmnString row_key = AosCounterUtil::getTerm2(cname, mTwoDime.row_idx);
			//OmnString col_key = AosCounterUtil::getTerm2(cname, mTwoDime.col_idx);
			OmnString col_key = AosCounterUtil::decomposeRemoveTerm(cname, mTwoDime.row_idx, false);
			aos_assert_r(row_key != "" && col_key != "", false);

			idx = 0;
			if (mIndex.count(row_key) == 0)
			{
				t_accumulates[row_key][stattype] = 0;
				t_keys.push_back(row_key);
				mIndex[row_key] = mValues[stattype].size();
				u32 index = column[col_key];
				aos_assert_r(index < column.size(), false);
				while(idx < index)
				{
					for (u32 j = 0; j < mStatTypes.size(); j++)
					{
						mValues[mStatTypes[j]].push_back(mDftValue);
					}
					idx ++;
				}
				aos_assert_rr(idx == index, rdata, false);

				//i64 value = mBuff->getI64(mDftValue);
				mValues[stattype].push_back(value);
				t_accumulates[row_key][stattype]+= value;

				for (u32 j = 0; j < mStatTypes.size(); j++)
				{
					if (stattype != mStatTypes[j]) mValues[mStatTypes[j]].push_back(mDftValue);
				}
				
				idx ++;
				while (idx < column.size())
				{
					for (u32 j = 0; j<mStatTypes.size(); j++)
					{
						mValues[mStatTypes[j]].push_back(mDftValue);
					}
					idx ++;
				}
			}
			else
			{
				//i64 vv = mBuff->getI64(mDftValue);
				idx = column[col_key] + mIndex[row_key];
				aos_assert_r(idx < (i64)mValues[stattype].size(), false);
				//mValues[stattype][idx] = vv;
				mValues[stattype][idx] = value;
				t_accumulates[row_key][stattype]+= value;
			}
		}
	}

	if (mFilter)
	{
		aos_assert_r(mFilterCond, false);                                

		map<OmnString, boost::array<i64, AosStatType::eMaxEntry> > ::iterator itr = t_accumulates.begin();;
		for (; itr != t_accumulates.end(); ++itr)
		{
			OmnString key = itr->first;
			aos_assert_r(key != "", false);

			vector<OmnString> str;
			AosCounterUtil::splitTerm2(key, str);

			OmnString name = "termcounter_";
			for (u32 i = 0; i< str.size(); i++)
			{
				name = "termcounter_";
				name << "cname" << i;
				rdata->setArg1(name, str[i]);
			}

			bool filter = true;
			for (u32 i = 0; i<mStatTypes.size(); i++)
			{
				AosStatType::E stattype = mStatTypes[i];
				boost::array<i64, AosStatType::eMaxEntry> vv = itr->second; 
				i64 value = vv[stattype];

				name = "termcounter_";
				name << AosStatType::toString(stattype);
				rdata->setArg1(name, value);

				rslt = AosCondition::evalCondStatic(mFilterCond, rdata);
				if (rslt) 
				{
					filter = false;
					break;
				}
			}

			if (!filter)
			{
				mKeys.push_back(key);
			}
		}
	}
	else
	{
		mKeys.swap(t_keys);
	}

	mNumValues = mKeys.size();
	return true;
}


void
AosTermCounterSub2::retrieveBuffValue(
			OmnString &cname, 
			i64 &value, 
			const bool &timeorder,
			const AosRundataPtr &rdata)
{
	if (mCTime.time_idx < 0 || !timeorder)
	{
		cname = mBuff->getOmnStr("");
		if (cname == "") return;
		value = mBuff->getI64(mDftValue);
		return;
	}

	//There are no more data in 'mBuff'. Need to generate a record
	//for each remaining time slot.
	aos_assert(AosTime::isValidTimeRange(mCTime.start_time, mCTime.end_time)); 

	cname = "";
	u64 end_time = AosTime::convertUniTime(mCTime.time_format, mCTime.time_gran, mCTime.end_time, mCTime.use_epochtime);
	if (mCTime.crt_time > end_time) return;

	i64 crtIdx = mBuff->getCrtIdx();
	cname = mBuff->getOmnStr("");

	OmnString str;
	str << mCTime.crt_time;
	value = mDftValue;
	if (cname == "")
	{
		if (mCTime.template_cname == "") 
		{
			u16 entry_type;
			OmnString key((char *)&entry_type, sizeof(u16));
			cname << key << str; 
			mCTime.template_cname = cname;
		}
		cname = AosCounterUtil::decomposeReplaceTerm(mCTime.template_cname, mCTime.time_idx, str);
		if (mCTime.crt_time != end_time) mBuff->setCrtIdx(crtIdx);

		aos_assert(cname!= "");
		mCTime.crt_time = AosTime::nextTime(mCTime.crt_time, 
			mCTime.time_gran, mCTime.time_format, mCTime.use_epochtime);
		return;
	}

	if (mCTime.template_cname == "") mCTime.template_cname = cname;
	OmnString timestr = AosCounterUtil::getTerm2(AosCounterUtil::getAllTerm2(cname), mCTime.time_idx);
	u64 tt = timestr.parseU64(0);

	if (mCTime.crt_time < tt && mCTime.crt_time <= end_time)
	{
		cname = AosCounterUtil::decomposeReplaceTerm(
				mCTime.template_cname, mCTime.time_idx, str);
		mBuff->setCrtIdx(crtIdx);
	}
	else
	{
		value = mBuff->getI64(mDftValue);
		if (mCTime.crt_time != tt)	return;
	}
	mCTime.crt_time = AosTime::nextTime(mCTime.crt_time, 
			mCTime.time_gran, mCTime.time_format, mCTime.use_epochtime);
}


bool
AosTermCounterSub2::valueSort(const AosRundataPtr &rdata)
{
	if(mKeys.size() > AosCounterUtil::eMaxPerCounterEntry)
	{
		mOrderByValue = false;
		OmnAlarm << "mKeys.size()>" << AosCounterUtil::eMaxPerCounterEntry <<enderr; 
		return true;
	}
	mArray->resetRecordLoop();
	char buff[mBuffLen];
	i64 start_pos;
	for (u32 i = 0; i< mKeys.size(); i++)
	{
		aos_assert_r(mKeys[i].length() < mBuffLen, false);
		strcpy(buff, mKeys[i].data());	

		AosStatType::E stattype = mOrderByKey;
		start_pos = (eMaxCnameLen-1) + 0 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64*)&buff[start_pos] = mValues[stattype][i];

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
AosTermCounterSub2::loadTimedData(const AosRundataPtr &rdata)
{
	//It has time. The 'mBuff' format should be:
	//			stattype[0] : [name+timestr, value]
	//						  [name+timestr, value]
	//						  ...
	//			stattype[1] : [name+timestr, value]
	//						  [name+timestr, value]
	//						  ...
	//			...
	//			stattype[n] : [name+timestr, value]
	//						  [name+timestr, value]
	//	Tf|$_|mCname|$$|20120501|$_|aa
	//	Tf|$_|mCname|$$|20120501|$_|bb
	//	...
	//	Tf|$_|mCname|$$|20120502|$_|aa
	//	Tf|$_|mCname|$$|20120502|$_|bb
	//	...
	//	Tk|$_|mCname|$$|20120501|$_|aa
	//	Tk|$_|mCname|$$|20120501|$_|bb
	//	...
	//	Tk|$_|mCname|$$|20120502|$_|aa
	//	Tk|$_|mCname|$$|20120502|$_|bb
	//	...
	if (mDataLoaded) return true;

	mDataLoaded = true;
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = false;
	if (mCTime.time_opr == "or")
	{
		rslt = AosCounterClt::getSelf()->retrieveCountersPrefix(
			mCounterId, mKeys, mStatTypes, mCounterQuery,
			mBuff, mQueryRslt, mUseIILName, mQueryRsltOpr, rdata);
	}
	else
	{
		rslt = AosCounterClt::getSelf()->retrieveCounters2(
			mCounterId, mKeys, mStatTypes, mCounterQuery, 
			mBuff, mQueryRslt, mUseIILName, mQueryRsltOpr, rdata);
	}
	aos_assert_rr(rslt, rdata, false);

	if (!mBuff || !rslt)
	{
		mNoMoreDocs = true;
		return true;
	}
	mBuff->reset();
	mKeys.clear();
	
	//if (mTwoDime.row_idx >= 0 && mTwoDime.col_idx >= 0)
	if (mTwoDime.row_idx >= 0)
	{
		rslt = processDataByTwoDime(rdata);
	}
	else
	{
		rslt = processDataByOneDime(rdata);
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTermCounterSub2::parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
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
	//      <counter_names>
	//      	...
	//      </counter_names>
	//     	<filter_condition type="zky_condition" zky_type="arith">
	//   	  	<zky_opr><![CDATA[>]]></zky_opr>
	//     		<zky_lhs></zky_lhs>
	//     		<zky_rhs></zky_rhs>
	//     	</filter_condition>
	//      <zky_time_condition/>
	//  </statterm>

	mIsGood = false;
	aos_assert_r(rdata, false);
	aos_assert_rr(term, rdata, false);

	mFactor = term->getAttrDouble("factor", 0.0);
	mAverage = term->getAttrBool(AOSSTATTYPE_AVERAGE, false);
	mUseIILName = term->getAttrBool("use_iilname", false);
	mQueryRsltOpr = term->getAttrStr("operator", "norm");
	mShowAccum = term->getAttrBool("show_accum", false);
	mFilter = term->getAttrBool("filter", false);

	// Retrieve Counter ID
	bool rslt = retrieveCounterId(term);
	aos_assert_r(rslt, false);

	// Retrieve StatTypes
	AosStatType::retrieveStatTypes(mStatTypes, term);

	// retrieve time config
	rslt = retrieveCounterTimeConf(term, rdata);
	aos_assert_r(rslt, false);

	// retrieve string header
	i64 level = 0;
	OmnString str_header = "";
	rslt = AosResolveCounterName::resolveCounterName2(
			term, "counter_header", str_header, level, rdata);
	aos_assert_r(rslt, false);

	// Retrieve the counter name
	rslt = AosResolveCounterName::resolveCounterName3(
	        term, AOSTAG_COUNTERNAMES, mCname1, mCname2, level, rdata);
	aos_assert_r(rslt, false);

	// compose Entry Type
	aos_assert_r(level >= 0, false);
	OmnString append_bit_flag = term->getAttrStr("append_bit", "false");
	if (append_bit_flag == "false")
	{
		append_bit_flag = term->getNodeText("append_bit");
	}

	i64 append_bit = (append_bit_flag == "true"?1:0); 
OmnScreen << "==============  append_bit: " << append_bit << endl;
	//mEntryType = AosCounterUtil::composeTimeEntryType(level, append_bit, AosStatType::eInvalid, mCTime.time_gran);
	u16 entry_type = AosCounterUtil::composeTimeEntryType(level, append_bit, AosStatType::eInvalid, mCTime.time_gran);

	//Aggregate Multiple 
	i64 fieldindex = -1;
	rslt = retrieveAggregateConf(term, fieldindex);
	aos_assert_r(rslt, false);

	//Two-Dimensional Results
	rslt = retrieveTwoDimeConf(term, fieldindex);
	aos_assert_r(rslt, false);

	// Retrieve order by value
	mOrderByValue = term->getAttrBool(AOSTAG_ORDER_BY_VALUE, false);
	//mReverse = term->getAttrBool("reverse", false);
	OmnString reverse_str = term->getAttrStr("reverse", "false");
	if (reverse_str == "false")
	{
		reverse_str = term->getNodeText("reverse");
	}
	mReverse = (reverse_str == "true"?true:false);
	if (mOrderByValue)
	{
		rslt = createBuffArray(term, rdata);
		aos_assert_r(rslt, false);
	}

	// retrieve counternames
	rslt = getCounterNames(entry_type, str_header, rdata);
	aos_assert_r(rslt, false);

	rslt = retrieveFilterCond(term);
	aos_assert_r(rslt, false);

	mIsGood = true;
	return true;
}

bool
AosTermCounterSub2::retrieveCounterTimeConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	mCTime.use_epochtime = term->getAttrBool("use_epochtime", false);
	bool rslt = AosTimeUtil::parseTimeNew(term, AOSTAG_TIME_CONDITION, mCTime.start_time, 
				mCTime.end_time, mCTime.time_gran, mCTime.time_format, mCTime.use_epochtime, 
				mCTime.time_opr, rdata);
	aos_assert_r(rslt, false);
	if (mCTime.start_time > mCTime.end_time)
	{
		OmnAlarm << "start_time(" << mCTime.start_time << ") > end_time(" << mCTime.end_time << ")"<< enderr;
	}
	aos_assert_r(mCTime.time_gran != AosTimeGran::eInvalid, false);
	AosXmlTagPtr termtime = term->getFirstChild(AOSTAG_TIME_CONDITION);
	aos_assert_r(termtime, false);
	mCTime.time_idx = termtime->getAttrInt64("zky_time_idx", -1);
	return true;
}

bool
AosTermCounterSub2::retrieveCounterId(const AosXmlTagPtr &term)
{
	mCounterId = term->getAttrStr(AOSTAG_COUNTER_ID);
	if (mCounterId == "") mCounterId = term->getNodeText(AOSTAG_COUNTER_ID);
	aos_assert_r(mCounterId != "", false);

	if (!mUseIILName)
	{
		mCounterId = AosIILName::composeCounterIILName(mCounterId);
	}
	return true;
}


bool
AosTermCounterSub2::retrieveAggregateConf(
		const AosXmlTagPtr &term, 
		i64 &fieldindex)
{
	fieldindex = -1;
	AosXmlTagPtr tag = term->getFirstChild("aggregate");
	if (!tag) return true;

	OmnString aggregateType = tag->getAttrStr("zky_aggregate_type", "");
	if (aggregateType  == "") return true;

	mCounterQuery = OmnNew AosCounterQuery();
	mCounterQuery->setAggregateType(aggregateType);

	fieldindex = tag->getAttrInt64("zky_mergefield_index", -1);
	mCounterQuery->setMergeFieldIndex(fieldindex);  
	
	mCounterQuery->setConvertEntriesConfig(tag);

	AosXmlTagPtr tt = tag->getFirstChild("filter_value");
	if (tt)
	{
		AosOpr opr = AosOpr_toEnum(tt->getAttrStr("zky_opr", "an"));
		OmnString value_str = tt->getNodeText();
		u64 value = value_str.toU64();
		mCounterQuery->setValueFilterCond(opr, value);
	}

	return true;
}


bool
AosTermCounterSub2::retrieveTwoDimeConf(
		const AosXmlTagPtr &term,
		const i64 &fieldindex)
{
	mTwoDime.row_idx = term->getAttrInt64("zky_x", -1);
	//mTwoDime.col_idx = term->getAttrInt64("zky_y", -1);
	//if (mTwoDime.row_idx > 0 && mTwoDime.col_idx > 0)
	//{
	//	aos_assert_r(mTwoDime.row_idx != fieldindex, false);
	//	aos_assert_r(mTwoDime.col_idx != fieldindex, false);
	//	if (mTwoDime.row_idx > fieldindex) mTwoDime.row_idx = mTwoDime.row_idx -1;
	//	if (mTwoDime.col_idx > fieldindex) mTwoDime.col_idx = mTwoDime.col_idx -1;
	//}
	if (mTwoDime.row_idx < 0) return true;
	aos_assert_r(mTwoDime.row_idx != fieldindex, false);
	if (fieldindex >= 0 && mTwoDime.row_idx > fieldindex) mTwoDime.row_idx = mTwoDime.row_idx -1;

	mTwoDime.col_psize = term->getAttrInt64("zky_ypsize", -1);
	if (mCTime.time_idx < 0) return true;

	mTwoDime.row_order_flag = false;
	if (mCTime.time_idx == mTwoDime.row_idx) mTwoDime.row_order_flag = true; 
	return true;
}


bool
AosTermCounterSub2::createBuffArray(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	OmnString order_key = term->getAttrStr("zky_order_key", "sum");
	mOrderByKey = AosStatType::toEnum(order_key); 
	if (!AosStatType::isValid(mOrderByKey)) mOrderByKey = AosStatType::eSum;
	bool found = false;
	for (u32 i=0; i< mStatTypes.size(); i++)
	{
		if (mStatTypes[i] ==  mOrderByKey) found = true;
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


bool
AosTermCounterSub2::getCounterNames(
		const u16 &entry_type, 
		const OmnString &str_header, 
		const AosRundataPtr &rdata)
{
	if (!AosTime::isValidTimeRange(mCTime.start_time, mCTime.end_time)) 
	{
		//mKeys.push_back(mCname);
		mKeys.push_back(AosCounterUtil::composeTerm2(entry_type, str_header, mCname1, 0));
		return true;
	}

	u64 crt_time = AosTime::convertUniTime(mCTime.time_format, mCTime.time_gran, mCTime.start_time, mCTime.use_epochtime); 
	u64 end_time = AosTime::convertUniTime(mCTime.time_format, mCTime.time_gran, mCTime.end_time, mCTime.use_epochtime);

	mKeys.push_back(AosCounterUtil::composeTerm2(entry_type, str_header, mCname1, crt_time));
	mKeys.push_back(AosCounterUtil::composeTerm2(entry_type, str_header, mCname2, end_time));
	return true;
}


bool
AosTermCounterSub2::retrieveFilterCond(const AosXmlTagPtr &term)
{
	if (!mFilter) return true;

	AosXmlTagPtr tag = term->getFirstChild("filter_condition");
	if (!tag) 
	{
		mFilter = false;
		return true;
	}

	mFilterCond = tag->clone(AosMemoryCheckerArgsBegin);	
	return true;
}


AosQueryTermObjPtr
AosTermCounterSub2::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterSub2(def, rdata);
}


bool	
AosTermCounterSub2::queryFinished()
{
	return mNoMoreDocs;
}


double
AosTermCounterSub2::calculateAverages(const i64 &index)
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
		OmnAlarm << "Invalid index: " << AosStatType::eSum << " : " << AosStatType::eCount << enderr;
		return 0;
	}
	aos_assert_r(index >= 0 && index < (i64)mValues[sumtype].size(), 0);
	i64 total = mValues[sumtype][index];
	i64 number = mValues[countype][index];
	if (total == 0 || number == 0) return 0;
	double value = (double) total /number;
	return value;
}


#if 0
AosXmlTagPtr
AosTermCounterSub2::processTimedRecord(i64 index, const AosRundataPtr &rdata)
{
	// The record format is:
	//  <record AOSTAG_CNAME="xxx" ...>
	//      <time AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	//      <time AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	//      ...
	//  </record>
	//  ...
	if (mOrderByValue)
	{
		char *buff;
		aos_assert_r(mArray->nextValue(&buff), NULL);
		OmnString kk(buff);
		i64 start_pos = (eMaxCnameLen -1) + 1 * sizeof(i64); 
		aos_assert_r(start_pos < mBuffLen, NULL);
		index = *(i64 *)&buff[start_pos];

		aos_assert_r(index >= 0 && index < (i64)mKeys.size(), 0);
		aos_assert_r(mKeys[index] == kk, 0);
	}

	aos_assert_r(index >= 0 && index < (i64)mKeys.size(), 0);
	OmnString key = mKeys[index];
	u64 end_time = AosTime::convertUniTime(mCTime.time_format, mCTime.time_gran, end_time, time_format); 
	u64 crt_time = AosTime::convertUniTime(time_format, mCTime.time_gran, mStartTime, time_format); 
	OmnString records;
	i64 idx = mIndex[key];
	while (crt_time <= end_time)
	{
		OmnString timeStr = AosTime::convertToStr(crt_time, mCTime.time_gran, time_format, time_format);
		records << "<time " << AOSTAG_CNAME << "=\"" << timeStr << "\" ";

		for (u32 i = 0; i< mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			OmnString name = "zky_";
			name << AosStatType::toString(stattype);

			aos_assert_r(idx >= 0 && idx < (i64)mValues[stattype].size(), NULL);
			i64 v1 = mValues[stattype][idx];
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
		
		if (mAverage)
		{
			// avg = sum/count;
			AosStatType::E stattype = AosStatType::eAverage;
			OmnString name = "zky_";
			name << AosStatType::toString(stattype);
			double vv1 = calculateAverages(index);
			records << name << "=\"" << vv1 << "\" ";
		}

		records << "/>";
		idx ++;
		crt_time = AosTime::nextTime(crt_time, mCTime.time_gran, time_format, time_format); 
	}

	// filter
	if (mFilter)
	{
		for (u32 i = 0; i< mStatTypes.size(); i++)
		{
			if (mAccumulates[mStatTypes[i]] != 0) break;
			if (i == mStatTypes.size()-1) return 0;
		}
	}

	OmnString cname = AosCounterUtil::getCounterNamePart(key);
	cname = AosCounterUtil::getCounterName(cname);
	OmnString contents = "<record ";
	contents << AOSTAG_USING_XML_AS_RESULT << "=\"" << "true" << "\" ";
	contents << AOSTAG_CNAME << "=\"" << cname << "\" ";

	vector<OmnString> str;
	AosCounterUtil::splitTerm2(cname, str);
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

	aos_assert_r(contents.length() < 20000000, 0);
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
	return 0;
}

bool
AosTermCounterSub2::processTimedData(const AosRundataPtr &rdata)
{
	u64 end_time = AosTime::convertUniTime(time_format, mCTime.time_gran, mCTime.end_time, time_format); 
	for (u32 i = 0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		while(1)
		{
			u64 crt_time = AosTime::convertUniTime(time_format, time_gran, mCTime.mStartTime, time_format); 
			OmnString cname = mBuff->getOmnStr("");
			if (cname == "") break;

			OmnString timestr =  AosCounterUtil::getTimePart1(cname);
			aos_assert_r(timestr != "", false);
			u64 unitime = timestr.parseU64(0);
			aos_assert_r(unitime, false);
			u64 tt = AosTime::convertUniTime(time_format, time_gran, unitime, time_format);

			OmnString key = AosCounterUtil::getCounterNamePart1(cname);
			if (mIndex.count(key) == 0)
			{
				mKeys.push_back(key);
				mIndex[key] = mValues[stattype].size();

				while (crt_time < tt && crt_time <= end_time)
				{
					mValues[stattype].push_back(mDftValue);
					crt_time = AosTime::nextTime(crt_time, time_gran, time_format, time_format); 
				}
				aos_assert_rr(crt_time <= end_time, rdata, false);
				aos_assert_rr(crt_time == tt, rdata, false);

				i64 vv = mBuff->getI64(mDftValue);
				mValues[stattype].push_back(vv);
				crt_time = AosTime::nextTime(crt_time, time_gran, time_format, time_format);
				while (crt_time <= end_time)
				{
					 mValues[stattype].push_back(mDftValue);
					 crt_time = AosTime::nextTime(crt_time, time_gran, time_format, time_format);
				}
			}
			else
			{
				i64 vv = mBuff->getI64(mDftValue);
				i64 index = mIndex[key];
				while (crt_time != tt && crt_time <= end_time)
				{
					index ++;
					crt_time = AosTime::nextTime(crt_time, time_gran, time_format, time_format);
				}

				if (index >= 0)
				{
					aos_assert_r(index < (i64)mValues[stattype].size(), false);
					mValues[stattype][index] = vv;
				}
			}
		}
	}
	mNumValues = (mKeys.size())/mStatTypes.size();
	if (!mOrderByValue) return true; 

	bool rslt = valueSortByTimed(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTermCounterSub2::valueSortByTimed(const AosRundataPtr &rdata)
{
	u64 end_time = AosTime::convertUniTime(mCTime.time_format, mCTime.time_gran, mCTime.end_time, mCTime.use_epochtime); 
	mArray->resetRecordLoop();
	char buff[mBuffLen];
	i64 start_pos;
	for (u32 i = 0;i<mKeys.size(); i++)
	{
		aos_assert_r(mKeys[i].length() < mBuffLen, false);
		strcpy(buff, mKeys[i].data());
		AosStatType::E stattype = mOrderByKey;
		i64 k = mIndex[mKeys[i]];
		i64 vv = 0;
		u64 crt_time = AosTime::convertUniTime(mCTime.time_format, mCTime.time_gran, mCTime.start_time, mCTime.use_epochtime);
		while(crt_time <= end_time && k >= 0 && k < (i64)mValues[stattype].size())
		{
			vv += mValues[stattype][k++]; 
			crt_time = AosTime::nextTime(crt_time, mCTime.time_gran, mCTime.time_format, mCTime.use_epochtime);
		}

		start_pos = (eMaxCnameLen-1) + 0 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64 *)&buff[start_pos] = vv;
		start_pos = (eMaxCnameLen-1) + 1 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64 *)&buff[start_pos] = i;
		mArray->addValue(buff, mBuffLen, rdata);
	}

	mArray->sort();
	return true;
}


#endif
