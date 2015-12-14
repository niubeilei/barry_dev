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
#ifndef Aos_Query_TermCounterMultiSub_h
#define Aos_Query_TermCounterMultiSub_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/StatTypes.h"
#include "SEInterfaces/Ptrs.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"


class AosTermCounterMultiSub : public AosTermIILType
{
	public:
		enum
		{
			eMaxCnameLen = 301,
			eMaxNumCounterId = 50 
		};
private:
    vector<OmnString>       mCounterIds;
	OmnString				mCname;
	i64						mNumValues;
    i64						mStartTime;
    i64						mEndTime;
	AosTimeGran::E          mTimeGran;
	AosTime::TimeFormat     mTimeFormat;
	//bool					mOrderByValue;
	AosBuffPtr				mBuff;
    bool                	mDataLoaded;
	i64						mCrtIdx;
	vector<AosStatType::E>	mStatTypes;
	i64                 	mDftValue;
	vector<OmnString>		mKeys;
	map<OmnString, vector<i64> *>			mValues;
	//vector<i64>			mValues[AosStatType::eMaxEntry];
	i64                 mAccumulates[eMaxNumCounterId][AosStatType::eMaxEntry];
	double 					mFactor;
	map<OmnString, u64>		mIndex;
	//bool					mAverage;
	AosBuffArrayPtr			mArray;
	i64						mBuffLen;
	AosStatType::E			mOrderByKey;
	bool					mUseEpochTime;
	bool					mUseIILName;
	OmnString 				mMember;


public:
	AosTermCounterMultiSub(const bool regflag);
	AosTermCounterMultiSub(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermCounterMultiSub();

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
	AosXmlTagPtr createTimedRecord(i64 &index, const AosRundataPtr &rdata);
	//double calculateAverages(const i64 &index);

	bool getCounterNames(const AosRundataPtr &rdata);
	bool loadTimedData(const AosRundataPtr &rdata);
};
#endif

