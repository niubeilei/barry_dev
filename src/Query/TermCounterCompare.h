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
// 11/28/2012	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermCounterCompare_h
#define Aos_Query_TermCounterCompare_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/Ptrs.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"
#include "CounterUtil/CounterQuery.h"


class AosTermCounterCompare : public AosTermIILType
{
	struct counter 
	{
		counter()
		:
    	mStartTime(0),
    	mEndTime(0),
		mTimeGran(AosTimeGran::eInvalid)
		{
		}
    	i64						mStartTime;
    	i64						mEndTime;
		AosTimeGran::E          mTimeGran;
		AosTime::TimeFormat     mTimeFormat;
		OmnString				mTimeOpr;
		u16						mEntryType;
		OmnString				mCname;
		AosBuffPtr				mBuff;
		vector<OmnString>		mRangeKeys;
		vector<OmnString>		mKeys;
		map<OmnString, u64>		mIndex;
		AosCounterQueryPtr 		mCounterQuery;
		vector<i64>				mValues[AosStatType::eMaxEntry];
		vector<AosStatType::E>	mStatTypes;
	};
	public:
		enum
		{
			eMaxCnameLen = 301
		};
private:
    OmnString           	mCounterId;
	i64						mNumValues;

	vector<struct counter> 	mCounters;
	

    bool                	mDataLoaded;
	i64						mCrtIdx;
	i64                 	mDftValue;
	bool					mUseIILName;
	bool					mUseEpochTime;
	bool					mNoMoreDocs;
	AosXmlTagPtr			mCompare;
	OmnString				mCompareMethod;
	vector<i64>				mComparedValues[AosStatType::eMaxEntry];
	vector<OmnString>		mComparedKeys;
	vector<AosStatType::E>	mComparedStatTypes;
	bool					mOrderByValue;
	AosBuffPtr				mBuff;
	AosBuffArrayPtr			mArray;
	AosStatType::E			mOrderByKey;
	i64						mBuffLen;
	OmnString               mQueryRsltOpr;


public:
	AosTermCounterCompare(const bool regflag);
	AosTermCounterCompare(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermCounterCompare();

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


private:
	bool parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	AosXmlTagPtr createRecord(i64 &index, const AosRundataPtr &rdata);

	bool processData(struct counter &counterobj1, struct counter &counterobj2, const bool firstdata, const AosRundataPtr &rdata);
	bool createBuffArray(const AosXmlTagPtr &term, const AosRundataPtr &rdata);
	bool valueSort(const AosRundataPtr &rdata);
};
#endif

