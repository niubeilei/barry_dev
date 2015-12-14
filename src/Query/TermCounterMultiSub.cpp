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
#include "Query/TermCounterMultiSub.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterQueryType.h"
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


AosTermCounterMultiSub::AosTermCounterMultiSub(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_COUNTER_MULTISUB, AosQueryType::eCounterMultiSub, regflag),
mNumValues(0),
mStartTime(0),
mEndTime(0),
//mOrderByValue(false),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0)
{
}


AosTermCounterMultiSub::AosTermCounterMultiSub(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_COUNTER_MULTISUB, AosQueryType::eCounterMultiSub, false),
mNumValues(0),
mStartTime(0),
mEndTime(0),
//mOrderByValue(false),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0)
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

	if (!AosTime::isValidTimeRange(mStartTime, mEndTime))
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


AosTermCounterMultiSub::~AosTermCounterMultiSub()
{
	map<OmnString, vector<i64> *>::iterator itr;	
	for (itr = mValues.begin(); itr != mValues.end(); itr ++)
	{
		OmnDelete [] itr->second;
	}
}


bool 	
AosTermCounterMultiSub::nextDocid(
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

	if (AosTime::isValidTimeRange(mStartTime, mEndTime))
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
AosTermCounterMultiSub::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermCounterMultiSub::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 03/16/2012
	//OmnShouldNeverComeHere;
	//if (!mDataLoaded) loadData(rdata);
	if (query_rslt) 
	{

		query_rslt->appendDocid(mNumValues);
		//query_rslt->setNumDocs(mNumValues);
		query_rslt->reset();
	}
	if(query_context)
	{
		query_context->setFinished(true);
	}
	return true;
}


AosXmlTagPtr
AosTermCounterMultiSub::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
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
	
	if (!AosTime::isValidTimeRange(mStartTime, mEndTime))
	{
		return  createRecord(mCrtIdx, rdata);
	}
	return createTimedRecord(mCrtIdx, rdata); 
}


AosXmlTagPtr
AosTermCounterMultiSub::createRecord(i64 &index, const AosRundataPtr &rdata)
{
	/*
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
	*/

	aos_assert_r(index >= 0 && index < (i64)mKeys.size(), 0);
	OmnString cname = AosCounterUtil::getCounterName(mKeys[index]);
	OmnString record = "<record ";
	record << AOSTAG_CNAME << "=\"" << cname << "\" ";

	vector<OmnString> str;
	AosCounterUtil::splitCounterName(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}

	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
		OmnString counter_id = mCounterIds[k];
		vector<i64> * values = mValues[counter_id];
		record << AOSTAG_COUNTER_ID << k << "=\"" << counter_id << "\" ";

		for (u32 i = 0; i< mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			OmnString name = "zky_";
			name << counter_id << k << "_" <<AosStatType::toString(stattype);

			i64 v1 = 0;
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

	/*
	if (mAverage)
	{
		//avg = sum/count;
		AosStatType::E stattype = AosStatType::eAverage;	
		OmnString name  = "zky_";
		name << AosStatType::toString(stattype);
		double vv1 = calculateAverages(index);
		record << name << "=\"" << vv1 << "\" ";
	}
	*/
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
	return 0;
}

AosXmlTagPtr
AosTermCounterMultiSub::createTimedRecord(i64 &index, const AosRundataPtr &rdata)
{
	// The record format is:
	//  <record AOSTAG_CNAME="xxx" ...>
	//      <time AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	//      <time AOSTAG_CNAME="timestr" AOSTAG_STATNAME="xxx" .../>
	//      ...
	//  </record>
	//  ...
	/*
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
	*/

	aos_assert_r(index >= 0 && index < (i64)mKeys.size(), 0);
	OmnString key = mKeys[index];
	OmnString records;

	i64 counterid_size = mCounterIds.size();
	aos_assert_r(counterid_size > 0, 0);

	i64 idxs[counterid_size];
	i64 accumulates[counterid_size][AosStatType::eMaxEntry];
	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
		for (i64 i = 0; i<AosStatType::eMaxEntry; i++)
		{
			accumulates[k][i] = 0;
		}
		OmnString index_key;
		index_key << k << "_" << key;
		idxs[k] = mIndex[index_key]; 
	}

	u64 end_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mEndTime, mUseEpochTime); 
	u64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, mUseEpochTime); 
	while (crt_time <= end_time)
	{
		OmnString timeStr = AosTime::convertToStr(crt_time, mTimeGran, mTimeFormat, mUseEpochTime);
		records << "<time " << AOSTAG_CNAME << "=\"" << timeStr << "\" ";
		if (mMember != "") records << AOSTAG_CNAME <<"1 =\"" << mMember << "\" ";

		for (u32 k = 0; k < mCounterIds.size(); k++)
		{
			OmnString counterid = mCounterIds[k];
			vector<i64> *values = mValues[counterid];

			for (u32 i = 0; i< mStatTypes.size(); i++)
			{
				AosStatType::E stattype = mStatTypes[i];
				OmnString name = "zky_";
				name << counterid << k << "_" << AosStatType::toString(stattype);

				i64 idx = idxs[k];
				//aos_assert_r(idx >= 0 && idx < (i64)values[stattype].size(), NULL);

				i64 v1 = 0;
				if (idx >= 0 && idx < (i64)values[stattype].size())
				{
					 v1 = values[stattype][idx];
				}
				accumulates[k][stattype] += v1; 
				i64 v2 = accumulates[k][stattype];

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
			idxs[k]++;
		}
		
		/*
		if (mAverage)
		{
			// avg = sum/count;
			AosStatType::E stattype = AosStatType::eAverage;
			OmnString name = "zky_";
			name << AosStatType::toString(stattype);
			double vv1 = calculateAverages(index);
			records << name << "=\"" << vv1 << "\" ";
		}
		*/

		records << "/>";
		crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, mUseEpochTime); 
	}

	OmnString cname = AosCounterUtil::getCounterNamePart(key);
	cname = AosCounterUtil::getCounterName(cname);
	OmnString contents = "<record ";
	contents << AOSTAG_USING_XML_AS_RESULT << "=\"" << "true" << "\" ";
	contents << AOSTAG_CNAME << "=\"" << cname << "\" ";

	vector<OmnString> str;
	AosCounterUtil::splitCounterName(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		contents << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}

	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
		contents << AOSTAG_COUNTER_ID << k << "=\"" << mCounterIds[k] <<"\" ";
		for (u32 i = 0; i<mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			i64 v1 = accumulates[k][stattype];
			OmnString name = "zky_";
			name << mCounterIds[k] << k << "_" 
				<< AosStatType::toString(stattype) << "_accum";
			if (mFactor != 0.0)
			{
				double vv1 = v1 * mFactor;
				if (stattype == AosStatType::eAverage && accumulates[k][AosStatType::eSum] && v1)
				{
					vv1 = accumulates[k][AosStatType::eSum]/v1 * mFactor;
				}
				contents << name << "=\"" << vv1 << "\" ";
			}
			else
			{
				contents << name << "=\"" << v1 << "\" ";
			}
			accumulates[k][stattype] = 0;
		}
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
	return 0;
}

bool 	
AosTermCounterMultiSub::moveTo(const i64 &pos, const AosRundataPtr &rdata)
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
AosTermCounterMultiSub::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
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
	return mNumValues;
}


void	
AosTermCounterMultiSub::reset(const AosRundataPtr &rdata)
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
AosTermCounterMultiSub::toString(OmnString &str)
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
		<< "zky_cname " << "=\"" << mCname << "\" "
		<< "start_time" << "=\"" << mStartTime << "\" "
		<< "end_time" << "=\"" << mEndTime << "\" "
		<< "factor" << "=\"" << mFactor << "\" "
		<< "time_gran" << "=\"" << AosTimeGran::toChar(mTimeGran) << "\" />";
}


bool
AosTermCounterMultiSub::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;

	aos_assert_r(mCname != "", false);
	mDataLoaded = true;
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = AosRetrieveCounters(
			        mCounterIds, mCname, mStatTypes, mBuff, mUseIILName, rdata);
	//bool rslt = AosCounterClt::getSelf()->retrieveCounters(
	//		mCounterIds, mCname, mStatTypes, mBuff, mUseIILName, rdata); 
	aos_assert_rr(rslt, rdata, false);
	if (!mBuff || !rslt)
	{
		mNoMoreDocs = true;
		return true;
	}
	mBuff->reset();

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

	map<OmnString, i64> temp;
	for (u32 k = 0; k < mCounterIds.size(); k++)
	{
		OmnString counterid = mCounterIds[k];
		aos_assert_r(counterid != "", false);
		vector<i64>* values = OmnNew vector<i64>[AosStatType::eMaxEntry];
		for (u32 i = 0; i<mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			while(1)
			{
				OmnString cname = mBuff->getOmnStr("");
				if (cname == "") break;
				OmnString key = AosCounterUtil::getCounterNamePart(cname);
				OmnString index_key;
				index_key << k <<"_"<< key;
				aos_assert_r(key !="", false);
				i64 value = mBuff->getI64(mDftValue);
				if (temp.count(key) == 0)
				{
					mKeys.push_back(key);
					temp[key] = 1;
				}
				if (mIndex.count(index_key) == 0)
				{
					values[stattype].push_back(value);
					mIndex[index_key] = values[stattype].size() -1; 

					for (u32 j = 0; j<mStatTypes.size(); j++) 
					{
						if (stattype != mStatTypes[j]) values[mStatTypes[j]].push_back(mDftValue);
					}
				}
				else
				{
					i64 index = mIndex[index_key];
					aos_assert_r(index >= 0 && index < (i64)values[stattype].size(), false);
					i64 vv = values[stattype][index];
					values[stattype][index] = vv + value;
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

	/*
	if (!mOrderByValue) return true;
	mArray->resetRecordLoop();
	char buff[mBuffLen];
	i64 start_pos;
	for (u32 i = 0; i< mKeys.size(); i++)
	{
		strcpy(buff, mKeys[i].data());	

		AosStatType::E stattype = mOrderByKey;
		start_pos = (eMaxCnameLen-1) + 0 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64*)&buff[start_pos] = mValues[stattype][i];

		start_pos = (eMaxCnameLen-1) + 1 * sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64*)&buff[start_pos] = i;

		mArray->addValue(buff, mBuffLen, rdata);
	}

	mArray->resetRecordLoop();
	mArray->sort();
	mArray->resetRecordLoop();
	*/
	return true;
}



bool
AosTermCounterMultiSub::loadTimedData(const AosRundataPtr &rdata)
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
	if (mMember != "")
	{
		rslt = AosRetrieveSubCountersBySingle(mCounterIds, mKeys, mStatTypes, mMember, mBuff, mUseIILName, rdata); 
	}
	else
	{
		rslt = AosRetrieveCounters(mCounterIds, mKeys, mStatTypes, mBuff, mUseIILName, rdata);
	}

	aos_assert_rr(rslt, rdata, false);
	if (!mBuff || !rslt)
	{
		mNoMoreDocs = true;
		return true;
	}
	mBuff->reset();
	mKeys.clear();
	map<OmnString, i64> temp;
	for (u32 k = 0; k < mCounterIds.size(); k ++)
	{
		OmnString counterid = mCounterIds[k];
		aos_assert_r(counterid != "", false);
		vector<i64>* values = OmnNew vector<i64>[AosStatType::eMaxEntry];
		u64 end_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mEndTime, mUseEpochTime); 
		for (u32 i = 0; i<mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			while(1)
			{
				u64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, mUseEpochTime); 
				OmnString cname = mBuff->getOmnStr("");
				if (cname == "") break;

				OmnString timestr =  AosCounterUtil::getTimePart1(cname);
				aos_assert_r(timestr != "", false);
				u64 unitime = timestr.parseU64(0);
				aos_assert_r(unitime, false);
				//u64 tt = AosTime::convertUniTime(mTimeFormat, mTimeGran, unitime, mUseEpochTime);
				u64 tt = unitime;

				OmnString key = AosCounterUtil::getCounterNamePart1(cname);
				if (temp.count(key) == 0)
				{
					mKeys.push_back(key);
					temp[key]= 1;
				}

				OmnString index_key;
				index_key << k <<"_"<< key;
				if (mIndex.count(index_key) == 0)
				{
					mIndex[index_key] = values[stattype].size();
					while (crt_time < tt && crt_time <= end_time)
					{
						values[stattype].push_back(mDftValue);
						crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, mUseEpochTime); 
					}
					aos_assert_rr(crt_time <= end_time, rdata, false);
					aos_assert_rr(crt_time == tt, rdata, false);

					i64 vv = mBuff->getI64(mDftValue);
					values[stattype].push_back(vv);
					crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, mUseEpochTime);
					while (crt_time <= end_time)
					{
						 values[stattype].push_back(mDftValue);
						 crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, mUseEpochTime);
					}
				}
				else
				{
					i64 vv = mBuff->getI64(mDftValue);
					i64 index = mIndex[index_key];
					while (crt_time != tt && crt_time <= end_time)
					{
						index ++;
						crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, mUseEpochTime);
					}

					if (index >= 0)
					{
						aos_assert_r(index < (i64)values[stattype].size(), false);
						values[stattype][index] = vv;
					}
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
	mNumValues = (mKeys.size())/mStatTypes.size();

	/*
	if (!mOrderByValue) return true;

	mArray->resetRecordLoop();
	char buff[mBuffLen];
	i64 start_pos;
	for (u32 i = 0;i<mKeys.size(); i++)
	{
		strcpy(buff, mKeys[i].data());
		AosStatType::E stattype = mOrderByKey;
		i64 k = mIndex[mKeys[i]];
		i64 vv = 0;
		u64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, mUseEpochTime);
		while(crt_time <= end_time && k >= 0 && k < (i64)mValues[stattype].size())
		{
			vv += mValues[stattype][k++]; 
			crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, mUseEpochTime);
		}

		start_pos = (eMaxCnameLen-1) + 0 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64 *)&buff[start_pos] = vv;
		start_pos = (eMaxCnameLen-1) + 1 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64 *)&buff[start_pos] = i;
		mArray->addValue(buff, mBuffLen, rdata);
	}
	mArray->resetRecordLoop();
	mArray->sort();
	mArray->resetRecordLoop();
	*/
	return true;
}


bool
AosTermCounterMultiSub::parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	// <statterm type="AOSTERMTYPE_COUNTERMULTISUB" 
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

	aos_assert_r(rdata, false);
	aos_assert_rr(term, rdata, false);

	mFactor = term->getAttrDouble("factor", 0.0);
	mUseEpochTime = term->getAttrBool("use_epochtime", false);
	mUseIILName= term->getAttrBool("use_iilname", false);

	mMember = term->getAttrStr(AOSTAG_MEMBER, "");                
	if (mMember == "") mMember = term->getNodeText(AOSTAG_MEMBER);

	// Retrieve Counter ID
	//mCounterId = term->getAttrStr(AOSTAG_COUNTER_ID);
	OmnString counterids = term->getAttrStr(AOSTAG_COUNTER_ID);
	if (counterids == "") counterids = term->getNodeText(AOSTAG_COUNTER_ID);
	OmnStrParser1 parser(counterids, ",");
	OmnString word;
	while ((word = parser.nextWord()) != "")
	{
		mCounterIds.push_back(word);
	}

	if (mCounterIds.size() == 0 || mCounterIds.size() > eMaxNumCounterId)
	{
		mIsGood = false;
		return false;
	}

	if (!mUseIILName)
	{
		for (u32 i = 0; i<mCounterIds.size(); i++)
		{
			mCounterIds[i] = AosIILName::composeCounterIILName(mCounterIds[i]);
		}
	}

	AosTimeUtil::parseTimeNew(term, AOSTAG_TIME_CONDITION,
			     mStartTime, mEndTime, mTimeGran, mTimeFormat, mUseEpochTime, rdata);
	
	// Retrieve the counter name
	bool rslt = AosResolveCounterName::resolveCounterName(
	        term, AOSTAG_COUNTERNAMES, mCname, false, rdata);
	if (!rslt ||(AosTime::isValidTimeRange(mStartTime, mEndTime) == false && mCname == ""))
	{
		OmnAlarm << "missing cname!"<< enderr;
		mIsGood = false;
		return false;
	}

	// Retrieve StatTypes
	AosStatType::retrieveStatTypes(mStatTypes, term);
	//mAverage = term->getAttrBool(AOSSTATTYPE_AVERAGE, false);
	


	// Retrieve order by value
	//mOrderByValue = term->getAttrBool(AOSTAG_ORDER_BY_VALUE, false);
	mReverse = term->getAttrBool("reverse", false);

	aos_assert_r(getCounterNames(rdata), false);
	return true;
}


bool
AosTermCounterMultiSub::getCounterNames(const AosRundataPtr &rdata)
{
	if (AosTime::isValidTimeRange(mStartTime, mEndTime))
	{
		u64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, mUseEpochTime); 
		u64 end_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mEndTime, mUseEpochTime);

		while(crt_time <= end_time)
		{
			OmnString name;
			if (mCname != "")
			{
				name = AosCounterUtil::composeTimeContainerPart(mCname, crt_time);
			}
			else
			{
				name << crt_time;
			}
			mKeys.push_back(name);
			crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, mUseEpochTime);	
		}
	}
	return true;
}


AosQueryTermObjPtr
AosTermCounterMultiSub::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterMultiSub(def, rdata);
}


/*
double
AosTermCounterMultiSub::calculateAverages(const i64 &index)
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
	if (total == 0 || number == 0) return true;
	double value = (double) total /number;
	return value;
}
*/
