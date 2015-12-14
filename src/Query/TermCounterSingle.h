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
// 06/22/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermCounterSingle_h
#define Aos_Query_TermCounterSingle_h

#include "Query/QueryTerm.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "CounterUtil/StatTypes.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"

class AosTermCounterSingle : public AosQueryTerm
{
private:
	i64						mStartTime;
	i64						mEndTime;
	AosTimeGran::E			mTimeGran;
	AosTime::TimeFormat		mTimeFormat;

	i64						mNumValues;
	vector<AosStatType::E>	mStatTypes;
	bool					mDataLoaded;
	u64						mCrtTime;
	i64						mCrtIdx;
	i64						mNumDocs;
	OmnString				mCounterId;
	OmnString				mCounterName;
	i64						mDftValue;
	vector<i64>				mValues[AosStatType::eMaxEntry];
	i64						mAccumulates[AosStatType::eMaxEntry];
	AosBuffPtr				mBuff;
	double					mFactor;
	bool					mAverage;

public:
	AosTermCounterSingle(const bool regflag);
	AosTermCounterSingle(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermCounterSingle();

	virtual bool nextDocid(
					const AosQueryTermObjPtr &parent_term,
					u64 &docid, 
					bool &finished, 
					const AosRundataPtr &rdata);
	virtual i64		getTotal(const AosRundataPtr &rdata);
	virtual bool	checkDocid(const u64 &docid, const AosRundataPtr &rdata);
	virtual void	reset(const AosRundataPtr &rdata);
	virtual bool 	moveTo(const i64 &startidx, const AosRundataPtr &rdata);
	virtual void	toString(OmnString &str);
	virtual bool    setOrder(
						const OmnString &container, 
						const OmnString &name, 
						const bool reverse, 
						const AosRundataPtr &);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	virtual bool 	getRsltSizeFromIIL(const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata); 
	virtual bool    loadData(const AosRundataPtr &rdata);
	virtual AosXmlTagPtr	getDoc(const u64 &docid, const AosRundataPtr &rdata);
	virtual bool    collectInfo(const AosRundataPtr &rdata);

	OmnString createValue(bool &finished);
	bool parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata);
	AosXmlTagPtr createRecord(
						const i64 &index, 
						const u64 &time, 
						const AosRundataPtr &rdata);

	// Chen Ding, 2014/01/29
	virtual bool getTimeCond(
						const AosRundataPtr &rdata, 
						const OmnString &time_fname, 
						bool &match,
						i32 &start_day, 
						i32 &end_day)
	{
		match = false;
		return true;
	}

private:
	double 		calculateAverages(const i64 &index);

};
#endif

