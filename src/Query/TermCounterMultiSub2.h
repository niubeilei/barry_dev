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
#ifndef Aos_Query_TermCounterMultiSub2_h
#define Aos_Query_TermCounterMultiSub2_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/Ptrs.h"
#include "CounterUtil/StatTypes.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"


class AosTermCounterMultiSub2 : public AosTermIILType
{
public:
	enum
	{
		eMaxCnameLen = 301,
		eMaxNumCounterId = 50 
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
    vector<OmnString>       mCounterIds;
	OmnString				mCname1;
	OmnString				mCname2;
	i64						mNumValues;
	AosCTime				mCTime;
	AosBuffPtr				mBuff;
    bool                	mDataLoaded;
	i64						mCrtIdx;
	vector<AosStatType::E>	mStatTypes;
	i64                 	mDftValue;
	vector<OmnString>		mKeys;
	map<OmnString, vector<i64> *>			mValues;
	i64                 	mAccumulates[eMaxNumCounterId][AosStatType::eMaxEntry];
	double 					mFactor;
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
	map<OmnString, u64>     mIndex;

public:
	AosTermCounterMultiSub2(const bool regflag);
	AosTermCounterMultiSub2(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermCounterMultiSub2();

	virtual bool nextDocid(
					const AosQueryTermObjPtr &parent_term,
					u64 &docid, 
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
	virtual AosXmlTagPtr    getDoc(const u64 &docid, const AosRundataPtr &rdata);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool loadData(const AosRundataPtr &rdata);

	virtual bool	queryFinished();

private:
	bool parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	AosXmlTagPtr createRecord(i64 &index, const AosRundataPtr &rdata);

	AosXmlTagPtr processRecordByOneDime(i64 &index, const AosRundataPtr &rdata);

	AosXmlTagPtr processRecordByTwoDime(i64 &index, const AosRundataPtr &rdata);

	bool getCounterNames(const u16 &entry_type, const OmnString &str_header, const AosRundataPtr &rdata);

	bool retrieveFilterCond(const AosXmlTagPtr &term);

	bool loadTimedData(const AosRundataPtr &rdata);
	
	bool processDataByOneDime(const AosRundataPtr &rdata);

	bool processDataByTwoDime(const AosRundataPtr &rdata);

	bool retrieveAggregateConf(const AosXmlTagPtr &term,  i64 &fieldindex);

	bool retrieveCounterIds(const AosXmlTagPtr &term);

	bool retrieveTwoDimeConf(const AosXmlTagPtr &term,  const i64 &fieldindex);

	bool retrieveCounterTimeConf(const AosXmlTagPtr &term,  const AosRundataPtr &rdata);

	bool findColumnKeys(map<OmnString, i64> &column, i64 &num_row_key, const AosRundataPtr &rdata);

	void retrieveBuffValue(OmnString &cname, i64 &value, const bool &timeorder,	const AosRundataPtr &rdata);
};
#endif

