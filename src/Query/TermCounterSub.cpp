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
// 08/11/2011	Created by Lynch
////////////////////////////////////////////////////////////////////////////
#include "Query/TermCounterSub.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
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


AosTermCounterSub::AosTermCounterSub(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_COUNTER_SUB, AosQueryType::eCounterSub, regflag),
mCounterId(""),
mNumValues(0),
mStartTime(0),
mEndTime(0),
mOrderByValue(false),
mDataLoaded(false),
mCrtIdx(-1),
mDftValue(0),
mFactor(0.0)
{
}


AosTermCounterSub::AosTermCounterSub(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_COUNTER_SUB, AosQueryType::eCounterSub, false),
mCounterId(""),
mNumValues(0),
mStartTime(0),
mEndTime(0),
mOrderByValue(false),
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
	
	for (i64 i = 0; i<AosStatType::eMaxEntry; i++)
	{
	    mAccumulates[i] = 0;
	}
	mIsGood = parse(def, rdata);
}


AosTermCounterSub::~AosTermCounterSub()
{
}


bool 	
AosTermCounterSub::nextDocid(
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
AosTermCounterSub::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermCounterSub::getDocidsFromIIL(
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
AosTermCounterSub::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
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
AosTermCounterSub::createRecord(i64 &index, const AosRundataPtr &rdata)
{
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
	OmnString cname = AosCounterUtil::getCounterName(mKeys[index]);
	OmnString record = "<record ";
	record << AOSTAG_CNAME << "=\"" << cname << "\" ";

	vector<OmnString> str;
	AosCounterUtil::splitCounterName(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}

	for (u32 i = 0; i< mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		OmnString name = "zky_";
		name << AosStatType::toString(stattype);

		i64 v1 = mValues[stattype][index];
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

	// filter
	if (mFilter)
	{
		for (u32 i = 0; i< mStatTypes.size(); i++)
		{
			if (mValues[mStatTypes[i]][index] != 0) break;
			if (i == mStatTypes.size() -1) return 0;
		}
	}

	if (mAverage)
	{
		//avg = sum/count;
		AosStatType::E stattype = AosStatType::eAverage;	
		OmnString name  = "zky_";
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
	return 0;
}


AosXmlTagPtr
AosTermCounterSub::createTimedRecord(i64 &index, const AosRundataPtr &rdata)
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
	u64 end_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mEndTime, false); 
	u64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, false); 
	OmnString records;
	i64 idx = mIndex[key];
	while (crt_time <= end_time)
	{
		OmnString timeStr = AosTime::convertToStr(crt_time, mTimeGran, mTimeFormat, false);
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
		crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false); 
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
	return 0;
}

bool 	
AosTermCounterSub::moveTo(const i64 &pos, const AosRundataPtr &rdata)
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
AosTermCounterSub::getTotal(const AosRundataPtr &rdata)
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
AosTermCounterSub::reset(const AosRundataPtr &rdata)
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
AosTermCounterSub::toString(OmnString &str)
{
	// This function converts the term into an XML.
	//aos_assert(mRequest);
	//str << mRequest->toString();
	str << "<counterSub " << AOSTAG_COUNTER_ID << "=\"" << mCounterId << "\" "
		<< "zky_cname " << "=\"" << mCname << "\" "
		<< "start_time" << "=\"" << mStartTime << "\" "
		<< "end_time" << "=\"" << mEndTime << "\" "
		<< "factor" << "=\"" << mFactor << "\" "
		<< "time_gran" << "=\"" << AosTimeGran::toChar(mTimeGran) << "\" />";
}


bool
AosTermCounterSub::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;

	mDataLoaded = true;
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = AosRetrieveCounters(
			mCounterId, mCname, mStatTypes, mBuff, rdata);
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

	for (u32 i = 0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		while(1)
		{
			OmnString cname = mBuff->getOmnStr("");
			if (cname == "") break;
			OmnString key = AosCounterUtil::getCounterNamePart(cname);
			aos_assert_r(key !="", false);
			i64 value = mBuff->getI64(mDftValue);
			if (mIndex.count(key) == 0)
			{
				mValues[stattype].push_back(value);
				mIndex[key] = mValues[stattype].size() -1; 
				mKeys.push_back(key);
				for (u32 j = 0; j<mStatTypes.size(); j++) 
				{
					//if (i != j) mValues[mStatTypes[j]].push_back(mDftValue);
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
mArray->resetRecordLoop();
char *b;
while(mArray->nextValue(&b))
{
OmnScreen << "cname : " << b << " , vv : " << *(i64 *)&b[eMaxCnameLen-1] 
	<< " , index: " << *(i64 *)&b[(eMaxCnameLen-1) + 1 *sizeof(i64)] << endl;
}

	mArray->sort();

mArray->resetRecordLoop();
char *bb;
while(mArray->nextValue(&bb))
{
OmnScreen << "cname : " << bb << " , vv : " << *(i64 *)&bb[eMaxCnameLen-1] 
	<< " , index: " << *(i64 *)&bb[(eMaxCnameLen-1) + 1 *sizeof(i64)] << endl;
}
	mArray->resetRecordLoop();
	return true;
}



bool
AosTermCounterSub::loadTimedData(const AosRundataPtr &rdata)
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
	bool rslt = AosRetrieveCounters(mCounterId, mKeys, mStatTypes, mBuff, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (!mBuff || !rslt)
	{
		mNoMoreDocs = true;
		return true;
	}
	mBuff->reset();
	mKeys.clear();

	u64 end_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mEndTime, false); 
	for (u32 i = 0; i<mStatTypes.size(); i++)
	{
		AosStatType::E stattype = mStatTypes[i];
		while(1)
		{
			u64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, false); 
			OmnString cname = mBuff->getOmnStr("");
			if (cname == "") break;

			OmnString timestr =  AosCounterUtil::getTimePart1(cname);
			aos_assert_r(timestr != "", false);
			u64 unitime = timestr.parseU64(0);
			aos_assert_r(unitime, false);
			u64 tt = AosTime::convertUniTime(mTimeFormat, mTimeGran, unitime, false);

			OmnString key = AosCounterUtil::getCounterNamePart1(cname);
			if (mIndex.count(key) == 0)
			{
				mKeys.push_back(key);
				mIndex[key] = mValues[stattype].size();

				while (crt_time < tt && crt_time <= end_time)
				{
					mValues[stattype].push_back(mDftValue);
					crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false); 
				}
				aos_assert_rr(crt_time <= end_time, rdata, false);
				aos_assert_rr(crt_time == tt, rdata, false);

				i64 vv = mBuff->getI64(mDftValue);
				mValues[stattype].push_back(vv);
				crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
				while (crt_time <= end_time)
				{
					 mValues[stattype].push_back(mDftValue);
					 crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
				}
			}
			else
			{
				i64 vv = mBuff->getI64(mDftValue);
				i64 index = mIndex[key];
				while (crt_time != tt && crt_time <= end_time)
				{
					index ++;
					crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
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
		u64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, false);
		while(crt_time <= end_time && k >= 0 && k < (i64)mValues[stattype].size())
		{
			vv += mValues[stattype][k++]; 
			crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);
		}

		start_pos = (eMaxCnameLen-1) + 0 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64 *)&buff[start_pos] = vv;
		start_pos = (eMaxCnameLen-1) + 1 *sizeof(i64);
		aos_assert_r(start_pos < mBuffLen, false);
		*(i64 *)&buff[start_pos] = i;
		mArray->appendEntry(buff, mBuffLen, rdata.getPtr());
	}

mArray->resetRecordLoop();
char *b;
while(mArray->nextValue(&b))
{
OmnScreen << "cname : " << b << " , vv : " << *(i64 *)&b[eMaxCnameLen-1] 
	<< " , index: " << *(i64 *)&b[(eMaxCnameLen-1) + 1 *sizeof(i64)] << endl;
}

	mArray->sort();

mArray->resetRecordLoop();
char *bb;
while(mArray->nextValue(&bb))
{
OmnScreen << "cname : " << bb << " , vv : " << *(i64 *)&bb[eMaxCnameLen-1] 
	<< " , index: " << *(i64 *)&bb[(eMaxCnameLen-1) + 1 *sizeof(i64)] << endl;
}

mArray->resetRecordLoop();
	return true;
}


bool
AosTermCounterSub::parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
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
	//      <zky_time_condition/>
	//  </statterm>

	aos_assert_r(rdata, false);
	aos_assert_rr(term, rdata, false);

	mFactor = term->getAttrDouble("factor", 0.0);
	mFilter = term->getAttrBool("filter", false);

	// Retrieve Counter ID
	mCounterId = term->getAttrStr(AOSTAG_COUNTER_ID);
	if (mCounterId == "") mCounterId = term->getNodeText(AOSTAG_COUNTER_ID);
	if (mCounterId == "")
	{
		mIsGood = false;
		return false;
	}
	mCounterId = AosIILName::composeCounterIILName(mCounterId);
	
	// Retrieve the counter name
	bool rslt = AosResolveCounterName::resolveCounterName(
	        term, AOSTAG_COUNTERNAMES, mCname, false, rdata);
	if (!rslt || mCname == "")
	{
	    mIsGood = false;
	    return false;
	}

	// Retrieve StatTypes
	AosStatType::retrieveStatTypes(mStatTypes, term);
	mAverage = term->getAttrBool(AOSSTATTYPE_AVERAGE, false);
	
	AosTimeUtil::parseTimeNew(term, AOSTAG_TIME_CONDITION,
			     mStartTime, mEndTime, mTimeGran, mTimeFormat, false, rdata);

	// Retrieve order by value
	mOrderByValue = term->getAttrBool(AOSTAG_ORDER_BY_VALUE, false);
	mReverse = term->getAttrBool("reverse", false);

	if (mOrderByValue)
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
	}

//u64 timestr = 2923023502540800;
//OmnString dd = AosTime::convertToStr(timestr, mTimeGran, mTimeFormat);
//cout << dd.data();
	aos_assert_r(getCounterNames(rdata), false);
	return true;
}


bool
AosTermCounterSub::getCounterNames(const AosRundataPtr &rdata)
{
	if (AosTime::isValidTimeRange(mStartTime, mEndTime))
	{
		u64 crt_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mStartTime, false); 
		u64 end_time = AosTime::convertUniTime(mTimeFormat, mTimeGran, mEndTime, false);

		while(crt_time <= end_time)
		{
			OmnString name = AosCounterUtil::composeTimeContainerPart(mCname, crt_time);
			mKeys.push_back(name);
			crt_time = AosTime::nextTime(crt_time, mTimeGran, mTimeFormat, false);	
		}
	}
	return true;
}


AosQueryTermObjPtr
AosTermCounterSub::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterSub(def, rdata);
}


double
AosTermCounterSub::calculateAverages(const i64 &index)
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
