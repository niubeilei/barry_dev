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
#include "Query/TermCounterMultiSub2.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Conds/Condition.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "SEUtil/IILName.h"
#include "Query/TermOr.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosTermCounterMultiSub2::AosTermCounterMultiSub2(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_COUNTER_MULTISUB2, AosQueryType::eCounterMultiSub2, regflag),
mNumValues(0),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0),
mQueryRsltOpr("norm"),
mShowAccum(false),
mFilter(false)
{
}


AosTermCounterMultiSub2::AosTermCounterMultiSub2(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_COUNTER_MULTISUB2, AosQueryType::eCounterMultiSub2, false),
mNumValues(0),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0),
mQueryRsltOpr("norm"),
mShowAccum(false),
mFilter(false)
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

	if (AosTime::isValidTimeRange(mCTime.start_time, mCTime.end_time))
	{
		for (u32 k = 0; k < mCounterIds.size(); k++)
		{
			for (i64 i = 0; i<AosStatType::eMaxEntry; i++)
			{
				mAccumulates[k][i] = 0;
			}
		}
	}
}


AosTermCounterMultiSub2::~AosTermCounterMultiSub2()
{
	map<OmnString, vector<i64> *>::iterator itr;	
	for (itr = mValues.begin(); itr != mValues.end(); itr ++)
	{
		OmnDelete [] itr->second;
	}
}


bool 	
AosTermCounterMultiSub2::nextDocid(
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
AosTermCounterMultiSub2::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermCounterMultiSub2::getDocidsFromIIL(
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
AosTermCounterMultiSub2::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
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
	
	//if (!AosTime::isValidTimeRange(mCTime.start_time, mCTime.end_time))
	//{
	//	return  createRecord(mCrtIdx, rdata);
	//}
	//return createTimedRecord(mCrtIdx, rdata); 
	return createRecord(mCrtIdx, rdata);
}


AosXmlTagPtr
AosTermCounterMultiSub2::createRecord(i64 &index, const AosRundataPtr &rdata)
{
	if (mTwoDime.row_idx >= 0)
	{
		return processRecordByTwoDime(index, rdata);
	}
	return processRecordByOneDime(index, rdata);
}


AosXmlTagPtr
AosTermCounterMultiSub2::processRecordByOneDime(i64 &idx, const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && idx < (i64)mKeys.size(), 0);
	OmnString key = mKeys[idx];

	aos_assert_r(mIndex.count(key) != 0, 0);
	i64 index = mIndex[key];
	OmnString cname = AosCounterUtil::getCounterName(key);
	OmnString record = "<record ";
	//record << AOSTAG_CNAME << "=\"" << cname << "\" ";

	vector<OmnString> str;
	AosCounterUtil::splitTerm2(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}

	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
		OmnString counter_id = mCounterIds[k];
		vector<i64> * values = mValues[counter_id];
		//record << AOSTAG_COUNTER_ID << k << "=\"" << counter_id << "\" ";

		for (u32 i = 0; i< mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			OmnString name = "zky_";
			name << counter_id << k << "_" <<AosStatType::toString(stattype);

			i64 v1 = mDftValue;
			if (index >= 0 && index < (i64)values[stattype].size())
			{
				v1 = values[stattype][index];
			}
			mAccumulates[k][stattype] += v1; 
			i64 v2 = mAccumulates[k][stattype];

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
AosTermCounterMultiSub2::processRecordByTwoDime(i64 &index, const AosRundataPtr &rdata)
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
	vector<OmnString> records;
	vector<i64> counterid_sum;
	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
		OmnString counter_id = mCounterIds[k];
		vector<i64> * values = mValues[counter_id];

		u64 crtYIdx = 0;
		i64 idx = index * mTwoDime.col_keys.size();
		i64 crtPsize = 0;
		while (crtYIdx <= mTwoDime.col_keys.size() -1)
		{
			if (mTwoDime.col_psize > 0 && ++crtPsize > mTwoDime.col_psize) break;
			if (crtYIdx >= records.size())
			{
				OmnString record =  "<time "; 
				record << AOSTAG_CNAME <<" =\"" << row_key << "\" ";

				vector<OmnString> str;
				AosCounterUtil::splitTerm2(mTwoDime.col_keys[crtYIdx], str);
				for (u32 i = 0; i< str.size(); i++)
				{
					record << AOSTAG_CNAME << i << "=\"" << str[i]<< "\" ";
				}
				records.push_back(record);
				aos_assert_r(records.size() -1 == crtYIdx, 0);

				counterid_sum.push_back(0);
				aos_assert_r(counterid_sum.size() -1 == crtYIdx, 0);
			}

			for (u32 i = 0; i< mStatTypes.size(); i++)
			{
				AosStatType::E stattype = mStatTypes[i];
				OmnString name = "zky_";
				name << counter_id << k << "_"<< AosStatType::toString(stattype);

				i64 v1 = mDftValue; 
				if (index >= 0 && index < (i64)values[stattype].size())
				{
					v1 = values[stattype][idx];
				}

				mAccumulates[k][stattype] += v1; 
				i64 v2 = mAccumulates[k][stattype];
				
				if (stattype == AosStatType::eSum) counterid_sum[crtYIdx] += v1;
				if (mFactor != 0.0)
				{
					double vv1 = v1 * mFactor;
					double vv2 = v2 * mFactor;
					records[crtYIdx] << name << "=\"" << vv1 << "\" ";
					if (mShowAccum)
					{
						name << "_accum";
						records[crtYIdx] << name << "=\"" << vv2 << "\" ";
					}
				}
				else
				{
					records[crtYIdx] << name << "=\"" << v1 << "\" ";
					if (mShowAccum)
					{
						name << "_accum";
						records[crtYIdx] << name << "=\"" << v2 << "\" ";
					}	
				}
			}

			idx ++;
			crtYIdx ++;
		}
	}

	OmnString contents = "<record ";
	contents << AOSTAG_USING_XML_AS_RESULT << "=\"" << "true" << "\" ";

	//vector<OmnString> str;
	//AosCounterUtil::splitCounterName(cname, str);
	//for (u32 i = 0; i< str.size(); i++)
	//{
	//	contents << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	//}
	contents << AOSTAG_CNAME << "=\"" << cname << "\" ";

	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
		contents << AOSTAG_COUNTER_ID << k << "=\"" << mCounterIds[k] <<"\" ";
		for (u32 i = 0; i<mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			if (mShowAccum)
			{
				i64 v1 = mAccumulates[k][stattype];
				OmnString name = "zky_";
				name << mCounterIds[k] << k << "_" 
					<< AosStatType::toString(stattype) << "_accum";
				if (mFactor != 0.0)
				{
					double vv1 = v1 * mFactor;
					contents << name << "=\"" << vv1 << "\" ";
				}
				else
				{
					contents << name << "=\"" << v1 << "\" ";
				}
			}
			mAccumulates[k][stattype] = 0;
		}
	}
	contents << ">";

	for (u32 i = 0; i<mTwoDime.col_keys.size(); i++)
	{
		contents << records[i] << "zky_sum" << "=\"" << counterid_sum[i] << "\" " << "/>";
	}

	contents << "</record>";

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
AosTermCounterMultiSub2::moveTo(const i64 &pos, const AosRundataPtr &rdata)
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
AosTermCounterMultiSub2::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	//if (!mDataLoaded) loadData(rdata);
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
AosTermCounterMultiSub2::reset(const AosRundataPtr &rdata)
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
AosTermCounterMultiSub2::toString(OmnString &str)
{
	// This function converts the term into an XML.
	OmnString counter_ids;
	for (u32 i = 0; i < mCounterIds.size(); i++)
	{
		if (counter_ids == "") 
		{
			counter_ids = mCounterIds[i];
		}
		else
		{
			counter_ids << "," << mCounterIds[i];
		}
	}

	str << "<counterMulitSub " << AOSTAG_COUNTER_ID << "=\"" << counter_ids << "\" "
		<< "zky_cname1 " << "=\"" << mCname1 << "\" "
		<< "zky_cname2 " << "=\"" << mCname2 << "\" "
		<< "start_time" << "=\"" << mCTime.start_time << "\" "
		<< "end_time" << "=\"" << mCTime.end_time << "\" "
		<< "factor" << "=\"" << mFactor << "\" "
		<< "time_gran" << "=\"" << AosTimeGran::toChar(mCTime.time_gran) << "\" />";
}


bool
AosTermCounterMultiSub2::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;

	mDataLoaded = true;
	aos_assert_r(mKeys.size() > 0, false);
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = AosCounterClt::getSelf()->retrieveCountersPrefix(
		mCounterIds, mKeys, mStatTypes, mCounterQuery,
		mBuff, mUseIILName, mQueryRsltOpr, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (!mBuff || !rslt)
	{
		mNoMoreDocs = true;
		return true;
	}
	mBuff->reset();
	mKeys.clear();

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
AosTermCounterMultiSub2::processDataByOneDime(const AosRundataPtr &rdata)
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
	
	//map<OmnString, i64> column;
	//bool rslt = findColumnKeys(column, rdata);
	//aos_assert_r(rslt, false);
	//mBuff->reset();

	mIndex.clear();
	OmnString cname;
	i64 value;
	vector<OmnString> t_keys;
	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
		mCTime.resetCrtTime();
		OmnString counterid = mCounterIds[k];
		aos_assert_r(counterid != "", false);
		vector<i64>* values = OmnNew vector<i64>[AosStatType::eMaxEntry];
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
				//nt64_t value = mBuff->getI64(mDftValue);
				if (mIndex.count(key) == 0)
				{
					t_keys.push_back(key);
					while (values[stattype].size() < t_keys.size())
					{
						for (u32 j = 0; j<mStatTypes.size(); j++) 
						{
							values[mStatTypes[j]].push_back(mDftValue);
						}
					}
					aos_assert_r(t_keys.size() == values[stattype].size(), false);
					//values[stattype].push_back(value);
					u32 idx = values[stattype].size() -1; 
					values[stattype][idx] = value;
					mIndex[key] = idx; 
				}
				else
				{
					u32 idx = mIndex[key];
					//aos_assert_r(idx >= 0 && idx < (i64)values[stattype].size(), false);
					while (values[stattype].size() <= idx)	
					{
						for (u32 j = 0; j<mStatTypes.size(); j++) 
						{
							values[mStatTypes[j]].push_back(mDftValue);
						}
					}
					i64 vv = values[stattype][idx];
					values[stattype][idx] = vv + value;
				}
			}
		}
		if (mValues.count(counterid) == 0)
		{
			mValues.insert(make_pair(counterid, values));
		}
		else
		{
			OmnDelete [] values;
		}
	}

	if (mFilter)
	{
		for (u32 e = 0; e < t_keys.size(); e++)
		{
			OmnString key = t_keys[e];
			aos_assert_r(key != "", false);
			aos_assert_r(mIndex.count(key) != 0, false);

			i64 index = mIndex[key];
			bool filter = true;
			for (u32 i = 0; i<mStatTypes.size(); i++)
			{
				AosStatType::E stattype = mStatTypes[i];
				for (u32 k = 0; k < mCounterIds.size(); k++)
				{
					OmnString counter_id = mCounterIds[k];
					vector<i64>* vv = mValues[counter_id];
					i64 value = mDftValue;
					if (index >= 0 && index < (i64)vv[stattype].size())
					{
						value = vv[stattype][index];
					}

					OmnString name = "termcounter_";
					name << mCounterIds[k] << "_" << AosStatType::toString(stattype);
					rdata->setArg1(name, value);
				}

				aos_assert_r(mFilterCond, false);                                
				bool rslt = AosCondition::evalCondStatic(mFilterCond, rdata);
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



bool
AosTermCounterMultiSub2::loadTimedData(const AosRundataPtr &rdata)
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
	rslt = AosCounterClt::getSelf()->retrieveCounters2(
		mCounterIds, mKeys, mStatTypes, mCounterQuery,
		mBuff, mUseIILName, mQueryRsltOpr, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (!mBuff || !rslt)
	{
		mNoMoreDocs = true;
		return true;
	}
	mBuff->reset();
	mKeys.clear();
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
AosTermCounterMultiSub2::findColumnKeys(
		map<OmnString, i64> &column, 
		i64	&num_row_key,
		const AosRundataPtr &rdata)
{
	map<OmnString, i64> row_map;
	OmnString cname;
	i64 value;
	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
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
				OmnString row_key = AosCounterUtil::getTerm2(cname, mTwoDime.row_idx);				
				if(row_map.count(row_key) == 0)
				{
					row_map.insert(make_pair(row_key, 1));
				}

				OmnString col_key = AosCounterUtil::decomposeRemoveTerm(cname, mTwoDime.row_idx, false);
				aos_assert_r(col_key !="", false);
				if (column.count(col_key) == 0) 
				{
					column[col_key] = mTwoDime.col_keys.size();
					mTwoDime.col_keys.push_back(col_key);
				}
			}
		}
	}
	aos_assert_r(column.size() >0, false);
	num_row_key = row_map.size();
	return true;
}


bool
AosTermCounterMultiSub2::processDataByTwoDime(const AosRundataPtr &rdata)
{
	map<OmnString, i64> column;
	i64 num_row_key = 0;
	bool rslt = findColumnKeys(column, num_row_key, rdata);
	aos_assert_r(rslt, false);
	mBuff->reset();

	i64 value = 0;
	OmnString cname;
	map<OmnString, i64> index;
	for (u32 k = 0; k < mCounterIds.size(); k ++)
	{
		mCTime.resetCrtTime();
		OmnString counterid = mCounterIds[k];
		aos_assert_r(counterid != "", false); 
		vector<i64>* values = OmnNew vector<i64>[AosStatType::eMaxEntry];
		for (u32 i = 0; i<mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			while (1)
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

				u32 idx = 0;
				if (index.count(row_key) == 0)
				{
					index[row_key] = column.size() * mKeys.size();
					mKeys.push_back(row_key);
					//index[row_key] = values[stattype].size();

					//while (values[stattype].size() < column.size() * mKeys.size())
					while (values[stattype].size() < column.size() * num_row_key)
					{
						for (u32 j = 0; j<mStatTypes.size(); j++)
						{
							values[mStatTypes[j]].push_back(mDftValue);
						}
					}
					//i64 value = mBuff->getI64(mDftValue);
					//i64 value = 0;
					//OmnString cname;
					idx = column[col_key] + index[row_key];
					values[stattype][idx]= value;
				}
				else
				{
					while (values[stattype].size() < column.size() + index[row_key])	
					{
						for (u32 j = 0; j<mStatTypes.size(); j++) 
						{
							values[mStatTypes[j]].push_back(mDftValue);
						}
					}
					//i64 vv = mBuff->getI64(mDftValue);
					idx = column[col_key] + index[row_key];
					//values[stattype][idx] = vv;
					values[stattype][idx] = value;
				}
			}
		}
		if (mValues.count(counterid) == 0)
		{
			mValues.insert(make_pair(counterid, values));
		}
		else
		{
			OmnDelete [] values;
		}
	}
	mNumValues = mKeys.size();
	return true;
}


void
AosTermCounterMultiSub2::retrieveBuffValue(
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
AosTermCounterMultiSub2::parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	// <statterm type="AOSTERMTYPE_COUNTERMULTISUB2" 
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
	//      <zky_time_condition/>
	//  </statterm>

	mIsGood = false;
	aos_assert_r(rdata, false);
	aos_assert_rr(term, rdata, false);

	mFactor = term->getAttrDouble("factor", 0.0);
	mUseIILName= term->getAttrBool("use_iilname", false);
	mQueryRsltOpr = term->getAttrStr("operator", "norm");
	mShowAccum = term->getAttrBool("show_accum", false);
	mFilter = term->getAttrBool("filter", false);

	//mMember = term->getAttrStr(AOSTAG_MEMBER, "");                
	//if (mMember == "") mMember = term->getNodeText(AOSTAG_MEMBER);

	// Retrieve Counter ID
	bool rslt = retrieveCounterIds(term);
	aos_assert_r(rslt, false);

	// Retrieve StatTypes
	AosStatType::retrieveStatTypes(mStatTypes, term);

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

	// Retrieve order by value
	mReverse = term->getAttrBool("reverse", false);

	// compose Entry Type
	aos_assert_r(level >= 0, false);
	//bool append_bit_flag = term->getAttrBool("append_bit", false);
	//i64 append_bit = (append_bit_flag?1:0);
	OmnString append_bit_flag = term->getAttrStr("append_bit", "false");
	if (append_bit_flag == "false")
	{
		append_bit_flag = term->getNodeText("append_bit");
	}
	i64 append_bit = (append_bit_flag == "true"?1:0); 

OmnScreen << "==============  append_bit: " << append_bit << endl;
	//mEntryType = AosCounterUtil::composeTimeEntryType(level, 0, AosStatType::eInvalid, mCTime.time_gran);
	u16 entry_type = AosCounterUtil::composeTimeEntryType(level, append_bit, AosStatType::eInvalid, mCTime.time_gran);

	//Aggregate Multiple 
	i64 fieldindex = -1;
	rslt = retrieveAggregateConf(term, fieldindex);
	aos_assert_r(rslt, false);

	//Two-Dimensional Results
	rslt = retrieveTwoDimeConf(term, fieldindex);
	aos_assert_r(rslt, false);

	rslt = getCounterNames(entry_type, str_header, rdata);
	aos_assert_r(rslt, false);

	rslt = retrieveFilterCond(term);
	aos_assert_r(rslt, false);

	mIsGood = true;
	return true;
}


bool
AosTermCounterMultiSub2::retrieveCounterTimeConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
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
AosTermCounterMultiSub2::retrieveCounterIds(const AosXmlTagPtr &term)
{
	OmnString counterids = term->getAttrStr(AOSTAG_COUNTER_ID);
	if (counterids == "") counterids = term->getNodeText(AOSTAG_COUNTER_ID);
	OmnStrParser1 parser(counterids, ",");
	OmnString word;
	while ((word = parser.nextWord()) != "")
	{
		if (!mUseIILName) 
		{
			word = AosIILName::composeCounterIILName(word);
		}
		mCounterIds.push_back(word);
	}

	aos_assert_r(mCounterIds.size() > 0 && mCounterIds.size() <= eMaxNumCounterId, false);
	return true;
}


bool
AosTermCounterMultiSub2::retrieveAggregateConf(
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
	return true;
}


bool
AosTermCounterMultiSub2::retrieveTwoDimeConf(
		const AosXmlTagPtr &term,
		const i64 &fieldindex)
{
	//mTwoDime.row_idx = term->getAttrInt64("zky_x", -1);
	OmnString row_idx_str = term->getAttrStr("zky_x", "");
	if (row_idx_str == "") 
	{
		row_idx_str = term->getNodeText("zky_x");
	}
	if (row_idx_str == "")
	{
		mTwoDime.row_idx = -1; 
	}
	else
	{
		mTwoDime.row_idx = atoi(row_idx_str.data()); 
	}

	if (mTwoDime.row_idx < 0) return true;
	//mTwoDime.col_idx = term->getAttrInt64("zky_y", -1);
	//if (mTwoDime.row_idx > 0 && mTwoDime.col_idx > 0)
	//{
	//	aos_assert_r(mTwoDime.row_idx != fieldindex, false);
	//	aos_assert_r(mTwoDime.col_idx != fieldindex, false);
	//	if (mTwoDime.row_idx > fieldindex) mTwoDime.row_idx = mTwoDime.row_idx -1;
	//	if (mTwoDime.col_idx > fieldindex) mTwoDime.col_idx = mTwoDime.col_idx -1;
	//}
	aos_assert_r(mTwoDime.row_idx != fieldindex, false);
	if (fieldindex >= 0 && mTwoDime.row_idx > fieldindex) mTwoDime.row_idx = mTwoDime.row_idx -1;

	mTwoDime.col_psize = term->getAttrInt64("zky_ypsize", -1);
	if (mCTime.time_idx < 0) return true;

	mTwoDime.row_order_flag = false;
	if (mCTime.time_idx == mTwoDime.row_idx) mTwoDime.row_order_flag = true; 
	return true;
}


bool
AosTermCounterMultiSub2::getCounterNames(
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
AosTermCounterMultiSub2::retrieveFilterCond(const AosXmlTagPtr &term)
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
AosTermCounterMultiSub2::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterMultiSub2(def, rdata);
}


bool	
AosTermCounterMultiSub2::queryFinished()
{
	return mNoMoreDocs;
}

