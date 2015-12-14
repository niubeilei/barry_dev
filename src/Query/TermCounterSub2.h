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
// 09/18/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermCounterSub2_h
#define Aos_Query_TermCounterSub2_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/Ptrs.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"


class AosTermCounterSub2 : public AosTermIILType
{
public:
	enum
	{
		eMaxCnameLen = 301
	};

	struct AosCTime 
	{
		i64                     start_time;
		i64                     end_time;
		AosTimeGran::E          time_gran;
		AosTime::TimeFormat     time_format;
		bool					use_epochtime;
		OmnString				time_opr;

		i64						time_idx;
		u64						crt_time;
		OmnString				template_cname;

		AosCTime()
		:
		start_time(0),
		end_time(0),
		time_gran(AosTimeGran::eInvalid),
		crt_time(start_time),
		template_cname("")
		{
		}

		void resetCrtTime()
		{
			if (AosTime::isValidTimeRange(start_time, end_time))
			{
				crt_time = AosTime::convertUniTime(time_format, time_gran, start_time, use_epochtime);
			}
		}
	};

	struct AosTwoDime
	{
		i64                     row_idx;
		//i64                     col_idx;
		vector<OmnString>       col_keys;
		i64                     col_psize;
		bool					row_order_flag;
	};
private:
    OmnString           	mCounterId;
	OmnString				mCname1;
	OmnString				mCname2;
	i64						mNumValues;
	AosCTime				mCTime;
	bool					mOrderByValue;
	AosBuffPtr				mBuff;
    bool                	mDataLoaded;
	i64						mCrtIdx;
	vector<AosStatType::E>	mStatTypes;
	i64                 	mDftValue;
	vector<OmnString>		mKeys;
	vector<i64>				mValues[AosStatType::eMaxEntry];
	i64                 	mAccumulates[AosStatType::eMaxEntry];
	double 					mFactor;
	map<OmnString, u64>		mIndex;
	bool					mAverage;
	AosBuffArrayPtr			mArray;
	i64						mBuffLen;
	AosStatType::E			mOrderByKey;
	bool					mUseIILName;
	AosCounterQueryPtr 		mCounterQuery;
	AosTwoDime				mTwoDime;
	OmnString				mQueryRsltOpr;

	// Ken Lee,2013/07/31
	bool					mShowAccum;
	bool					mFilter;
	AosXmlTagPtr			mFilterCond;

public:
	AosTermCounterSub2(const bool regflag);
	AosTermCounterSub2(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermCounterSub2();

	virtual bool	nextDocid(
						const AosQueryTermObjPtr &parent_term,
						u64 &docid, 
						bool &finished, 
						const AosRundataPtr &rdata);

	virtual bool	nextDocid(
			        	const AosQueryTermObjPtr &parent_term,
						u64 &docid,
						OmnString &value,
						bool &finished,
						const AosRundataPtr &rdata);

	virtual bool	checkDocid(const u64 &docid, const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual i64		getTotal(const AosRundataPtr &rdata);
	virtual void	reset(const AosRundataPtr &rdata);
	virtual bool 	moveTo(const i64 &pos, const AosRundataPtr &rdata);
	virtual void	toString(OmnString &str);

	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const OmnString &k,
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const AosRundataPtr &rdata);

	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool loadData(const AosRundataPtr &rdata);

	virtual AosQueryRsltObjPtr getQueryData() const {return mQueryRslt;}
	virtual bool	queryFinished();

	virtual bool	withValue() const {return true;}

private:
	bool parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	bool parseAggregate(const AosXmlTagPtr &term);

	AosXmlTagPtr createRecord(i64 &index, const AosRundataPtr &rdata);

	AosXmlTagPtr processRecordByOneDime(i64 &index, const AosRundataPtr &rdata);

	AosXmlTagPtr processRecordByTwoDime(i64 &index, const AosRundataPtr &rdata);

	AosXmlTagPtr createTimedRecord(i64 &index, const AosRundataPtr &rdata);

	double calculateAverages(const i64 &index);

	bool getCounterNames(const u16 &entry_type, const OmnString &str_header, const AosRundataPtr &rdata);

	bool retrieveFilterCond(const AosXmlTagPtr &term);

	bool createBuffArray(const AosXmlTagPtr &term, const AosRundataPtr &rdata);

	bool loadTimedData(const AosRundataPtr &rdata);

	//bool valueSortByTimed(const AosRundataPtr &rdata);

	bool processDataByOneDime(const AosRundataPtr &rdata);

	bool processDataByTwoDime(const AosRundataPtr &rdata);

	bool valueSort(const AosRundataPtr &rdata);

	bool retrieveAggregateConf(const AosXmlTagPtr &term, i64 &fieldindex);

	bool retrieveCounterId(const AosXmlTagPtr &term);

	bool retrieveTwoDimeConf(const AosXmlTagPtr &term, const i64 &fieldindex);

	bool retrieveCounterTimeConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata);

	bool findColumnKeys(map<OmnString, i64> &column, const AosRundataPtr &rdata);

	void retrieveBuffValue(OmnString &cname, i64 &value, const bool &timeorder, const AosRundataPtr &rdata);
};
#endif

